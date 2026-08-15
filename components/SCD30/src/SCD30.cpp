/*
 This is a library written for the SCD30
 SparkFun sells these at its website: www.sparkfun.com
 Do you like this library? Help support SparkFun. Buy a board!
 https://www.sparkfun.com/products/14751

 Written by Nathan Seidle @ SparkFun Electronics, May 22nd, 2018

 Updated February 1st 2021 to include some of the features of paulvha's version of the library
 (while maintaining backward-compatibility):
 https://github.com/paulvha/scd30
 Thank you Paul!

 The SCD30 measures CO2 with accuracy of +/- 30ppm.

 This library handles the initialization of the SCD30 and outputs
 CO2 levels, relative humidty, and temperature.

 https://github.com/sparkfun/SparkFun_SCD30_Arduino_Library

 Development environment specifics:
 Arduino IDE 1.8.13

 SparkFun code, firmware, and software is released under the MIT License.
 Please see LICENSE.md for more details.

 adapted for non-arduino ESP32 idf 5
 */

//#define TESTPOINTS
#ifdef TESTPOINTS 
#define RS485DE_PIN     GPIO_NUM_6   // CANtx      
#define RS485TX_PIN     GPIO_NUM_44  // RX0  
#include "driver/gpio.h"  
#endif 

#define TAG "SDC30"

#define SCD30CLK 50000 // 50khz recommended SCD 30!

#include "driver/i2c_master.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include <stdbool.h>
#include <stdint.h>
#include <string.h>

#if defined(__has_include)
#if __has_include(<esp_log.h>)

#define LOG_LOCAL_LEVEL ESP_LOG_NONE

#include <esp_log.h>


#elif __has_include("esp_log.h")
#include "esp_log.h"
#else
typedef int esp_err_t;
#define ESP_OK 0
#define ESP_ERR_INVALID_ARG 0x102
#define ESP_ERR_NOT_FINISHED 0x104
#define ESP_ERR_TIMEOUT 0x105
#define ESP_ERR_INVALID_CRC 0x106
#define ESP_LOGI(tag, fmt, ...)
#define ESP_LOGE(tag, fmt, ...)
static inline const char *esp_err_to_name(esp_err_t err) { return ""; }
#endif
#else
#include <esp_log.h>
#endif

#if defined(__has_include)
#if __has_include(<driver/i2c_master.h>)
#include <driver/i2c_master.h>
#elif __has_include("driver/i2c_master.h")
#include "driver/i2c_master.h"
#else
typedef void *i2c_master_dev_handle_t;
typedef void *i2c_master_bus_handle_t;
typedef int i2c_port_t;
typedef struct {
	int dev_addr_length;
	int device_address;
	int scl_speed_hz;
} i2c_device_config_t;
extern esp_err_t i2c_master_bus_add_device(i2c_master_bus_handle_t, const i2c_device_config_t *, i2c_master_dev_handle_t *);
extern esp_err_t i2c_master_transmit(i2c_master_dev_handle_t, uint8_t *, size_t, int);
extern esp_err_t i2c_master_receive(i2c_master_dev_handle_t, uint8_t *, size_t, int);
#endif
#else
#include <driver/i2c_master.h>
#endif

#define I2C_TIMEOUT_MS 200

#define SCD30_ADDRESS 0x61

#define COMMAND_CONTINUOUS_MEASUREMENT 0x0010
#define COMMAND_SET_MEASUREMENT_INTERVAL 0x4600
#define COMMAND_GET_DATA_READY 0x0202
#define COMMAND_READ_MEASUREMENT 0x0300
#define COMMAND_AUTOMATIC_SELF_CALIBRATION 0x5306
#define COMMAND_SET_FORCED_RECALIBRATION_FACTOR 0x5204
#define COMMAND_SET_TEMPERATURE_OFFSET 0x5403
#define COMMAND_SET_ALTITUDE_COMPENSATION 0x5102
#define COMMAND_RESET 0xD304
#define COMMAND_STOP_MEAS 0x0104
#define COMMAND_READ_FW_VER 0xD100

#define SIMULATEINTERVAL 50
//extern SemaphoreHandle_t I2CSemaphore; // used by lvgl touch, shares the same bus


typedef union {
	uint8_t array[4];
	float value;
} ByteToFl;

class SCD30 {
public:
	SCD30(void);

	esp_err_t begin(i2c_master_bus_handle_t I2CbusHandle, bool autoCalibrate = false, bool measBegin = true);

	esp_err_t beginMeasuring(uint16_t pressureOffset);
	esp_err_t beginMeasuring(void);
	esp_err_t StopMeasurement(void);

	esp_err_t setAmbientPressure(uint16_t pressure_mbar);

	esp_err_t getSettingValue(uint16_t registerAddress, uint16_t *val);
	esp_err_t getFirmwareVersion(uint16_t *val) { return (getSettingValue(COMMAND_READ_FW_VER, val)); }
	uint16_t getCO2(void);
	float getHumidity(void);
	float getTemperature(void);

	uint16_t getMeasurementInterval(void);
	bool getMeasurementInterval(uint16_t *val);
	esp_err_t setMeasurementInterval(uint16_t interval);

	uint16_t getAltitudeCompensation(void);
	bool getAltitudeCompensation(uint16_t *val);
	esp_err_t setAltitudeCompensation(uint16_t altitude);

	bool getAutoSelfCalibration(void);
	esp_err_t setAutoSelfCalibration(bool enable);

	bool getForcedRecalibration(uint16_t *val);
	esp_err_t setForcedRecalibrationFactor(uint16_t concentration);

	float getTemperatureOffset(void);
	bool getTemperatureOffset(uint16_t *val);
	esp_err_t setTemperatureOffset(float tempOffset);

	bool dataAvailable();
	esp_err_t readMeasurement();

	esp_err_t reset();

	esp_err_t sendCommand(uint16_t command, uint16_t arguments);
	esp_err_t sendCommand(uint16_t command);

	esp_err_t readRegister(uint16_t registerAddress, uint16_t *data);

	uint8_t computeCRC8(uint8_t data[], uint8_t len);

private:
	i2c_port_t _i2cPort;

	float co2;
	float temperature;
	float humidity;

	bool co2HasBeenReported;
	bool humidityHasBeenReported;
	bool temperatureHasBeenReported;

	bool simulate;
	int simulateTmr;
};

i2c_master_dev_handle_t sps30DevHandle;

esp_err_t swapBytes(uint8_t *src, int nrBytes) {
	uint8_t bytes[8];
	if (nrBytes < 8) {
		memcpy(bytes, src, nrBytes);
		for (int n = nrBytes - 1; n >= 0; n--)
			*src++ = bytes[n];
	} else
		return ESP_ERR_INVALID_ARG;

	return ESP_OK;
}

SCD30::SCD30(void) {
	// Constructor
}

esp_err_t SCD30::begin(i2c_master_bus_handle_t I2CbusHandle, bool autoCalibrate, bool measBegin) {
	esp_err_t err;
#ifdef TESTPOINTS 
		gpio_set_level(RS485DE_PIN, 1);
		ESP_LOGE(TAG, "TESTPOINTS ON!");
#endif 


	i2c_device_config_t dev_cfg = {
		.dev_addr_length = I2C_ADDR_BIT_LEN_7,
		.device_address = SCD30_ADDRESS,
		.scl_speed_hz = SCD30CLK,
	};
//	xSemaphoreTake(I2CSemaphore, portMAX_DELAY);

	err = i2c_master_bus_add_device(I2CbusHandle, &dev_cfg, &sps30DevHandle);
//	xSemaphoreGive(I2CSemaphore);
	if (err != ESP_OK) {
		ESP_LOGE(TAG, "Error adding SCD30 to I2Cbus");
		return err;
	}

	simulate = false;
	uint16_t fwVer;

	err = getFirmwareVersion(&fwVer); // Read the firmware version. Return false if the CRC check fails.

	if (err != ESP_OK) {
		simulate = true;
		simulateTmr = SIMULATEINTERVAL;
		ESP_LOGE(TAG, "Error reading firmware version");
		ESP_LOGE(TAG, "Simulating");
		return err;
	}
	ESP_LOGI(TAG, "SCD30 firmware version: %x", fwVer);

	if (measBegin == false) // Exit now if measBegin is false
		return ESP_OK;

	// Check for device to respond correctly
	if (beginMeasuring() == ESP_OK) // Start continuous measurements
	{
		err = setMeasurementInterval(2);			  // 2 seconds between measurements
		err |= setAutoSelfCalibration(autoCalibrate); // Enable auto-self-calibration
	}

	if (err != ESP_OK)
		ESP_LOGE(TAG, "Error intializing (%s)", esp_err_to_name(err));

	return (err);
}

////Calling this function with nothing sets the debug port to Serial
////You can also call it with other streams like Serial1, SerialUSB, etc.
// void SCD30::enableDebugging(Stream &debugPort)
//{
//   _debugPort = &debugPort;
//   _printDebug = true;
// }

// Returns the latest available CO2 level
// If the current level has already been reported, trigger a new read
uint16_t SCD30::getCO2(void) {
	if (co2HasBeenReported == true) // Trigger a new read
		readMeasurement();			// Pull in new co2, humidity, and temp into global vars

	co2HasBeenReported = true;

	return (uint16_t)co2; // Cut off decimal as co2 is 0 to 10,000
}

// Returns the latest available humidity
// If the current level has already been reported, trigger a new read
float SCD30::getHumidity(void) {
	if (humidityHasBeenReported == true) // Trigger a new read
		readMeasurement();				 // Pull in new co2, humidity, and temp into global vars

	humidityHasBeenReported = true;

	return humidity;
}

// Returns the latest available temperature
// If the current level has already been reported, trigger a new read
float SCD30::getTemperature(void) {
	if (temperatureHasBeenReported == true) // Trigger a new read
		readMeasurement();					// Pull in new co2, humidity, and temp into global vars

	temperatureHasBeenReported = true;

	return temperature;
}

// Enables or disables the ASC
esp_err_t SCD30::setAutoSelfCalibration(bool enable) {
	if (enable)
		return sendCommand(COMMAND_AUTOMATIC_SELF_CALIBRATION, 1); // Activate continuous ASC
	else
		return sendCommand(COMMAND_AUTOMATIC_SELF_CALIBRATION, 0); // Deactivate continuous ASC
}

// Set the forced recalibration factor. See 1.3.7.
// The reference CO2 concentration has to be within the range 400 ppm ≤ cref(CO2) ≤ 2000 ppm.
esp_err_t SCD30::setForcedRecalibrationFactor(uint16_t concentration) {
	if (concentration < 400 || concentration > 2000) {
		return ESP_ERR_INVALID_ARG; // Error check.
	}
	return sendCommand(COMMAND_SET_FORCED_RECALIBRATION_FACTOR, concentration);
}

// Get the temperature offset. See 1.3.8.
float SCD30::getTemperatureOffset(void) {
	uint16_t response;
	readRegister(COMMAND_SET_TEMPERATURE_OFFSET, &response);

	union {
		int16_t signed16;
		uint16_t unsigned16;
	} signedUnsigned; // Avoid any ambiguity casting int16_t to uint16_t
	signedUnsigned.signed16 = response;

	return (((float)signedUnsigned.signed16) / 100.0);
}

// Set the temperature offset to remove module heating from temp reading
esp_err_t SCD30::setTemperatureOffset(float tempOffset) {
	// Temp offset is only positive. See: https://github.com/sparkfun/SparkFun_SCD30_Arduino_Library/issues/27#issuecomment-971986826
	//"The SCD30 offset temperature is obtained by subtracting the reference temperature from the SCD30 output temperature"
	// https://www.sensirion.com/fileadmin/user_upload/customers/sensirion/Dokumente/9.5_CO2/Sensirion_CO2_Sensors_SCD30_Low_Power_Mode.pdf

	if (tempOffset < 0.0)
		return ESP_ERR_INVALID_ARG;

	uint16_t value = tempOffset * 100;
	return sendCommand(COMMAND_SET_TEMPERATURE_OFFSET, value);
}

// Get the altitude compenstation. See 1.3.9.
uint16_t SCD30::getAltitudeCompensation(void) {
	uint16_t response;
	readRegister(COMMAND_SET_ALTITUDE_COMPENSATION, &response);
	return response;
}

// Set the altitude compenstation. See 1.3.9.
esp_err_t SCD30::setAltitudeCompensation(uint16_t altitude) { return sendCommand(COMMAND_SET_ALTITUDE_COMPENSATION, altitude); }

// Set the pressure compenstation. This is passed during measurement startup.
// mbar can be 700 to 1200
esp_err_t SCD30::setAmbientPressure(uint16_t pressure_mbar) {
	if (pressure_mbar < 700 || pressure_mbar > 1200) {
		return ESP_ERR_INVALID_ARG;
	}
	return sendCommand(COMMAND_CONTINUOUS_MEASUREMENT, pressure_mbar);
}

// SCD30 soft reset
esp_err_t SCD30::reset() { return sendCommand(COMMAND_RESET); }

// Get the current ASC setting
bool SCD30::getAutoSelfCalibration() {
	uint16_t response;
	readRegister(COMMAND_AUTOMATIC_SELF_CALIBRATION, &response);
	if (response == 1) {
		return true;
	} else {
		return false;
	}
}

// Begins continuous measurements
// Continuous measurement status is saved in non-volatile memory. When the sensor
// is powered down while continuous measurement mode is active SCD30 will measure
// continuously after repowering without sending the measurement command.

esp_err_t SCD30::beginMeasuring(uint16_t pressureOffset) { return (sendCommand(COMMAND_CONTINUOUS_MEASUREMENT, pressureOffset)); }

// Overload - no pressureOffset
esp_err_t SCD30::beginMeasuring(void) { return (beginMeasuring(0)); }

// Stop continuous measurement
esp_err_t SCD30::StopMeasurement(void) { return (sendCommand(COMMAND_STOP_MEAS)); }

// Sets interval between measurements
// 2 seconds to 1800 seconds (30 minutes)
esp_err_t SCD30::setMeasurementInterval(uint16_t interval) { return sendCommand(COMMAND_SET_MEASUREMENT_INTERVAL, interval); }

// Gets interval between measurements
// 2 seconds to 1800 seconds (30 minutes)
uint16_t SCD30::getMeasurementInterval(void) {
	uint16_t interval = 0;
	getSettingValue(COMMAND_SET_MEASUREMENT_INTERVAL, &interval);
	return (interval);
}

// Returns true when data is available
bool SCD30::dataAvailable() {
	uint16_t response = 0;
	readRegister(COMMAND_GET_DATA_READY, &response);

	if (response == 1)
		return (true);
	return (false);
}

// Get 18 bytes from SCD30
// Updates global variables with floats
// Returns true if success
esp_err_t SCD30::readMeasurement() {
	// Verify we have data from the sensor
	esp_err_t err;

	if (simulate) {
		if (simulateTmr-- == 0) {
			simulateTmr = SIMULATEINTERVAL;
			// Now copy the uint32s into their associated floats
			co2 = 456;
			temperature = 23.4;
			humidity = 56.7;
			// Mark our global variables as fresh
			co2HasBeenReported = false;
			humidityHasBeenReported = false;
			temperatureHasBeenReported = false;
			return (ESP_OK); // Success! New data simulator values in globals.
		}
		return ESP_ERR_NOT_FINISHED;
	}

	if (dataAvailable() == false)
		return (ESP_ERR_NOT_FINISHED);

	uint8_t receivedBytes[18] = {0};
	ByteToFl tempCO2;
	tempCO2.value = 0;
	ByteToFl tempHumidity;
	tempHumidity.value = 0;
	ByteToFl tempTemperature;
	tempTemperature.value = 0;
	uint8_t write_buf[2] = {COMMAND_READ_MEASUREMENT >> 8, COMMAND_READ_MEASUREMENT & 0xF};
//	xSemaphoreTake(I2CSemaphore, portMAX_DELAY);
	// err = i2c_master_write_to_device(_i2cPort, SCD30_ADDRESS, write_buf, sizeof(write_buf), I2C_TIMEOUT_MS / portTICK_PERIOD_MS);
	err = i2c_master_transmit(sps30DevHandle, (uint8_t *)&write_buf, sizeof(write_buf), I2C_TIMEOUT_MS / portTICK_PERIOD_MS);
//	xSemaphoreGive(I2CSemaphore);

	if (err != ESP_OK)
		return err;

	vTaskDelay(3 / portTICK_PERIOD_MS);
//	xSemaphoreTake(I2CSemaphore, portMAX_DELAY);
	// err = i2c_master_read_from_device(_i2cPort, SCD30_ADDRESS, receivedBytes, sizeof(receivedBytes), I2C_TIMEOUT_MS / portTICK_PERIOD_MS);
	err = i2c_master_receive(sps30DevHandle, receivedBytes, sizeof(receivedBytes), I2C_TIMEOUT_MS / portTICK_PERIOD_MS);
//	xSemaphoreGive(I2CSemaphore);
	if (err != ESP_OK)
		return err;

	uint8_t bytesToCrc[2];
	for (uint8_t x = 0; x < sizeof(receivedBytes); x++) {
		uint8_t incoming = receivedBytes[x];

		switch (x) {
		case 0:
		case 1:
		case 3:
		case 4:
			tempCO2.array[x < 3 ? 3 - x : 4 - x] = incoming;
			bytesToCrc[x % 3] = incoming;
			break;
		case 6:
		case 7:
		case 9:
		case 10:
			tempTemperature.array[x < 9 ? 9 - x : 10 - x] = incoming;
			bytesToCrc[x % 3] = incoming;
			break;
		case 12:
		case 13:
		case 15:
		case 16:
			tempHumidity.array[x < 15 ? 15 - x : 16 - x] = incoming;
			bytesToCrc[x % 3] = incoming;
			break;
		default:
			// Validate CRC
			uint8_t foundCrc = computeCRC8(bytesToCrc, 2);
			if (foundCrc != incoming) {
				err = ESP_ERR_INVALID_CRC;
			}
			break;
		}
	}
	if (err != ESP_OK)
		return err;

	// Now copy the uint32s into their associated floats
	co2 = tempCO2.value;
	temperature = tempTemperature.value;
	humidity = tempHumidity.value;

	// Mark our global variables as fresh
	co2HasBeenReported = false;
	humidityHasBeenReported = false;
	temperatureHasBeenReported = false;

	return (ESP_OK); // Success! New data available in globals.
}

// Gets a setting by reading the appropriate register.
// Returns true if the CRC is valid.
esp_err_t SCD30::getSettingValue(uint16_t registerAddress, uint16_t *val) {

	int retries = 3;
	bool ok = false;
	uint8_t receivedBytes[3];
	esp_err_t err;

	if (simulate)
		return ESP_OK;

	swapBytes((uint8_t *)&registerAddress, sizeof(registerAddress));

	do {
		ESP_LOGE(TAG, "i2c_master_write_to_SCD30");
	//	xSemaphoreTake(I2CSemaphore, portMAX_DELAY);
		//	err = i2c_master_write_to_device(_i2cPort, SCD30_ADDRESS, (uint8_t *)&registerAddress, sizeof(registerAddress), I2C_TIMEOUT_MS /
		//portTICK_PERIOD_MS);

#ifdef TESTPOINTS 
		gpio_set_level(RS485TX_PIN, 1);
#endif 

		err = i2c_master_transmit(sps30DevHandle, (uint8_t *)&registerAddress, sizeof(registerAddress), I2C_TIMEOUT_MS / portTICK_PERIOD_MS);
	//	xSemaphoreGive(I2CSemaphore);

#ifdef TESTPOINTS 		
		gpio_set_level(RS485TX_PIN, 0);
#endif 


		if (err != ESP_OK) {
			ESP_LOGE(TAG, "i2c_master_write_to_SCD30 failed (%s)!", esp_err_to_name(err));
			vTaskDelay(3 / portTICK_PERIOD_MS);
		}
		vTaskDelay(3 / portTICK_PERIOD_MS);
		if (!err) {
#ifdef TESTPOINTS 
		gpio_set_level(RS485TX_PIN, 1);
#endif 

	//		xSemaphoreTake(I2CSemaphore, portMAX_DELAY);
			//	err =  i2c_master_read_from_device(_i2cPort, SCD30_ADDRESS, receivedBytes, sizeof(receivedBytes), I2C_TIMEOUT_MS /
			//portTICK_PERIOD_MS);
			err = i2c_master_receive(sps30DevHandle, (uint8_t *)&receivedBytes, sizeof(receivedBytes), I2C_TIMEOUT_MS / portTICK_PERIOD_MS);
	//		xSemaphoreGive(I2CSemaphore);

		#ifdef TESTPOINTS 
		gpio_set_level(RS485TX_PIN, 0);
#endif 	
			if (err != ESP_OK) {
				ESP_LOGE(TAG, "i2c_master_read_from_SCD30 failed (%s)!", esp_err_to_name(err));
				vTaskDelay(3 / portTICK_PERIOD_MS);
			} else
				ok = true;
		}
		if (retries-- == 0)
			return ESP_ERR_TIMEOUT;
	} while (!ok);

	uint8_t crc = receivedBytes[2];
	*val = (uint16_t)receivedBytes[0] << 8 | receivedBytes[1];

	uint8_t expectedCRC = computeCRC8(receivedBytes, 2);
	if (crc == expectedCRC) // Return true if CRC check is OK
		return (ESP_OK);
	else
		ESP_LOGE(TAG, "CRC failed!");

	return (ESP_ERR_INVALID_CRC);
}

// Gets two bytes from SCD30
esp_err_t SCD30::readRegister(uint16_t registerAddress, uint16_t *data) {
	uint16_t receivedBytes;
	esp_err_t err;

	swapBytes((uint8_t *)&registerAddress, sizeof(registerAddress));

//	xSemaphoreTake(I2CSemaphore, portMAX_DELAY);
	//	err = i2c_master_write_to_device(_i2cPort, SCD30_ADDRESS, (uint8_t *)&registerAddress, sizeof(registerAddress), I2C_TIMEOUT_MS /
	//portTICK_PERIOD_MS);
	err = i2c_master_transmit(sps30DevHandle, (uint8_t *)&registerAddress, sizeof(registerAddress), I2C_TIMEOUT_MS / portTICK_PERIOD_MS);
//	xSemaphoreGive(I2CSemaphore);

	if (err == ESP_OK) {
		vTaskDelay(3 / portTICK_PERIOD_MS);
#ifdef TESTPOINTS 
		gpio_set_level(RS485TX_PIN, 1);
#endif 

//		xSemaphoreTake(I2CSemaphore, portMAX_DELAY);
		//	err = i2c_master_read_from_device(_i2cPort, SCD30_ADDRESS, (uint8_t *) &receivedBytes, sizeof(receivedBytes), I2C_TIMEOUT_MS /
		//portTICK_PERIOD_MS);
		err = i2c_master_receive(sps30DevHandle, (uint8_t *)&receivedBytes, sizeof(receivedBytes), I2C_TIMEOUT_MS / portTICK_PERIOD_MS);
	//	xSemaphoreGive(I2CSemaphore);

	#ifdef TESTPOINTS 
		gpio_set_level(RS485TX_PIN, 0);
#endif 	

		if (err == ESP_OK) {
			swapBytes((uint8_t *)&receivedBytes, sizeof(receivedBytes));
			*data = receivedBytes;
		} else {
			ESP_LOGE(TAG, "Read register Read failed (%s)!", esp_err_to_name(err));
		}
	} else {
		// ESP_LOGE(TAG, "Read register Write failed (%s)!", esp_err_to_name(err));
	}
	return err;
}

// Sends a command along with arguments and CRC
esp_err_t SCD30::sendCommand(uint16_t command, uint16_t arguments) {
	esp_err_t err;
	if (simulate)
		return ESP_OK;

	uint8_t data[5];
	data[0] = command >> 8;
	data[1] = command & 0xFF;
	data[2] = arguments >> 8;
	data[3] = arguments & 0xFF;
	data[4] = computeCRC8(&data[2], 2); // Calc CRC on the arguments only, not the command
	// return i2c_master_write_to_device(_i2cPort, SCD30_ADDRESS, data, sizeof(data), I2C_TIMEOUT_MS / portTICK_PERIOD_MS);
#ifdef TESTPOINTS 
		gpio_set_level(RS485TX_PIN, 1);
#endif 
//	xSemaphoreTake(I2CSemaphore, portMAX_DELAY);
	err = i2c_master_transmit(sps30DevHandle, (uint8_t *)&data, sizeof(data), I2C_TIMEOUT_MS / portTICK_PERIOD_MS);
//	xSemaphoreGive(I2CSemaphore);

#ifdef TESTPOINTS 
		gpio_set_level(RS485TX_PIN, 0);
#endif 

	return err;
}

// Sends just a command, no arguments, no CRC
esp_err_t SCD30::sendCommand(uint16_t command) {
	esp_err_t err;
	if (simulate)
		return ESP_OK;

	swapBytes((uint8_t *)&command, sizeof(command));
#ifdef TESTPOINTS 
		gpio_set_level(RS485TX_PIN, 1);
#endif 
//	xSemaphoreTake(I2CSemaphore, portMAX_DELAY);
	//	return i2c_master_write_to_device(_i2cPort, SCD30_ADDRESS, (uint8_t *)&command, sizeof(command), I2C_TIMEOUT_MS / portTICK_PERIOD_MS);
	err = i2c_master_transmit(sps30DevHandle, (uint8_t *)&command, sizeof(command), I2C_TIMEOUT_MS / portTICK_PERIOD_MS);
//	xSemaphoreGive(I2CSemaphore);
#ifdef TESTPOINTS 
		gpio_set_level(RS485TX_PIN, 0);
#endif 

	return err;
}

// Given an array and a number of bytes, this calculate CRC8 for those bytes
// CRC is only calc'd on the data portion (two bytes) of the four bytes being sent
// From: http://www.sunshine2k.de/articles/coding/crc/understanding_crc.html
// Tested with: http://www.sunshine2k.de/coding/javascript/crc/crc_js.html
// x^8+x^5+x^4+1 = 0x31
uint8_t SCD30::computeCRC8(uint8_t data[], uint8_t len) {
	uint8_t crc = 0xFF; // Init with 0xFF

	for (uint8_t x = 0; x < len; x++) {
		crc ^= data[x]; // XOR-in the next input byte

		for (uint8_t i = 0; i < 8; i++) {
			if ((crc & 0x80) != 0)
				crc = (uint8_t)((crc << 1) ^ 0x31);
			else
				crc <<= 1;
		}
	}

	return crc; // No output reflection
}

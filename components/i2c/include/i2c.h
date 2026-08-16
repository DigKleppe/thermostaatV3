
#include <freertos/FreeRTOS.h>
#include <freertos/task.h>
#include <inttypes.h> // PRIx64
#include <stdbool.h>
#include <stdio.h> // printf
#include "driver/i2c_master.h"

#define SDA_PIN 0 // CANRx CANL 
#define SCL_PIN 6 // CANtx CANH	

#define I2C_MASTER_SCL_IO SCL_PIN	/*!< GPIO number used for I2C master clock */
#define I2C_MASTER_SDA_IO SDA_PIN	/*!< GPIO number used for I2C master data  */
#define I2C_MASTER_NUM I2C_NUM_0	/*!< I2C master i2c port number, the number of i2c peripheral interfaces available will depend on the chip */
#define I2C_MASTER_TX_BUF_DISABLE 0 /*!< I2C master doesn't need buffer */
#define I2C_MASTER_RX_BUF_DISABLE 0 /*!< I2C master doesn't need buffer */
#define I2C_MASTER_TIMEOUT_MS 1000

#ifdef __cplusplus
extern "C" {
#endif


/**
 * I2C device descriptor
 */
typedef struct {
	i2c_port_t port;		 //!< I2C port number
	i2c_device_config_t cfg; //!< I2C driver configuration
	i2c_master_dev_handle_t dev_handle;

	SemaphoreHandle_t mutex; //!< Device mutex
	uint32_t timeout_ticks;	 /*!< HW I2C bus timeout (stretch time), in ticks. 80MHz APB clock
								  ticks for ESP-IDF, CPU ticks for ESP8266.
								  When this value is 0, I2CDEV_MAX_STRETCH_TIME will be used */
} i2c_dev_t;

esp_err_t i2c_dev_read(const i2c_dev_t *dev, const void *out_data, size_t out_size, void *in_data, size_t in_size);
esp_err_t i2c_dev_write(const i2c_dev_t *dev, const void *out_reg, size_t out_reg_size, const void *out_data, size_t out_size);
esp_err_t i2c_master_bus_init();
i2c_master_bus_handle_t getI2CbusHandle(void ); 

#ifdef __cplusplus
}
#endif
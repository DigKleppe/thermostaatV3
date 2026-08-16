

#include <freertos/FreeRTOS.h>
#include <freertos/task.h>
#include <inttypes.h> // PRIx64
#include <stdbool.h>

#include "i2c.h"
#ifdef __cplusplus
extern "C" { 
#endif

i2c_master_bus_handle_t bus_handle;

esp_err_t i2c_dev_read(const i2c_dev_t *dev, const void *out_data, size_t out_size, void *in_data, size_t in_size) {
	esp_err_t res = ESP_OK;
	res = i2c_master_transmit_receive(dev->dev_handle, (const uint8_t *) out_data, out_size,(uint8_t *)  in_data, in_size, -1);
	return res;
}

esp_err_t i2c_dev_write(const i2c_dev_t *dev, const void *out_reg, size_t out_reg_size, const void *out_data, size_t out_size) {
	esp_err_t res = ESP_OK;

	if (out_reg && out_reg_size)
		res = i2c_master_transmit(dev->dev_handle,(const uint8_t *) out_reg, out_reg_size, -1);
	if (res == ESP_OK)
		res = i2c_master_transmit(dev->dev_handle,(const uint8_t *) out_data, out_size, -1);

	return res;
}


/**
 * @brief i2c master initialization
 */
esp_err_t i2c_master_bus_init(void) {

//	gpio_reset_pin((gpio_num_t) I2C_MASTER_SDA_IO);
//	gpio_reset_pin((gpio_num_t) I2C_MASTER_SCL_IO);
	
	i2c_master_bus_config_t bus_config = {
		.i2c_port = I2C_MASTER_NUM,
		.sda_io_num = (gpio_num_t) I2C_MASTER_SDA_IO,
		.scl_io_num = (gpio_num_t)  I2C_MASTER_SCL_IO,
		.clk_source = I2C_CLK_SRC_DEFAULT,
		.glitch_ignore_cnt = 7,
		//.flags.enable_internal_pullup = true,
	};
	return i2c_new_master_bus(&bus_config, &bus_handle);
}

i2c_master_bus_handle_t getI2CbusHandle() {
	return bus_handle;
}


#ifdef __cplusplus
} 
#endif
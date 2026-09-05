
/*
 * autoCalTask.cpp
 *
 *  Created on: june 28 2025
 *      Author: dig
 */
#define LOG_LOCAL_LEVEL ESP_LOG_ERROR
#include "esp_log.h"
#include "freertos/FreeRTOS.h"
#include "freertos/queue.h"
#include "freertos/semphr.h"
#include "freertos/task.h"
#include "time.h"

#include "log.h"
#include "sensirionTask.h"
#include "settings.h"
#include "udpServer.h"
#include "wifiConnect.h"
#include "clockTask.h"

static const char *TAG = "autoCalTask";

#define SENSORMSSG_TIMEOUT 1000 // wait 1 sec
#define UDPRECEIVEPORT 5051		// send by calibrated sensor
#define MAXLEN 128
#define CHECKINTERVAL 5 // minutes

TaskHandle_t udpServerTaskh;

char buffer[1024];

void autoCalTask(void *pvParameters) {
	udpMssg_t udpMssg;
	sensorMssg_t calMssg = {0};
	sensorMssg_t actualValues;
	int lastminute = -1;
	time_t now = 0;
	struct tm timeinfo;
	int minuteTmr = 2;
	float dev;
	bool save = false;
	int sensorNr;

	const udpTaskParams_t udpTaskParams = {.port = UDPRECEIVEPORT, .maxLen = MAXLEN};

	xTaskCreate(udpServerTask, "udpServerTask", 1024 * 2, (void *)&udpTaskParams, 5, &udpServerTaskh);

	while (!timeIsSet) {
		vTaskDelay(1000 / portTICK_PERIOD_MS);
	}
	ESP_LOGI(TAG, "running");
	
	while (1) {
		if (xQueueReceive(udpMssgBox, &udpMssg, portMAX_DELAY)) { // wait for messages from reference to arrive
			if (udpMssg.mssg) {
				ESP_LOGI(TAG, "%s", udpMssg.mssg);
				sscanf(udpMssg.mssg, "S%d,%f,%f,%f,%d", &sensorNr, &calMssg.co2, &calMssg.temperature, &calMssg.hum, &calMssg.rssi);
				if (sensorNr != 0) // check
					calMssg.co2 = 0;

				free(udpMssg.mssg);
			}
		}
		time(&now);
		localtime_r(&now, &timeinfo);
		if (lastminute == -1) {
			lastminute = timeinfo.tm_min;
		}
	

		if (lastminute != timeinfo.tm_min) {
			lastminute = timeinfo.tm_min;
			ESP_LOGI(TAG, "%d  %d", lastminute , minuteTmr);
			minuteTmr--;
			if (minuteTmr == 1)
				calMssg.co2 = 0; // only fresh message..
			if (minuteTmr <= 0) {
				ESP_LOGI(TAG, "calibrating");
				minuteTmr = CHECKINTERVAL;
				if (calMssg.co2) { // then my calibrator is in the air...
					getAvgMeasValues(&actualValues);

					dev = actualValues.co2 - calMssg.co2; // check CO2
					if ( dev < 0 )
						dev *= -1;
					if (dev > 10.0) {
						calValues.CO2 = calMssg.co2; // sensor will be updated in sensirionTask
						ESP_LOGI(TAG, "CO2 sensor calibrated %1.2f", dev);
						calvaluesReceived = true;
					} else
						ESP_LOGI(TAG, "CO2 sensor OK %1.2f", dev);
					calMssg.co2 = 0;

					dev = actualValues.temperature - userSettings.temperatureOffset - calMssg.temperature; // check temperature
					if ( dev < 0 )
						dev *= -1;
				
					ESP_LOGI(TAG, "temperature dev:  %1.2f", dev);

					if (dev > 0.1) {
						userSettings.temperatureOffset = actualValues.temperature - calMssg.temperature;
						ESP_LOGI(TAG, "temperature sensor calibrated %1.2f C", dev);
						save = true;
					} else
						ESP_LOGI(TAG, "temperature sensor OK %1.2f", dev);

					dev = actualValues.hum - userSettings.RHoffset - calMssg.hum; // checkRH
					if ( dev < 0 )
						dev *= -1;
					if (dev > 2) {
						userSettings.RHoffset = actualValues.hum - calMssg.hum;
						ESP_LOGI(TAG, "RH sensor calibrated %1.2f %%", dev);
						save = true;
					} else
						ESP_LOGI(TAG, "RH sensor OK %1.2f", dev);
					if (save) {
						save = false;
						saveSettings();
					}
				}
			}
		}
	}
}

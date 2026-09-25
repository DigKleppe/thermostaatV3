/*
 * Clock.cpp
 *
 *  Created on: Apr 2, 2022
 *      Author: dig
 */

#include "ClockDisplay.h"
#include "esp_log.h"
#include "esp_netif_sntp.h"
#include "esp_sntp.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "guiTask.h"
#include "lwip/ip_addr.h"

#include "KNMItask.h"
#include "wifiConnect.h"
#include <string.h>
#include <sys/time.h>
#include <time.h>

volatile bool clockSynced;
struct tm timeinfo;
static const char *TAG = "Clock";
#define CONFIG_SNTP_TIME_SERVER "pool.ntp.org"
volatile bool timeIsSet;
volatile uint32_t minuteCntr;


static void initialize_sntp(void) {
	ESP_LOGI(TAG, "Initializing and starting SNTP");
	esp_sntp_config_t config = ESP_NETIF_SNTP_DEFAULT_CONFIG(CONFIG_SNTP_TIME_SERVER);
	esp_netif_sntp_init(&config);
}

void clockTask(void *pvParameter) {
	int lastsec = -1;
	char strftime_buf[64];
	char outSiteTempBuf[20];
	ClockDisplay *pd;
	bool once = false;
	displayMssg_t displayMssg;
	int dummy;
	int minutePresc = 60;
	sntp_sync_status_t syncStatus;

	displayMssg.displayItem = DISPLAY_ITEM_CLOCK;
	displayMssg.str1 = strftime_buf;
	displayMssg.str2 = outSiteTempBuf;

	setenv("TZ", "CET-1CEST,M3.5.0,M10.5.0/3", 1);
	tzset();
	while (connectStatus != CONNECT_READY) {
		vTaskDelay(1000 / portTICK_PERIOD_MS);
	}

	initialize_sntp();

	time_t now = 0;
	int retry = 1;
	
	while (sntp_get_sync_status() == SNTP_SYNC_STATUS_RESET) { 
		ESP_LOGI(TAG, "Waiting for system time to be set... %d)", retry++);
		vTaskDelay(2000 / portTICK_PERIOD_MS);
	}
	timeIsSet = true;

	do {
		time(&now);
		localtime_r(&now, &timeinfo);
		if (lastsec != timeinfo.tm_sec) {
			lastsec = timeinfo.tm_sec;
			if (!once) {
				strftime(strftime_buf, sizeof(strftime_buf), "%c", &timeinfo);
				ESP_LOGI(TAG, "The current date/time is: %s", strftime_buf );
				once = true;
			}
			minutePresc--;
			if (minutePresc <= 0) {
				minutePresc = 60;
				minuteCntr++;
				esp_sntp_restart(); // time sometimes gets changed ?? 
			}

			sprintf(strftime_buf, "%2d:%02d:%02d", timeinfo.tm_hour, timeinfo.tm_min, timeinfo.tm_sec);
			if (buitenTemperatuur != ERROR_TEMPERATURE)
				sprintf(outSiteTempBuf,
						"%2.1f \xC2\xB0"
						"C",
						buitenTemperatuur);
			else
				outSiteTempBuf[0] = 0;

			if (displayMssgBox)
				xQueueSend(displayMssgBox, &displayMssg, DISPLAYPROCESTTIME);
		}
		vTaskDelay(200 / portTICK_PERIOD_MS);
	} while (1);
}
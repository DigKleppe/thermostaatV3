/*
 * Clock.cpp
 *
 *  Created on: Apr 2, 2022
 *      Author: dig
 */

#include "ClockDisplay.h"
#include "guiTask.h"
#include "esp_log.h"
#include "esp_netif_sntp.h"
#include "esp_sntp.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "lwip/ip_addr.h"

#include "KNMItask.h"
#include <string.h>
#include <sys/time.h>
#include <time.h>

volatile bool clockSynced;
struct tm timeinfo;
static const char *TAG = "Clock";
#define CONFIG_SNTP_TIME_SERVER "pool.ntp.org"
volatile bool timeIsSet;

// #define MAXDISPLAYS 5
// ClockDisplay *clockToUpdate[MAXDISPLAYS];
// int clockDisplays;

// void registerTimeUpdate(ClockDisplay *p) {
// 	if (clockDisplays < MAXDISPLAYS)
// 		clockToUpdate[clockDisplays++] = p;
// 	else
// 		ESP_LOGE(TAG, "Maximum number clockDisplays reached");
// }

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
	displayMssg.displayItem = DISPLAY_ITEM_CLOCK;
	displayMssg.str1 = strftime_buf;
	displayMssg.str2 = outSiteTempBuf;

	setenv("TZ", "CET-1CEST,M3.5.0,M10.5.0/3", 1);
	tzset();

	initialize_sntp();

	time_t now = 0;
	int retry = 0;

	const int retry_count = 20;
	while (sntp_get_sync_status() == SNTP_SYNC_STATUS_RESET) { //  && ++retry < retry_count) {
		ESP_LOGI(TAG, "Waiting for system time to be set... );// (%d/%d)", retry, retry_count);
		vTaskDelay(5000 / portTICK_PERIOD_MS);
	}
	
	timeIsSet = true;
	do {
		time(&now);
		localtime_r(&now, &timeinfo);
		if (lastsec != timeinfo.tm_sec) {
			lastsec = timeinfo.tm_sec;
			if (!once) {
				strftime(strftime_buf, sizeof(strftime_buf), "%c", &timeinfo);
				ESP_LOGI(TAG, "The current date/time is: %s", strftime_buf);
				once = true;
			}
			sprintf(strftime_buf, "%2d:%02d:%02d", timeinfo.tm_hour, timeinfo.tm_min, timeinfo.tm_sec);
			if ( buitenTemperatuur != ERROR_TEMPERATURE)
				sprintf(outSiteTempBuf, "%2.1f \xC2\xB0""C", buitenTemperatuur);
			else
				outSiteTempBuf[0]=0;


		if( displayMssgBox)
			xQueueSend(displayMssgBox, &displayMssg, DISPLAYPROCESTTIME);
		}
		vTaskDelay(200 / portTICK_PERIOD_MS);
	} while (1);
}
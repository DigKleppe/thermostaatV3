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
#include "wifiConnect.h"
#include <string.h>
#include <sys/time.h>
#include <time.h>

volatile bool clockSynced;
struct tm timeinfo;
static const char *TAG = "Clock";
volatile bool timeIsSet;

#define CONFIG_SNTP_TIME_SERVER "pool.ntp.org"
#ifndef INET6_ADDRSTRLEN
#define INET6_ADDRSTRLEN 48
#endif

void time_sync_notification_cb(struct timeval *tv)
{
    ESP_LOGI(TAG, "Notification of a time synchronization event");
}

static void print_servers(void)
{
    ESP_LOGI(TAG, "List of configured NTP servers:");

    for (uint8_t i = 0; i < SNTP_MAX_SERVERS; ++i){
        if (esp_sntp_getservername(i)){
            ESP_LOGI(TAG, "server %d: %s", i, esp_sntp_getservername(i));
        } else {
            // we have either IPv4 or IPv6 address, let's print it
            char buff[INET6_ADDRSTRLEN];
            ip_addr_t const *ip = esp_sntp_getserver(i);
            if (ipaddr_ntoa_r(ip, buff, INET6_ADDRSTRLEN) != NULL)
                ESP_LOGI(TAG, "server %d: %s", i, buff);
        }
    }
}



static void initialize_sntp(void) {
	ESP_LOGI(TAG, "Initializing and starting SNTP");
	esp_sntp_config_t config = ESP_NETIF_SNTP_DEFAULT_CONFIG(CONFIG_SNTP_TIME_SERVER);
	config.sync_cb = time_sync_notification_cb;     // Note: This is only needed if we want
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
	while( connectStatus != CONNECT_READY) {
		vTaskDelay( 1000/portTICK_PERIOD_MS);
	}
	

	initialize_sntp();

	time_t now = 0;
	int retry = 0;

	const int retry_count = 20;
	// while (sntp_get_sync_status() == SNTP_SYNC_STATUS_RESET) { //  && ++retry < retry_count) {
	// 	ESP_LOGI(TAG, "Waiting for system time to be set... );// (%d/%d)", retry, retry_count);
	// 	vTaskDelay(5000 / portTICK_PERIOD_MS);
	// }
	

    while (esp_netif_sntp_sync_wait(2000 / portTICK_PERIOD_MS) == ESP_ERR_TIMEOUT && ++retry < retry_count) {
        ESP_LOGI(TAG, "Waiting for system time to be set... (%d/%d)", retry, retry_count);
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
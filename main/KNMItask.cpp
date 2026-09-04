#define LOG_LOCAL_LEVEL ESP_LOG_ERROR

#include <esp_log.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "KNMItask.h"
#include "clockTask.h"
#include "httpsReadFile.h"
#include "passwords.pwd"
#include "wifiConnect.h"

#include <freertos/FreeRTOS.h>
#include <freertos/queue.h>
#include <freertos/task.h>
#if CONFIG_MBEDTLS_CERTIFICATE_BUNDLE
#include "esp_crt_bundle.h"
#endif

// extern const char server_root_cert_pem_start[] asm("_binary_ca_cert_pem_start");

extern volatile bool hpptActive;

// --- KNMI API gegevens ---
// #define API_KEY in passwords.pwd

#define LOCATION_ID "0-20000-0-06323" // Wilhelminadorp
#define COLLECTION "10-minute-in-situ-meteorological-observations"
#define BASE_URL "https://api.dataplatform.knmi.nl/edr/v1/collections"

static const char *TAG = "KNMItask";

#define READBUFFERSIZE 1200
float buitenTemperatuur = ERROR_TEMPERATURE;

// #define SAMPLEPERIOD 10
// #define FIRSTSAMPLEOFFSET (60 + SAMPLEPERIOD) // wintertijd GMT

#define SAMPLEPERIOD 15
#define FIRSTSAMPLEOFFSET (60 + SAMPLEPERIOD) // wintertijd GMT

static float get_temperature(void) {
	httpsRegParams_t httpsRegParams;
	httpsMssg_t mssg;
	float temp = -999.0;
	float value1, value2;
	struct tm *t;
	char dateTime[64];
	//  = "2026-08-10T12:00:00Z/2026-08-10T13:00:00Z";
	char *readBuffer = (char *)heap_caps_malloc(READBUFFERSIZE + 1, MALLOC_CAP_SPIRAM); // malloc(READBUFFERSIZE);
	if (readBuffer == NULL) {
		ESP_LOGE(TAG, "No memory for readBuffer");
		return -999;
	}
	// prepare request period for last sample (latest does not work)
	time_t now = time(NULL); // Get current time
	time_t firstValue = now - FIRSTSAMPLEOFFSET * 60;
	t = localtime(&now);
	if (t->tm_isdst > 0) { // zomertijd
		firstValue -= 60 * 60;
	}

	time_t secondValue = firstValue + SAMPLEPERIOD * 60;
	t = localtime(&firstValue); // Convert to local time structure
	strftime(dateTime, sizeof(dateTime), "%Y-%m-%dT%H:%M:%SZ/", t);
	t = localtime(&secondValue);
	strftime(dateTime + strlen(dateTime), sizeof(dateTime), "%Y-%m-%dT%H:%M:%SZ", t);
	ESP_LOGI(TAG, "datetime: %s", dateTime);

	char url[256];
	snprintf(url, sizeof(url), "%s/%s/locations/%s?datetime=%s&parameter-name=ta", BASE_URL, COLLECTION, LOCATION_ID, dateTime);

	// "https://api.dataplatform.knmi.nl/edr/v1/collections/10-minute-in-situ-meteorological-observations/locations/0-20000-0-06323?datetime=2026-08-10T12%3A00%3A00Z%2F2026-08-10T13%3A00%3A00Z&parameter-name=ta");

	esp_http_client_config_t config;
	memset((uint8_t *)&config, 0, sizeof(config));
	config.url = url;
	config.method = HTTP_METHOD_GET;
	config.timeout_ms = 10000;
	config.cert_pem = NULL;						// NULL betekent: gebruik de ingebouwde certificatenbundel
	config.skip_cert_common_name_check = false; // CN validatie inschakelen
	config.keep_alive_enable = false;

	// config.cert_pem = server_root_cert_pem_start;

	ESP_LOGI(TAG, "Request URL: %s", url);

	esp_http_client_handle_t client = esp_http_client_init(&config);
	if (client == NULL) {
		ESP_LOGE(TAG, " HTTP client not initialized");
		free(readBuffer);
		return -999;
	}
	// API-sleutel toevoegen in de Authorization header
	char auth_header[128];
	snprintf(auth_header, sizeof(auth_header), "%s", API_KEY);
	//  esp_http_client_set_header(client, "Authorization", auth_header);
	esp_http_client_set_header(client, "Authorization", API_KEY);

	// Optioneel: extra headers voor HTTPS
	esp_http_client_set_header(client, "User-Agent", "ESP32-KNMI/1.0");
	esp_http_client_set_header(client, "Accept", "application/json");
	httpsRegParams.httpClientHandle = client;
	httpsRegParams.destbuffer = (uint8_t *)readBuffer;
	httpsRegParams.maxChars = READBUFFERSIZE;

	xTaskCreate(httpsGetRequestTask, "httpsReqTask", 4 * 1024, (void *)&httpsRegParams, 0, NULL);

	do {
		xQueueSend(httpsReqRdyMssgBox, &mssg, 0);
		mssg.len = 0;
		if (xQueueReceive(httpsReqMssgBox, (void *)&mssg, (5000 / portTICK_PERIOD_MS))) {
			if (mssg.len) {
				readBuffer[mssg.len] = 0;
				ESP_LOGI(TAG, "%d %s", mssg.len, readBuffer);

				char *ta = strstr(readBuffer, "\"ta\":");
				if (ta) {
					char *pValue = strstr(ta, "\"values\":");
					if (pValue) {
						if (sscanf(pValue, "\"values\":[%f,%f]", &value1, &value2) == 2) {
							temp = value2;
							//	printf("2 values %f", temp);
						} else {
							if (sscanf(pValue, "\"values\":[%f]", &value1) == 1) {
								temp = value1;
								//	printf("1 value %f", temp);
							}
						}
					} else
						ESP_LOGE(TAG, "values not found");
				} else
					ESP_LOGE(TAG, "ta not found");

				vTaskDelay(100);
				xQueueSend(httpsReqRdyMssgBox, &mssg, 0);
			}
		} else
			ESP_LOGE(TAG, "Timeout HTTP request");
	} while (mssg.len > 0);

	free(readBuffer);
	return temp;
}

void KNMItask(void *parameters) {

	while (!timeIsSet)
		vTaskDelay(pdMS_TO_TICKS(1000));



	// if (httpsReqMssgBox == NULL) { // once
	// 	httpsReqMssgBox = xQueueCreate(1, sizeof(httpsMssg_t));
	// 	httpsReqRdyMssgBox = xQueueCreate(1, sizeof(httpsMssg_t));
	// } else {
	// 	xQueueReset(httpsReqMssgBox);
	// 	xQueueReset(httpsReqRdyMssgBox);
	// }
	while (1) {
		//	if (xSemaphoreTake(hpptReqSemphore, portMAX_DELAY) == pdTRUE) { // shared with updateTask
		do {
			ESP_LOGI(TAG, "wait semaphore ");
			vTaskDelay(100 / portMAX_DELAY);
		} while (hpptActive);

		hpptActive = true; // sorry
		ESP_LOGI(TAG, "semaphore taken");
		buitenTemperatuur = get_temperature();
		//	xSemaphoreGive(hpptReqSemphore);

		hpptActive = false; // sorry
		if (buitenTemperatuur != -999.0) {
			ESP_LOGI(TAG, "🌡️ Temperatuur in Wilhelminadorp: %.1f °C", buitenTemperatuur);
		} else {
			ESP_LOGE(TAG, "❌ Kon temperatuur niet ophalen");
		}
		// Wacht 5 minuten voor volgende meting
		//	vTaskDelay(pdMS_TO_TICKS(5 * 60 * 60 * 1000));
		vTaskDelay(pdMS_TO_TICKS(30 * 60 * 1000));
	}
}

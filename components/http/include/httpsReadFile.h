/*
 * httpsRequest.h
 *
 *  Created on: Jan 7, 2024
 *      Author: dig
 */

#ifndef COMPONENTS_HTTP_INCLUDE_HTTPSREQUEST_H_
#define COMPONENTS_HTTP_INCLUDE_HTTPSREQUEST_H_

#include "freertos/FreeRTOS.h"
#include "freertos/queue.h"
#include <esp_http_client.h>

#define SERVER_URL_MAX_SZ 256
#define HTTPSBUFSIZE 1024
#define MSSGBOX_TIMEOUT (5000/portTICK_PERIOD_MS)


typedef struct {
	esp_http_client_handle_t httpClientHandle;
//	char *httpsServer;
//	char *httpsURL;
	uint8_t *destbuffer; // where to put data read
	int maxChars;
} httpsRegParams_t;

// typedef struct {
// 	char *httpsServer;
// 	char *httpsURL;
// 	uint8_t *destbuffer; // where to put data read
// 	int maxChars;
// } httpsRegParams_t;




typedef struct {
	int len;
} httpsMssg_t ;

extern QueueHandle_t httpsReqMssgBox;
extern QueueHandle_t httpsReqRdyMssgBox;
void httpsGetRequestTask(void *pvparameters);

int httpsReadFile(const httpsRegParams_t *httpsRegParams);
int httpsReadFile(char * url, char * dest, int maxChars);


#endif /* COMPONENTS_HTTP_INCLUDE_HTTPSREQUEST_H_ */

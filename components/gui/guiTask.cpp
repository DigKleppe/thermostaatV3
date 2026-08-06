/*
 * guiTask.c
 *
 *  Created on: Mar 2, 2021
 *      Author: dig
 *
 *      handles screens
 */

#include "MeasScreen.h"
#include "InfoScreen.h"
#include "MainScreen.h"
#include "guiTask.h"

// extern "C" {      if ( timer++ == 200) {

#include "freertos/FreeRTOS.h"
#include "freertos/semphr.h"
#include "freertos/task.h"
#include "styles.h"

#include "settings.h"
#include "wifiConnect.h"

// extern MenuSetttingsDesrc_t DMMSettingsDescrTable[];
extern "C" void disp_wait_for_pending_transactions(void);

userState_t userState = USER_STATE_RUN;

QueueHandle_t displayMssgBox;
QueueHandle_t displayReadyMssgBox;
SemaphoreHandle_t xGuiSemaphore;

MainScreen *mainScreen;
MeasScreen *measScreen;
InfoScreen *infoScreen;

int screenIdx;
extern float PIDsetting;
extern char myIpAddress[];
extern uint32_t upTime;

#define NRSCREENS 3

const infoDescr_t infoDesc[] = {{"Netwerk:", "%s", wifiSettings.SSID}, {"IPadres:", "%s", myIpAddress}, {"PID:", "%2.2f", &PIDsetting}, {"Optijd:", "%d", &upTime}, {NULL, NULL, NULL}};

void showScreen(int idx) {
	switch (idx) {
	case 0:
		measScreen->show();
		break;
	case 1:
		mainScreen->show();
		break;
	case 2:
		infoScreen->show();
		break;

	default:
		break;
	}
}

void nextScreenClick(lv_event_t *e) { // from navigArrows
	if (screenIdx < (NRSCREENS - 1))
		screenIdx++;
	else
		screenIdx = 0;
	showScreen(screenIdx);
}

void prevScreenClick(lv_event_t *e) { // from navigArrows
	if (screenIdx > 0)
		screenIdx--;
	else
		screenIdx = NRSCREENS - 1;
	showScreen(screenIdx);
}

void guiTask(void *pvParameter) {
	displayMssg_t recDdisplayMssg;
	int dummy;
	int step = 0;

	displayMssgBox = xQueueCreate(5, sizeof(displayMssg_t));
	displayReadyMssgBox = xQueueCreate(1, sizeof(uint32_t));
	xGuiSemaphore = xSemaphoreCreateMutex();
	initStyles();

	mainScreen = new MainScreen();
	measScreen = new MeasScreen();
	infoScreen = new InfoScreen(infoDesc);
	vTaskDelay(50 / portTICK_PERIOD_MS);
	showScreen(0);

	while (1) {
		if (xQueueReceive(displayMssgBox, &recDdisplayMssg, 0) == pdTRUE) {
			if (pdTRUE == xSemaphoreTake(xGuiSemaphore, portMAX_DELAY)) {
				switch (recDdisplayMssg.displayItem) {
				case DISPLAY_ITEM_STATUSLINE:
					measScreen->setStatusLine((const char *)recDdisplayMssg.str1);
					break;

				case DISPLAY_ITEM_MEASLINE:
					measScreen->setDisplayText(recDdisplayMssg.line, (char *)recDdisplayMssg.str1);
					break;
				case DISPLAY_ITEM_STOP:
				case DISPLAY_ITEM_COLOR:
					break;

				case DISPLAY_ITEM_MESSAGE:
					//	messageScreen.show ((const char *) recDdisplayMssg.str1 , LV_COLOR_BLACK, recDdisplayMssg.showTime);
					break;
				}
				xSemaphoreGive(xGuiSemaphore);
			}
			xQueueSend(displayReadyMssgBox, &dummy, 0);
		}
		else
			vTaskDelay(10 / portTICK_PERIOD_MS);

	}
}

// /home/dig/.espressif/tools/openocd-esp32/v0.10.0-esp32-20200420/openocd-esp32/bin/openocd -f interface/ftdi/c232hm.cfg -f board/esp-wroom-32.cfg -c "program_esp
// /home/dig/projecten/littleVGL/dmmGui/build/dmm. 0x10000 verify"

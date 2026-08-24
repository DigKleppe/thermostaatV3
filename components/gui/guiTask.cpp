/*
 * guiTask.c
 *
 *  Created on: Mar 2, 2021
 *      Author: dig
 *
 *      handles screens
 */

#include "guiTask.h"
#include "StartScreen.h"
#include "InfoScreen.h"
#include "MainScreen.h"
#include "MeasScreen.h"

// extern "C" {      if ( timer++ == 200) {

#include "freertos/FreeRTOS.h"
#include "freertos/semphr.h"
#include "freertos/task.h"
#include "freertos/timers.h"

#include "styles.h"

#include "settings.h"
#include "wifiConnect.h"
#include "softwareVersions.h"

// extern MenuSetttingsDesrc_t DMMSettingsDescrTable[];

userState_t userState = USER_STATE_RUN;

QueueHandle_t displayMssgBox;
QueueHandle_t displayReadyMssgBox;
// SemaphoreHandle_t xGuiSemaphore;

MainScreen *mainScreen;
MeasScreen *measScreen;
InfoScreen *infoScreen;
StartScreen * startScreen;


int screenIdx;
extern float PIDsetting;
extern char myIpAddress[];
extern uint32_t upTimeHrs;
extern int rssi;

#define NRSCREENS 4

#define SCREENTIME (15 * 1000 / portTICK_PERIOD_MS)

TimerHandle_t screenTimer;



const infoDescr_t infoDesc[] = {{"Netwerk:", "%s", wifiSettings.SSID},
								{"IPadres:", "%s", myIpAddress},
								{"Softwareversie:","%s" , FIRMWARE_VERSION},
								{"SPIFFSversie:","%s",SPIFFS_VERSION},
								{"Temp. offset:", "%1.1f", &userSettings.temperatureOffset},
								{"RH offset:", "%1.1f", &userSettings.RHoffset},
								{"PID:", "%2.2f", &PIDsetting},
								{"Signaal:", "%d", &rssi},
								{"Optijd:", "%d", &upTimeHrs},
								{NULL, NULL, NULL}};

void showScreen(int idx) {
	xTimerStart(screenTimer,0);
	switch (idx) {
	case 0:
		startScreen->show();
	
		break;

	case 1:
		xTimerChangePeriod(screenTimer,SCREENTIME, 100);
		measScreen->show();
		measScreen->setSetpointValue(); 
		break;
	case 2:
		mainScreen->show();
		break;
	case 3:
		infoScreen->show();
		break;

	default:
		break;
	}
}

void screenTimerCallback(TimerHandle_t xTimer) {
	screenIdx = 1;
	showScreen(screenIdx);
}

void nextScreenClick(lv_event_t *e) { // from navigArrows
	
	if (screenIdx < (NRSCREENS - 1))
		screenIdx++;
	else
		screenIdx = 1;
	showScreen(screenIdx);
}

void prevScreenClick(lv_event_t *e) { // from navigArrows
	
	if (screenIdx > 1)
		screenIdx--;
	else
		screenIdx = NRSCREENS - 1;
	showScreen(screenIdx);
}

void guiTask(void *pvParameter) {
	displayMssg_t recDdisplayMssg;
	int dummy;
	displayMssgBox = xQueueCreate(5, sizeof(displayMssg_t));
	TickType_t xLastWakeTime;

	screenTimer = xTimerCreate("screenTimer", (5 * 1000 / portTICK_PERIOD_MS), false, (void *)0, screenTimerCallback);

	initStyles();
	mainScreen = new MainScreen();
	measScreen = new MeasScreen();
	infoScreen = new InfoScreen(infoDesc);
	startScreen = new StartScreen();

	vTaskDelay(50 / portTICK_PERIOD_MS);
	showScreen(0);

	while (1) {
		if (xQueueReceive(displayMssgBox, &recDdisplayMssg, portMAX_DELAY) == pdTRUE) {
			xLastWakeTime = xTaskGetTickCount();
			switch (recDdisplayMssg.displayItem) {
			case DISPLAY_ITEM_STATUSLINE:
				measScreen->setStatusLine((const char *)recDdisplayMssg.str1);
				break;

			case DISPLAY_ITEM_MEASLINE:
				measScreen->setDisplayText(recDdisplayMssg.line, (char *)recDdisplayMssg.str1);
				break;

			case DISPLAY_ITEM_CLOCK:
				measScreen->setClockDisplayText((char *)recDdisplayMssg.str1);
				measScreen->setClockDisplayOutsideTemp((char *)recDdisplayMssg.str2);
				break;

			case DISPLAY_ITEM_STOP:
			case DISPLAY_ITEM_COLOR:
				break;

			case DISPLAY_ITEM_MESSAGE:
				//	messageScreen.show ((const char *) recDdisplayMssg.str1 , LV_COLOR_BLACK, recDdisplayMssg.showTime);
				break;

			default:
				break;
			}
		}
		xTaskDelayUntil(&xLastWakeTime, DISPLAYPROCESTTIME - (110 / portTICK_PERIOD_MS));
	}
}

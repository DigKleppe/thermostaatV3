/*
 * PID.cpp
 *
 *  Created on: Apr 18, 2022
 *      Author: dig
 */

// #define LOG_LOCAL_LEVEL ESP_LOG_ERROR

#include "PID.h"
#include "esp_log.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "sensirionTask.h"
#include "settings.h"

#define TAG "PID"
#define PWMINTERVAL 100 // ms

#define HEATINGONLEVEL 1
#define MAXdTime 100		// window for PID d
float diffBuffer[MAXdTime]; // for PI d
int diffbufIDX;
int nrValuesInDiffbuf;

float PIDsetting; // for cgi
float PIDdiff;

thermostatStatus_t thermostatStatus;

int PWMpercent;

void heatingOn() {
	if (userSettings.heatingOn) {
		gpio_set_level(RS485DE_PIN, 1);
		gpio_set_level(RS485TX_PIN, HEATINGONLEVEL);

		//	ESP_LOGI(TAG, "Heating ON");

		thermostatStatus = HEATING_ON;
	} else
		thermostatStatus = THERMOSTATOFF;
}

void coolingOn() {
	if (userSettings.coolingOn) {
		gpio_set_level(RS485DE_PIN, 1);
		gpio_set_level(RS485TX_PIN, !HEATINGONLEVEL);
		thermostatStatus = COOLING_ON;
		//	ESP_LOGI(TAG, "Cooling  ON");
	} else
		thermostatStatus = THERMOSTATOFF;
}

void bothOff(void) {
	gpio_set_level(RS485DE_PIN, 0);
	thermostatStatus = THERMOSTATOFF;
	//	ESP_LOGI(TAG, "off");
}

void PWMTask(void *p) {
	int onTimer;
	int offTimer;
	int lastPWMpercent = 999;
	int state = 0;
	bool heatingActive = true;

	while (1) {
		vTaskDelay(PWMINTERVAL);

		if (PWMpercent < 0) { // cooling?
			if (userSettings.coolingOn) {
				heatingActive = false;
				PWMpercent = -1 * PWMpercent;
			} else {			// switched off
				PWMpercent = 0; // off
				state = 0;
			}
		} else {
			if (!userSettings.heatingOn) {
				PWMpercent = 0; // off
				state = 0;
			}
		}

		switch (state) {
		case 0: // inactove
			bothOff();
			offTimer = 0;
			onTimer = 0;
			if (PWMpercent > 1) {
				onTimer = PWMpercent;
				state++;
				if (heatingActive)
					heatingOn();
				else
					coolingOn();
			}
			break;

		case 1:
			if (heatingActive)
				heatingOn();
			else
				coolingOn();

			if (PWMpercent < 1)
				state = 0;
			else {
				onTimer--;
				if (onTimer <= 0) {
					if (PWMpercent < 99) {
						offTimer = (100 - PWMpercent);
						state++;
					} else
						onTimer = 1; // keep in this state at 100%
				}
			}
			break;

		case 2: // off timer active
			bothOff();
			if (PWMpercent > lastPWMpercent) { // react quick on higher demand
				int newOffTimer = (100 - PWMpercent);
				if (newOffTimer < offTimer)
					offTimer = newOffTimer;
			}
			lastPWMpercent = PWMpercent;

			offTimer--;
			if (offTimer <= 0) {
				state = 1;
				lastPWMpercent = PWMpercent;
				onTimer = PWMpercent;
				break;
			}
		}
		// ESP_LOGI(TAG, "OnTmr: %d  OffTmr:%d State:%d", onTimer, offTimer, state);
	}
}

void updatePID(float temperature) {

	float delta;
	float result;
	float diffTemperature; // for diff
	static float iSum = 0;
	static int dTime = userSettings.PIDdTime;

	delta = userSettings.temperatureSetpoint - temperature;
	result = delta * userSettings.PIDp;
	iSum += delta * userSettings.PIDi;
	
	if (userSettings.heatingOn) {
		if (delta <= 0) // temperature above setpoint heating
			iSum = 0;	 // zero i to avoid overshoot
		else {
			if (iSum < userSettings.PWMstartVal)
				iSum = userSettings.PWMstartVal;
		}
	}
	if (iSum > 0) {
		if (iSum > userSettings.PIDmaxi) // limit to maxI
			iSum = userSettings.PIDmaxi;
	} else {
		if (iSum < -1 * userSettings.PIDmaxi) // or negative value
			iSum = -1 * userSettings.PIDmaxi;
	}
	if (!userSettings.heatingOn) {
		if (iSum > 0) // limit to zero if heating is off
			iSum = 0;
	}
	if (!userSettings.coolingOn) {
		if (iSum < 0) // limit to zero if cooling is off
			iSum = 0;
	}
	//	printf("\ndelta: %f P:%f I:%f ", delta, result, iSum);

	result += iSum;

	if (userSettings.PIDdTime > dTime)
		nrValuesInDiffbuf = 0;
	if (userSettings.PIDdTime <= 0)
		userSettings.PIDdTime = 30;

	if (dTime != userSettings.PIDdTime) {
		dTime = userSettings.PIDdTime;

		if (dTime > MAXdTime) {
			dTime = MAXdTime;
			userSettings.PIDdTime = MAXdTime;
		}
	}
	diffBuffer[diffbufIDX++] = temperature;
	if (diffbufIDX >= dTime)
		diffbufIDX = 0;

	if (nrValuesInDiffbuf < dTime)
		nrValuesInDiffbuf++;
	else {
		diffTemperature = - temperature - diffBuffer[diffbufIDX] ;
		PIDdiff = diffTemperature * userSettings.PIDd;
		ESP_LOGI(TAG, "D: %1.1f \n", PIDdiff);
		if (userSettings.heatingOn)
			result -= PIDdiff;
		else
			result += PIDdiff;
	}

	if (result > 100)
		result = 100;

	if (result < -100)
		result = -100;

	ESP_LOGI(TAG, "PWM: %1.1f \n", result);
	PIDsetting = result;
	PWMpercent = result;
	// PWMpercent =  ( int) temperature;   //result;
}

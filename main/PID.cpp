/*
 * PID.cpp
 *
 *  Created on: Apr 18, 2022
 *      Author: dig
 */

#define LOG_LOCAL_LEVEL ESP_LOG_ERROR

#include "settings.h"
#include "sensirionTask.h"
#include "PID.h"
#include "esp_log.h"

#define TAG "PID"

#define MINIMUM_ON_TIME   120 // seconds
#define HEATINGONLEVEL	  0

float PIDsetting; // for cgi
thermostatStatus_t thermostatStatus;

void heatingOn(){
	if ( userSettings.heatingOn) {
		gpio_set_level(RS485DE_PIN, 1);
		gpio_set_level(RS485TX_PIN, HEATINGONLEVEL);
		
		ESP_LOGI( TAG, "Heating ON");

		thermostatStatus = HEATING_ON;
	}
	else
		thermostatStatus = THERMOSTATOFF;
}

void coolingOn(){
	if ( userSettings.coolingOn) {
		gpio_set_level(RS485DE_PIN, 1);
		gpio_set_level(RS485TX_PIN, !HEATINGONLEVEL);
		thermostatStatus = COOLING_ON;
		ESP_LOGI( TAG,"Cooling  ON");
	}
	else
		thermostatStatus = THERMOSTATOFF;
}

void bothOff (void) {
	gpio_set_level(RS485DE_PIN, 0);
	thermostatStatus = THERMOSTATOFF;
		ESP_LOGI( TAG,"off");
}


// called from sensirionTask every MEASINTERVAL seconds


void setPWM(int perc) {
	static int onTimer;
	static int offTimer;
	static int lastPerc = 999;
	static int state = 0;
	bool heatingActive = true;

	if (perc < 0) {
		heatingActive = false;
		perc = -1 * perc;
	}

	switch (state) {
	case 0:  // inactove
		bothOff();
		offTimer = 0;
		onTimer = 0;
		if (perc > 1) {
			onTimer = MINIMUM_ON_TIME / MEASINTERVAL;
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

		if (perc < 1)
			state = 0;
		else {
			onTimer--;
			if (onTimer <= 0) {
				if (perc < 99) {
					offTimer = (100 - perc) / MEASINTERVAL;
					state++;
				} else
					onTimer = 1; // keep in this state at 100%
			}
		}
		break;

	case 2:  // off timer active
		bothOff();
		if (perc > lastPerc) {  // react quick on higher demand
			int newOffTimer = (100 - perc) / MEASINTERVAL;
			if (newOffTimer < offTimer)
				offTimer = newOffTimer;
		}
		lastPerc = perc;

		offTimer--;
		if (offTimer == 0) {
			state = 1;
			onTimer = MINIMUM_ON_TIME / MEASINTERVAL;
			break;
		}
	}
	ESP_LOGI( TAG, "OnTmr: %d  OffTmr:%d State:%d",  onTimer, offTimer, state);
}


void updatePID(float temperature) {

	float delta;
	float result;
	static float iSum = 0;

	delta = userSettings.temperatureSetpoint - temperature;
	result = delta * userSettings.PIDp;
	iSum += delta * userSettings.PIDi;
	if ( userSettings.heatingOn && (delta < 0)) // temperature above setpoint heating
		iSum = 0; // zero i to avoid overshoot

	// if ( userSettings.coolingOn && (delta > 0)) // temperature below setpoint cooling
	// 	iSum = 0; // zero i to avoid overshoot

	if (iSum > 0) {
		if (iSum > userSettings.PIDmaxi)  // limit to maxI
			iSum = userSettings.PIDmaxi;
	} else {
		if (iSum < -1 * userSettings.PIDmaxi) // or negative value
			iSum = -1 * userSettings.PIDmaxi;
	}
	if ( !userSettings.heatingOn )
	{
		if ( iSum > 0 ) // limit to zero if heating is off
			iSum = 0;
	}
	if ( !userSettings.coolingOn )
	{
		if ( iSum < 0 ) // limit to zero if cooling is off
			iSum = 0;
	}
//	printf("\ndelta: %f P:%f I:%f ", delta, result, iSum);

	result += iSum;

	if (result > 100)
		result = 100;

	if (result < -100)
		result = -100;

	ESP_LOGI( TAG, "PWM: %1.1f \n",  result);
	PIDsetting = result;
	setPWM(result);
}




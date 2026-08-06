/*
 * PID.cpp
 *
 *  Created on: Apr 18, 2022
 *      Author: dig
 */



#include "settings.h"
#include "sensirionTask.h"
#include "PID.h"


#define MINIMUM_ON_TIME 240 // seconds

float PIDsetting; // for cgi
thermostatStatus_t thermostatStatus;


void heatingOff(){
	printf( " *** OFF *** ");
	gpio_set_level(HEATING_PIN, 1);
	thermostatStatus = THERMOSTATOFF;

}
void heatingOn(){
	if ( userSettings.heatingOn) {
		printf( " *** ON *** ");
		gpio_set_level(HEATING_PIN, 0);
		thermostatStatus = HEATING_ON;
	}
	else
		thermostatStatus = THERMOSTATOFF;
}

void coolingOn(){
	if ( userSettings.coolingOn) {
		gpio_set_level(COOLING_PIN, 0);
		thermostatStatus = COOLING_ON;
	}
	else
		thermostatStatus = THERMOSTATOFF;

}

void coolingOff(){
	gpio_set_level(COOLING_PIN, 1);
	thermostatStatus = THERMOSTATOFF;
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
		heatingOff();
		coolingOff();
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
			if (onTimer == 0) {
				if (perc < 99) {
					offTimer = (100 - perc) / MEASINTERVAL;
					state++;
				} else
					onTimer = 1; // keep in this state at 100%
			}
		}
		break;

	case 2:  // off timer active
		heatingOff();
		coolingOff();
		if (perc > lastPerc) {  // react quick on higher demand
			int newOffTimer = (100 - perc) / MEASINTERVAL;
			if (newOffTimer < offTimer)
				offTimer = newOffTimer;
		}
		lastPerc = perc;

		offTimer--;
		if (offTimer == 0) {
			state = 1;
			break;
		}
	}
}


void updatePID(float temperature) {

	float delta;
	float result;
	static float iSum = 0;

	delta = userSettings.temperatureSetpoint - temperature;
	result = delta * userSettings.PIDp;
	iSum += delta * userSettings.PIDi;
	if ( userSettings.heatingOn && (delta < 0)) // temperature above setpoint
		iSum = 0; // zero i to avoid overshoot

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
	printf("\ndelta: %f P:%f I:%f ", delta, result, iSum);

	result += iSum;

	if (result > 100)
		result = 100;

	if (result < -100)
		result = -100;

	printf( " %%: %1.1f \n",  result);
	PIDsetting = result;
	setPWM(result);
}




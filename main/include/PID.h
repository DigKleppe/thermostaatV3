/*
 * PID.h
 *
 *  Created on: Apr 18, 2022
 *      Author: dig
 */

#ifndef MAIN_INCLUDE_PID_H_
#define MAIN_INCLUDE_PID_H_

#include "driver/gpio.h"

// uses RS485 outputsconnected tp optocouplers for heating and cooling valve
// T6 removed , pin 1 U6 connected to pin2/3 U7 
// RS485A   koeling
// RS485B   verwarming

#define RS485DE_PIN     GPIO_NUM_6   // CANtx      
#define RS485TX_PIN     GPIO_NUM_44  // RX0    


void updatePID ( float temperature);
extern float PIDsetting; // for cgi

typedef enum { THERMOSTATOFF, HEATING_ON, COOLING_ON } thermostatStatus_t;

extern thermostatStatus_t thermostatStatus;

#endif /* MAIN_INCLUDE_PID_H_ */

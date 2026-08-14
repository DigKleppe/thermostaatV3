/**
 * @file main
 *
 */

/*********************
 *      INCLUDES
 *********************/
#include <stdlib.h>
#include <unistd.h>
#include "MeasScreen.h"
#include "fonts.h"

#include <stdio.h>
#include "styles.h"
#include "backGround.h"
#include "lcd.h"
#include "settings.h"

#ifdef LV_CONF_INCLUDE_SIMPLE
#include "lvgl.h"
#else
#include "lvgl/lvgl.h"
#endif

#ifndef LV_HOR_RES_MAX
	#define LV_HOR_RES_MAX BSP_LCD_H_RES
	#define LV_VER_RES_MAX BSP_LCD_V_RES
#endif

#define SHOWSETPOINT 


// fonts Ohm, micro  0x3A9,0x3BC 01234567890 -.,mMnkVAHz

#define PADDING 12 // 8
#define ITEMHEIGHT	100 // 80
#define ITEMWIDTH	200
#define FIRSTY 		60  //CLOCKDISPLAYHEIGHT
#define SPINBOXX	380
#define SPINBOXY	10



static const SpinBoxDescr_t  spinBoxDescr = {
												  .name = "Gewenste temperatuur:",
												  .format = "%2.1f",
												  .maxVal = 30.0,
												  .minVal = 10.0,
												  .step = 0.1,
												  .var = &userSettings.temperatureSetpoint,
											 };
											

//const char units[4][7] { "\xC2" "\xB0" "C", "%RH",  "ppm","" };
static const char * units[] = { "\xC2\xB0" "C", "%" ,"ppm"};
static const char * formats[] = { "%2.1f","%2.1f", "%d"};
//static const char * name[] = { "Temperatuur:","Luchtvochtigheid:", "CO2:"};
static const char * name[] = { "T:","RH:", "CO2:"};

MeasScreen::MeasScreen( ) {

	screen = lv_obj_create(NULL);
	backGround =  makeBackGround(screen);
	clockDisplay = new ClockDisplay(backGround);
//	statusIndicator = new StatusIndicator(backGround);

	for (int n = 0; n < NR_ITEMS; n++) {
		measDisplay[n] = new MeasDisplay (backGround,FIRSTY + n * (ITEMHEIGHT + PADDING) + PADDING,  name[n], units[n], formats[n]);
	}
	statusLine = new StatusLine (backGround);
	statusLine->setText( NULL);
	navigArrows = new NavigArrows(backGround, true, true);
	#ifdef SHOWSETPOINT
	spinbox = new VerticalSpinbox( backGround,SPINBOXX,SPINBOXY,&spinBoxDescr);
	#endif
	

//	setStatusIndicator(HEATING_ON);


}

void MeasScreen::setDisplayText(int line, char *text) {
	measDisplay[line]->setText ( text);
	// if (line == 0)
	// 	printf ("\n\r %d: %s", line, text);
	// else
	// 	printf ("\t %d: %s", line, text);
}

void MeasScreen::setDisplayValue(int line , float value) {
	measDisplay[line]->setValue ( value );

}

void MeasScreen::setStatusLine(const char * text) {
	if( strlen( text) > 0) {
		statusLine->setText( text);
	}
	else
		statusLine->setText( NULL);
}



void MeasScreen::setStatusIndicator (thermostatStatus_t status) {
	switch (status) {
	case THERMOSTATOFF:
		statusIndicator->setSymbol(NULL);
		break;
	case HEATING_ON:
		statusIndicator->setSymbol(HEATERON_SIMBOL);
		break;
	case COOLING_ON:
		statusIndicator->setSymbol(COOLINGON_SYMBOL);
		break;
	}
}

void MeasScreen::setDisplayValue(int line , int value) {
	measDisplay[line]->setValue ( value );
}

void MeasScreen::setValueAndName(int line, const char *value, const char *name) {
	setDisplayText( line,(char *)  value);
}

void MeasScreen::setClockDisplayText(const char *text){
	clockDisplay->setText(text);
}


void MeasScreen::show() {
	lv_scr_load(screen);
}



MeasScreen::~MeasScreen() {
	// TODO Auto-generated destructor stub
}

void makeMeasScreen(void) {

}


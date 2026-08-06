/*
 * MeasScreen.h
 *
 *  Created on: Mar 2, 2021
 *      Author: dig
 */

#ifndef COMPONENTS_GUI_MAINSCREEN_H_
#define COMPONENTS_GUI_MAINSCREEN_H_

#ifdef LV_CONF_INCLUDE_SIMPLE
#include "lvgl.h"
#else
#include "lvgl/lvgl.h"
#endif
//http://www.ltg.ed.ac.uk/~richard/utf-8.cgi?input=3a9&mode=hex

//#include "Stream.h"

//#include "StatusLine.h"
#include "SpinBox.h"
#include "NavigArrows.h"
#include "ClockDisplay.h"
#include "StatusIndicator.h"
#include "MeasDisplay.h"

//typedef struct {
//	float temperature;
//	float tempSetPoint;
//	bool heatingOn;
//	bool coolingOn;
//} mainScreenVars_t;
#define  NR_SPINBOXES  2
typedef enum { CBHEATING, CBCOOLING,NR_CHECKBOXES } checBox_t;

class MainScreen { //public Stream{
public:
	MainScreen();
	virtual ~MainScreen();
	void show();
//	void setValues( mainScreenVars_t * );
//	void getValues( mainScreenVars_t * );
//	void setTemperatureDisplayValue( float value);
//	void setTemperatureDisplayText ( char * text);
	static const int MAXVALUECHARS = 8;
	void update (void);

private:
	char  measValue1[MAXVALUECHARS+3]; // extra space for a symbol (3 characters)
	static void event_handler(lv_obj_t * obj, lv_event_t event);
	static void screenClicked(lv_event_t * event);
	lv_obj_t * backGround;
	MeasDisplay * measDisplay;
//	SpinBox * spinBoxTemperatuur;
	SpinBox * spinBox[NR_SPINBOXES];
	lv_obj_t * cb[NR_CHECKBOXES];
	lv_obj_t * screen;
	lv_obj_t * infoLabel;
	NavigArrows * navigArrows;
	ClockDisplay * clockDisplay;
	StatusIndicator* statusIndicator;

	static void event_handler(lv_event_t * e);
};

#endif /* COMPONENTS_GUI_MEASSCREEN_H_ */

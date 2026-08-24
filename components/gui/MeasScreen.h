/*
 * MeasScreen.h
 *
 *  Created on: Mar 2, 2021
 *      Author: dig
 */

#ifndef COMPONENTS_GUI_MEASSCREEN_H_
#define COMPONENTS_GUI_MEASSCREEN_H_

#ifdef LV_CONF_INCLUDE_SIMPLE
#include "lvgl.h"
#else
#include "lvgl/lvgl.h"
#endif
// http://www.ltg.ed.ac.uk/~richard/utf-8.cgi?input=3a9&mode=hex

// #include "Stream.h"

// #include "StatusLine.h"
#include "ClockDisplay.h"
#include "MeasDisplay.h"
#include "NavigArrows.h"
#include "StatusIndicator.h"
#include "StatusLine.h"
#include "VerticalSpinbox.h"

#include "PID.h"

#define LV_SYMBOL_OHM "\xef\xCE\xA9"   // 0x3A9
#define LV_SYMBOL_MICRO "\xef\xCE\xBC" // 0x3BC

#define NR_ITEMS 3

// #define SHOWSETPOINT

class MeasScreen { // public Stream{
public:
	MeasScreen();
	virtual ~MeasScreen();
	static const int MAXVALUECHARS = 4;
	void show();
	void setDisplayText(int line, char *text);
	void setDisplayValue(int line, float value);
	void setDisplayValue(int line, int value);
	void setValueAndName(int line, const char *value, const char *name);
	void setStatusIndicator(thermostatStatus_t);
	void setStatusLine(const char *text);
	void setClockDisplayText(const char *text);
	void setClockDisplayOutsideTemp(const char *text);

	lv_obj_t *screen;

	//    int read(void);
	//    size_t read(uint8_t *buffer, size_t size);
	//    inline size_t read(char * buffer, size_t size)
	//    {
	//        return read((uint8_t*) buffer, size);
	//    }
	//    void flush(void);
	//    void flush( bool txOnly);
	//    size_t write(uint8_t);
	//    size_t write(const uint8_t *buffer, size_t size);
	//    inline size_t write(const char * buffer, size_t size)
	//    {
	//        return write((uint8_t*) buffer, size);
	//    }
	//    inline size_t write(const char * s)
	//    {
	//        return write((uint8_t*) s, strlen(s));
	//    }
	//    inline size_t write(unsigned long n)
	//    {
	//        return write((uint8_t) n);
	//    }
	//    inline size_t write(long n)
	//    {
	//        return write((uint8_t) n);
	//    }
	//    inline size_t write(unsigned int n)
	//    {
	//        return write((uint8_t) n);
	//    }
	//    inline size_t write(int n)
	//    {
	//        return write((uint8_t) n);
	//    }
	//

private:
	char measValue1[MAXVALUECHARS + 3]; // extra space for a symbol (3 characters)
	static void event_handler(lv_obj_t *obj, lv_event_t event);
	static void screenClicked(lv_event_t *event);
	lv_obj_t *backGround;
	lv_obj_t *valueLabel[NR_ITEMS];
	lv_obj_t *label[NR_ITEMS];
	StatusLine *statusLine;
	NavigArrows *navigArrows;
	ClockDisplay *clockDisplay;
	MeasDisplay *measDisplay[NR_ITEMS];
	StatusIndicator *statusIndicator;
	VerticalSpinbox *spinbox;
	lv_obj_t *setpointLabel;
	lv_obj_t *setpointValueLabel;

public:
	void setSetpointValue(void);
};

#endif /* COMPONENTS_GUI_MEASSCREEN_H_ */

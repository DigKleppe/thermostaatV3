/*
 * MeasScreen.h
 *
 *  Created on: Mar 2, 2021
 *      Author: dig
 */

#ifndef COMPONENTS_GUI_WIFISELECTSCREEN_H_
#define COMPONENTS_GUI_WIFISELECTSCREEN_H_

#ifdef LV_CONF_INCLUDE_SIMPLE
#include "lvgl.h"
#else
#include "lvgl/lvgl.h"
#endif

#include "NavigArrows.h"

class WifiSelectScreen {
public:
	WifiSelectScreen();
	virtual ~WifiSelectScreen();
	void show();
	void update (void);

private:
	static void event_handler(lv_obj_t * obj, lv_event_t event);
	static void kb_event_cb(lv_event_t *e);
    static void ta_event_cb(lv_event_t *e);
    static void btn_scan_event_cb(lv_event_t *e);
    static void btn_connect_event_cb(lv_event_t *e);
   	lv_obj_t * screen;
	lv_obj_t * backGround;
	NavigArrows * navigArrows;
};

#endif /* COMPONENTS_GUI_MEASSCREEN_H_ */

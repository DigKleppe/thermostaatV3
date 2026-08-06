/*
 * InfoScreen.h
 *
 *  Created on: Apr 15, 2022
 *      Author: dig
 */

#ifndef GUI_INFOSCREEN_H_
#define GUI_INFOSCREEN_H_

#ifdef LV_CONF_INCLUDE_SIMPLE
#include "lvgl.h"
#else
#include "lvgl/lvgl.h"
#endif

#include "InfoDisplay.h"
#include "NavigArrows.h"

#define MAXNR_INFO 10



class InfoScreen {
public:
	InfoScreen(const infoDescr_t * descr);
	virtual ~InfoScreen();
	void upDate (void);
	void show();
private:
	lv_obj_t * screen;
	InfoDisplay * display[ MAXNR_INFO];

};

#endif /* GUI_INFOSCREEN_H_ */

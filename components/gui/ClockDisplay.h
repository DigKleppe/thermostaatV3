/*
 * ClockDisplay.h
 *
 *  Created on: Apr 3, 2022
 *      Author: dig
 */

#ifndef GUI_CLOCKDISPLAY_H_
#define GUI_CLOCKDISPLAY_H_

#ifdef LV_CONF_INCLUDE_SIMPLE
#include "lvgl.h"
#else
#include "lvgl/lvgl.h"
#endif

#define CLOCKDISPLAYHEIGHT 40

class ClockDisplay {
public:
	ClockDisplay(lv_obj_t * parent, lv_coord_t y= -1);
	virtual ~ClockDisplay();
	void setText (const char *);
	void setOutsideTemp (const char *);

private:
	lv_obj_t* _parent;
	lv_obj_t* label;
	
	lv_obj_t* nameLabel; 
	lv_obj_t* outTempLabel; 
};

#endif /* GUI_CLOCKDISPLAY_H_ */

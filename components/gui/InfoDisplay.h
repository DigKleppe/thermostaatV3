/*
 * ClockDisplay.h
 *
 *  Created on: Apr 3, 2022
 *      Author: dig
 */

#ifndef GUI_INFODISPLAY_H_
#define GUI_INFODISPLAY_H_

#ifdef LV_CONF_INCLUDE_SIMPLE
#include "lvgl.h"
#else
#include "lvgl/lvgl.h"
#endif

typedef struct {
	const char * name;
	const char * format;
	const void * value;
}infoDescr_t;


#define INFODISPLAYHEIGHT 30

class InfoDisplay {
public:
	InfoDisplay(lv_obj_t * parent,  int line,const infoDescr_t *descr);
	virtual ~InfoDisplay();
	void Update (void);

private:
	lv_obj_t* _parent;
	lv_obj_t* label;
	lv_obj_t* textDisplay;
	const infoDescr_t * _descr;
};

#endif /* GUI_CLOCKDISPLAY_H_ */

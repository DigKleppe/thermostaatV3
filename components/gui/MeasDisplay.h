/*
 * measDisplay.h
 *
 *  Created on: Apr 6, 2022
 *      Author: dig
 */

#ifndef COMPONENTS_GUI_MEASDISPLAY_H_
#define COMPONENTS_GUI_MEASDISPLAY_H_

#ifdef LV_CONF_INCLUDE_SIMPLE
#include "lvgl.h"
#else
#include "lvgl/lvgl.h"
#endif

#include "styles.h"
#include <stdio.h>


class MeasDisplay {
public:
	MeasDisplay( lv_obj_t * parent, int y, const char *name = NULL, const char * unit = NULL, const char * formatStr= NULL);
	void setValue (int value);
	void setValue (float value);
	void setText ( char * text);
	virtual ~MeasDisplay();

private:
	char format[10];
	lv_obj_t * _parent;
	lv_obj_t * valueLabel;
	lv_obj_t * nameLabel;
	lv_obj_t * unitLabel;
};

#endif /* COMPONENTS_GUI_MEASDISPLAY_H_ */

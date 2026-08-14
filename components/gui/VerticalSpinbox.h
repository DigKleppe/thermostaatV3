/*
 * NavigArrows.h
 *
 *  Created on: Mar 21, 2022
 *      Author: dig
 */

#ifndef GUI_VSPINBOX_H_
#define GUI_VSPINBOX_H_

#ifdef LV_CONF_INCLUDE_SIMPLE
#include "lvgl.h"
#else
#include "lvgl/lvgl.h"
#endif

#include "styles.h"
#include "SpinBox.h"

class VerticalSpinbox {
public:
	VerticalSpinbox(lv_obj_t* parent, int x, int y, const SpinBoxDescr_t *desc, lv_style_t* style = &styleNavArrows ); 
	virtual ~VerticalSpinbox();
	void setStyle(lv_style_t * style);
	SpinBoxDescr_t myDesc;
	lv_obj_t * valueLabel;

private:
	lv_obj_t * buttonUp;
	lv_obj_t * buttonDown;

	lv_obj_t * _parent;


	void setText(char *text); 
	void setValue(float value); 
};

#endif /* GUI_VSPINBOX_H_ */

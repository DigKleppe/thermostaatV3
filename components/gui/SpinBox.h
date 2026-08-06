/*
 * Spinbox.h
 *
 *  Created on: Mar 19, 2022
 *      Author: dig
 */

#ifndef GUI_SPINBOX_H_
#define GUI_SPINBOX_H_


#include "lcd.h"

#ifdef LV_CONF_INCLUDE_SIMPLE
#include "lvgl.h"
#else
#include "lvgl/lvgl.h"
#endif

#ifndef LV_HOR_RES_MAX
	#define LV_HOR_RES_MAX BSP_LCD_H_RES
	#define LV_VER_RES_MAX BSP_LCD_V_RES
#endif




typedef struct
{ 	const char * name;
	const char  *format;
	float maxVal;
	float minVal;
	float step;
	float *var;
	lv_obj_t *label;
}SpinBoxDescr_t;


// static const int spinBoxButtonHeigth	= 75;
// static const int spinBoxButtonWidth		= 100;
// static const int MAXCHARS				= 4;
// static const int NAMELABELHEIGHT		= 20;
// static const int PADDING				= 20;
// const int SPINBOXHEIGHT 				= (spinBoxButtonHeigth + NAMELABELHEIGHT + PADDING);

class SpinBox  { //public Stream{
public:
	SpinBox(lv_obj_t * parent, int line,const SpinBoxDescr_t *desc );
//	void init (SpinBoxDescr_t * desc, int y );
	void upDate (void);
	virtual ~SpinBox();
	SpinBoxDescr_t myDesc;
private:
	lv_obj_t* label;
	lv_obj_t* buttonP;
	lv_obj_t* buttonM;
	lv_obj_t *_parent;

};


#endif /* GUI_SPINBOX_H_ */

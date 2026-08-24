/*
 * NavigArrows.cpp
 *
 *  Created on: Mar 21, 2022
 *      Author: dig
 *
 *      0xF060,0xF061,0xF053,0xF054
 *      0123456789,:.-+%
 */

#include <NavigArrows.h>
#include "fonts.h"
#include "stdio.h"

void nextScreenClick (lv_event_t * e);
void prevScreenClick (lv_event_t * e);

#ifndef NAVIGARROWS_FONT
LV_FONT_DECLARE(lv_font_montserrat_44)
#define NAVIGARROWS_FONT	lv_font_montserrat_44 //Awsome50
#endif
#define BUTTONHEIGHT	70
#define BUTTONWIDTH		100

static lv_style_t navStyle;
static bool styleIsSet;


static void rightClick(lv_event_t * e)
{
   
//	printf( "next:\n");
	lv_event_code_t code = lv_event_get_code(e);
    if(code == LV_EVENT_SHORT_CLICKED)
    {
    	nextScreenClick(e);
		printf( "next: sc\n");
    }
}


static void leftClick(lv_event_t * e)
{
  //  printf( "prev:\n");
	lv_event_code_t code = lv_event_get_code(e);
    if(code == LV_EVENT_SHORT_CLICKED ) {
    	prevScreenClick(e);
		printf( "prev: sc\n");
    }
}



NavigArrows::NavigArrows(lv_obj_t* parent, bool right, bool left, lv_style_t* style ) {
	lv_obj_t * lbel;
	_parent = parent;
	if ( style != NULL) {
	if (!styleIsSet) {
		styleIsSet = true;
		lv_style_init(&navStyle);
		lv_style_set_text_font(&navStyle, &NAVIGARROWS_FONT);
		lv_color_t c = lv_color_make(255, 255, 0);
		lv_style_set_text_color(&navStyle, c);
	}
	}

	if (right) {
		buttonRight = lv_btn_create(_parent);
		lv_obj_set_size(buttonRight, BUTTONWIDTH, BUTTONHEIGHT);
		lv_obj_align(buttonRight, LV_ALIGN_BOTTOM_RIGHT, 13, 15);
		if ( style != NULL)
			lv_obj_add_style(buttonRight,style, 0);
		else
			lv_obj_add_style(buttonRight,&navStyle, 0);
		lv_obj_set_style_bg_img_src(buttonRight, LV_SYMBOL_RIGHT, _LV_STYLE_STATE_CMP_SAME);
		lv_obj_add_event_cb(buttonRight, rightClick, LV_EVENT_ALL, this);

		lbel = lv_label_create(buttonRight);
		lv_obj_add_style( lbel, &styleSpinButton,0);
		lv_label_set_text(lbel, ">");
		lv_obj_center(lbel);
	}

	if (left) {
		buttonLeft = lv_btn_create(_parent);
		lv_obj_set_size(buttonLeft, BUTTONWIDTH, BUTTONHEIGHT);
		lv_obj_align(buttonLeft, LV_ALIGN_BOTTOM_LEFT,-13,15);
		if ( style != NULL)
			lv_obj_add_style(buttonLeft,style, 0);
		else
			lv_obj_add_style(buttonLeft,&navStyle, 0);
		lv_obj_set_style_bg_img_src(buttonLeft, LV_SYMBOL_LEFT, _LV_STYLE_STATE_CMP_SAME);
		lv_obj_add_event_cb(buttonLeft, leftClick, LV_EVENT_ALL, this);

		lbel = lv_label_create(buttonLeft);
		lv_obj_add_style( lbel, &styleSpinButton,0);
		lv_label_set_text(lbel, "<");
		lv_obj_center(lbel);
	}
}




NavigArrows::~NavigArrows() {
	// TODO Auto-generated destructor stub
}


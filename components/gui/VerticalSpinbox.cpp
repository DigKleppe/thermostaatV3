/*
 * NavigArrows.cpp
 *
 *  Created on: Mar 21, 2022
 *      Author: dig
 *
 *      0xF060,0xF061,0xF053,0xF054
 *      0123456789,:.-+%
 */

#ifdef LV_CONF_INCLUDE_SIMPLE
#include "lvgl.h"
#else
#include "lvgl/lvgl.h"
#endif

#include "fonts.h"
#include <VerticalSpinbox.h>
#include <stdio.h>

#ifndef NAVIGARROWS_FONT
LV_FONT_DECLARE(lv_font_montserrat_44)
#define NAVIGARROWS_FONT lv_font_montserrat_44 // Awsome50
#endif
#define BUTTONHEIGHT 70
#define BUTTONWIDTH 70
#define VALUEWIDTH 150
#define VALUEHEIGHT 50
#define PADDING 30 // vertical
#define MAXCHARS 5

extern bool settingsChanged;

static lv_style_t navStyle;
static bool styleIsSet;

void VerticalSpinbox::setText(char *text) { lv_label_set_text(valueLabel, text); }

void VerticalSpinbox::setValue(float value) {
	char str[20];
	sprintf(str, "%1.2f", value);
	setText(str);
}

static void upClick(lv_event_t *e) {
	lv_event_code_t code = lv_event_get_code(e);
	if (code == LV_EVENT_SHORT_CLICKED || code == LV_EVENT_LONG_PRESSED_REPEAT) {
		VerticalSpinbox *ps = (VerticalSpinbox *)e->user_data;
		SpinBoxDescr_t *p = &ps->myDesc;
		*p->var += p->step;
		if (*p->var > p->maxVal)
			*p->var = p->maxVal;
		char str[MAXCHARS + 1];
		sprintf(str, p->format, *p->var);
		lv_label_set_text(ps->valueLabel, str);
		settingsChanged = true;
	}
}

static void downClick(lv_event_t *e) {
	lv_event_code_t code = lv_event_get_code(e);
	if (code == LV_EVENT_SHORT_CLICKED || code == LV_EVENT_LONG_PRESSED_REPEAT) {
		VerticalSpinbox *ps = (VerticalSpinbox *)e->user_data;
		SpinBoxDescr_t *p = &ps->myDesc;
		*p->var -= p->step;
		if (*p->var < p->minVal)
			*p->var = p->minVal;
		char str[MAXCHARS + 1];
		sprintf(str, p->format, *p->var);
		lv_label_set_text(ps->valueLabel, str);
		settingsChanged = true;
	}
}

VerticalSpinbox::VerticalSpinbox(lv_obj_t *parent, int x, int y, const SpinBoxDescr_t *desc, lv_style_t *style) {
	lv_obj_t *lbel;
	_parent = parent;
	myDesc = *desc;

	if (style != NULL) {
		if (!styleIsSet) {
			styleIsSet = true;
			lv_style_init(&navStyle);
			lv_style_set_text_font(&navStyle, &NAVIGARROWS_FONT);
			lv_color_t c = lv_color_make(255, 255, 0);
			lv_style_set_text_color(&navStyle, c);
		}
	}

	buttonUp = lv_btn_create(_parent);
	lv_obj_set_size(buttonUp, BUTTONWIDTH, BUTTONHEIGHT);
	lv_obj_set_pos(buttonUp, x, y);
	lv_obj_add_style(buttonUp, style, 0);
	lv_obj_set_style_bg_img_src(buttonUp, LV_SYMBOL_RIGHT, _LV_STYLE_STATE_CMP_SAME);
	lv_obj_add_event_cb(buttonUp, upClick, LV_EVENT_ALL, this);

	lbel = lv_label_create(buttonUp);
	lv_obj_add_style(lbel, &styleSpinButton, 0);
	lv_label_set_text(lbel, "+");
	lv_obj_center(lbel);

	valueLabel = lv_label_create(_parent);
	lv_obj_align_to(valueLabel, buttonUp, LV_ALIGN_CENTER, -10, PADDING + VALUEHEIGHT / 2);

	// lv_obj_set_pos(valueLabel, x, y + BUTTONHEIGHT + PADDING);
	lv_obj_add_style(valueLabel, &styleMeasName, 0);
	// lv_label_set_text(valueLabel, name);
	lv_obj_set_style_text_align(valueLabel, LV_TEXT_ALIGN_RIGHT, 0);

	buttonDown = lv_btn_create(_parent);
	lv_obj_set_size(buttonDown, BUTTONWIDTH, BUTTONHEIGHT);
	// lv_obj_set_pos(buttonDown, x, y + BUTTONHEIGHT + PADDING);
	lv_obj_align_to(buttonDown, valueLabel, LV_ALIGN_CENTER, 0, PADDING + BUTTONHEIGHT / 2);

	lv_obj_add_style(buttonDown, style, 0);
	lv_obj_set_style_bg_img_src(buttonDown, LV_SYMBOL_LEFT, _LV_STYLE_STATE_CMP_SAME);
	lv_obj_add_event_cb(buttonDown, downClick, LV_EVENT_ALL, this);

	lbel = lv_label_create(buttonDown);
	lv_obj_add_style(lbel, &styleSpinButton, 0);
	lv_label_set_text(lbel, "-");
	lv_obj_center(lbel);

	char str[MAXCHARS + 1];
	sprintf(str, myDesc.format, *myDesc.var);
	lv_label_set_text(valueLabel, str);
}

VerticalSpinbox::~VerticalSpinbox() {
	// TODO Auto-generated destructor stub
}

/*
 * ClockDisplay.cpp
 *
 *  Created on: Apr 3, 2022
 *      Author: dig
 */

#include "styles.h"
#include <ClockDisplay.h>

// void registerTimeUpdate( ClockDisplay * );

ClockDisplay::ClockDisplay(lv_obj_t *parent, lv_coord_t y) {
	_parent = parent;

	label = lv_label_create(_parent);
	int h = CLOCKFONT.line_height;

	lv_obj_add_style(label, &styleClock, 0);
	lv_obj_set_size(label, 240, h + 4);
	lv_label_set_text(label, "");
	lv_obj_set_pos(label, 0, 0);

	nameLabel = lv_label_create(_parent);
	lv_obj_set_pos(nameLabel, 220, 0);
	lv_obj_add_style(nameLabel, &styleMeasName, 0);
	lv_obj_set_size(nameLabel, 50, h + 4);
	lv_label_set_text(nameLabel, "");
	lv_obj_set_style_text_align(nameLabel, LV_TEXT_ALIGN_RIGHT, 0);

	outTempLabel = lv_label_create(_parent);
	lv_obj_add_style(outTempLabel, &styleClock, 0);
	lv_obj_set_size(outTempLabel, 160, h + 4);
	lv_label_set_text(outTempLabel, "");
	lv_obj_set_pos(outTempLabel, 280, 0);
}

void ClockDisplay::setText(const char *str) { lv_label_set_text(label, str); }
void ClockDisplay::setOutsideTemp(const char *str) {
	if (strlen(str) > 0)
		lv_label_set_text(nameLabel, "Tb:");
	else
		lv_label_set_text(nameLabel, "");
	lv_label_set_text(outTempLabel, str);
}

ClockDisplay::~ClockDisplay() {
	// TODO Auto-generated destructor stub
}

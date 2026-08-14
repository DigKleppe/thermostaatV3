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
	lv_label_set_text(label, "12:34:56");
	lv_obj_set_pos(label, 0, 1);
}

void ClockDisplay::setText(const char *str) {
	 lv_label_set_text(label, str); 
}

ClockDisplay::~ClockDisplay() {
	// TODO Auto-generated destructor stub
}

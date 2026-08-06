/*
 * ClockDisplay.cpp
 *
 *  Created on: Apr 3, 2022
 *      Author: dig
 */

#include <ClockDisplay.h>
#include "styles.h"

void registerTimeUpdate( ClockDisplay * );



ClockDisplay::ClockDisplay(lv_obj_t*  parent, lv_coord_t y) {
	_parent = parent;

//	if ( style != NULL) {

	label = lv_label_create ( _parent);

	int h = CLOCKFONT.line_height;

	lv_obj_add_style( label, &styleClock,0);
	lv_obj_set_size(label, 240,h + 4);
	lv_label_set_text(label,"12:34:56");
	lv_obj_set_pos( label, 280, 1);
	
	// if( y == -1)
	// 	lv_obj_set_align(label, LV_ALIGN_TOP_MID);
	// else
	//	lv_obj_set_style_text_align(label, LV_TEXT_ALIGN_CENTER, 0);

	registerTimeUpdate(this);


}


void ClockDisplay::setText ( char * str){
	lv_label_set_text(label,str);
}


ClockDisplay::~ClockDisplay() {
	// TODO Auto-generated destructor stub
}


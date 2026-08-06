/*
 * StatusIndicator.cpp
 *
 *  Created on: Apr 13, 2022
 *      Author: dig
 */

#include <StatusIndicator.h>



StatusIndicator::StatusIndicator(lv_obj_t * parent) {
	_parent = parent;
	label = lv_label_create ( _parent);
	lv_obj_add_style( label, &styleSymbol,0);
	lv_obj_align(label, LV_ALIGN_TOP_RIGHT, 0,0);
	lv_label_set_text(label,NULL);
}


void StatusIndicator::setSymbol ( const char *symbol) {
	if ( symbol == NULL)
		lv_obj_add_flag(label, LV_OBJ_FLAG_HIDDEN);
	else {
		lv_label_set_text(label,symbol);
		lv_obj_clear_flag(label, LV_OBJ_FLAG_HIDDEN);
	}

}

StatusIndicator::~StatusIndicator() {
	// TODO Auto-generated destructor stub
}


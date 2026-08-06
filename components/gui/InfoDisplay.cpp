/*
 * InfoDisplay.cpp
 *
 *  Created on: Apr 3, 2022
 *      Author: dig
 */

#include <InfoDisplay.h>
#include "styles.h"
#include <stdio.h>


InfoDisplay::InfoDisplay(lv_obj_t * parent, int line,  const infoDescr_t *descr )
{
	_parent = parent;
	_descr = descr;

	label = lv_label_create ( _parent);
	lv_obj_set_pos( label, 0,line * (INFOFONT.line_height + 10));
	lv_obj_add_style( label, &styleInfo,0);
	lv_obj_set_style_text_align(label, LV_TEXT_ALIGN_LEFT, 0);
	lv_label_set_text(label,descr->name);

	textDisplay = lv_label_create ( _parent);
	lv_obj_set_pos( textDisplay, LV_HOR_RES/2 -50 ,line * (INFOFONT.line_height + 10));
	lv_obj_add_style( textDisplay, &styleInfo,0);
	lv_obj_set_style_text_align(textDisplay, LV_TEXT_ALIGN_LEFT, 0);
}

void InfoDisplay::Update ( void){
	char str [20];

	if ( strstr(_descr->format, "s"))
		snprintf ( str , sizeof(str)-1, _descr->format, _descr->value);
	else
		if ( strstr(_descr->format, "d"))
			snprintf ( str, sizeof(str)-1 , _descr->format, *( int *)  _descr->value);
		else
			snprintf ( str, sizeof(str)-1 , _descr->format, *( float *)  _descr->value);

	lv_label_set_text(textDisplay,str);
}


InfoDisplay::~InfoDisplay() {
	// TODO Auto-generated destructor stub
}


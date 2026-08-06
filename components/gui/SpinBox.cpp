/*
 * spinBox.cpp
 *
 *  Created on: Mar 19, 2022
 *      Author: dig
 */

#include "SpinBox.h"
#include <stdio.h>
#include "fonts.h"
#include "styles.h"

#define TOPSPACING		5 
#define SPACING			10 // between lines
#define TITLESPACING	5  // between title and keys
#define BUTTONHEIGHT	70
#define BUTTONWIDTH		100
#define TOTALHEIGHT		(SPINBUTTONNAMEFONT.line_height + TITLESPACING + BUTTONHEIGHT+ 20)
#define VALUEWIDTH		140
#define MAXCHARS 		5

extern bool settingsChanged;

static void lv_spinbox_increment_event_cb(lv_event_t * e)
{
    lv_event_code_t code = lv_event_get_code(e);
    if(code == LV_EVENT_SHORT_CLICKED || code  == LV_EVENT_LONG_PRESSED_REPEAT) {
      	SpinBox* ps =(SpinBox*)  e->user_data;
    	SpinBoxDescr_t *p = &ps->myDesc;
    	*p->var += p->step;
    	if ( *p->var > p->maxVal)
    		*p->var= p->maxVal;
    	char str[MAXCHARS+1];
    	sprintf( str, p->format, *p->var);
    	lv_label_set_text(p->label,str);
    	settingsChanged = true;
    }
}


static void lv_spinbox_decrement_event_cb(lv_event_t * e)
{
    lv_event_code_t code = lv_event_get_code(e);
    if(code == LV_EVENT_SHORT_CLICKED || code == LV_EVENT_LONG_PRESSED_REPEAT) {
      	SpinBox* ps =(SpinBox*)  e->user_data;
    	SpinBoxDescr_t *p = &ps->myDesc;
    	*p->var -= p->step;
    	if ( *p->var < p->minVal)
    	   	*p->var= p->minVal;
    	char str[MAXCHARS+1];
    	sprintf( str, p->format, *p->var);
    	lv_label_set_text(p->label,str);
    	settingsChanged = true;

    }
}


SpinBox::SpinBox(lv_obj_t * parent, int line,const SpinBoxDescr_t *desc ){
	_parent = parent;

	lv_coord_t yPos = TOPSPACING + line * TOTALHEIGHT;
	lv_obj_t * namelabel;
	myDesc = *desc;


//// name label
	if ( strlen(myDesc.name) >0 ){
		namelabel = lv_label_create ( _parent);//  lv_spinbox_create(parent);
		lv_obj_add_style( namelabel, &styleSpinButtonName,0);
		lv_obj_set_pos( namelabel, 10,  yPos);
		lv_label_set_text(namelabel,myDesc.name);
	}

 //value label
	label = lv_label_create ( _parent);//  lv_spinbox_create(parent);
	lv_obj_set_size(label, VALUEWIDTH,BUTTONHEIGHT);

//	if ( strlen(myDesc.name) >0 )
//		lv_obj_set_pos( label, (LV_HOR_RES_MAX-140)/2, y +spinBoxButtonHeigth/2 -5);
//	else
//		lv_obj_set_pos( label, (LV_HOR_RES_MAX-140)/2, y);
//	lv_obj_align(label, LV_ALIGN_CENTER, 0, (-(LV_VER_RES_MAX/2)) + spinBoxButtonHeigth/2 + yPos   + 65);
	
	lv_obj_align(label, LV_ALIGN_CENTER, 0, (-(LV_VER_RES_MAX/2)) + yPos + BUTTONHEIGHT + 40);
	lv_obj_add_style( label, &styleSpin,0);
	lv_obj_set_style_text_align(label, LV_TEXT_ALIGN_CENTER, 0);
	myDesc.label = label;

	buttonP = lv_btn_create(_parent);
	lv_obj_set_size(buttonP, BUTTONWIDTH,  BUTTONHEIGHT);
	lv_obj_align_to(buttonP, label, LV_ALIGN_OUT_RIGHT_MID, 0, -20);

	lv_obj_t* lbel = lv_label_create(buttonP);
	lv_obj_add_style( lbel, &styleSpinButton,0);
	lv_label_set_text(lbel, "+");
	lv_obj_center(lbel);

	lv_obj_add_style( buttonP, &styleSpinButton,0);
	lv_obj_add_event_cb(buttonP, lv_spinbox_increment_event_cb, LV_EVENT_ALL, this);

	buttonM = lv_btn_create(_parent);
	lv_obj_set_size(buttonM,BUTTONWIDTH, BUTTONHEIGHT);
	lv_obj_align_to(buttonM, label, LV_ALIGN_OUT_LEFT_MID, 0, -20);

	lbel = lv_label_create(buttonM);
	lv_obj_add_style( lbel, &styleSpinButton,0);
	lv_label_set_text(lbel, "-");
	lv_obj_center(lbel);

	lv_obj_add_style( buttonM, &styleSpinButton,0);
	lv_obj_add_event_cb(buttonM, lv_spinbox_decrement_event_cb, LV_EVENT_ALL, this);

	char str[MAXCHARS+1];
	sprintf( str,myDesc.format, *myDesc.var);
	lv_label_set_text(label,str);
}


void SpinBox::upDate ( void){
	char str[MAXCHARS+1];
    sprintf( str, myDesc.format, *myDesc.var);
   	lv_label_set_text(label,str);
}


SpinBox::~SpinBox() {
	// TODO Auto-generated destructor stub
}



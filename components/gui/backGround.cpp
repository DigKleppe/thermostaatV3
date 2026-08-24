/*
 * backGround.cpp
 *
 *  Created on: Apr 4, 2022
 *      Author: dig
 */

#include "backGround.h"
#include <stdio.h>

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
int cntr;

// static void screenClicked(lv_event_t *e) {
// 	cntr++;
// 	char txt[20];
// 	sprintf(txt, "%d", cntr);
// 	printf("Clicked\n\r");
// }

lv_obj_t* makeBackGround(lv_obj_t *parent) {

	lv_obj_t *backGround;
	backGround = lv_obj_create(parent);

	lv_obj_set_size(backGround, LV_HOR_RES_MAX, LV_VER_RES_MAX);
	lv_obj_clear_flag(backGround, LV_OBJ_FLAG_SCROLLABLE);
	lv_obj_set_pos(backGround, 0, 0);
	lv_obj_add_style(backGround, &style_background, 0);
//	lv_obj_add_event_cb(backGround, screenClicked, LV_EVENT_CLICKED, NULL); /*Assign an event callback*/
	lv_obj_clear_flag(backGround, LV_OBJ_FLAG_GESTURE_BUBBLE);
	lv_obj_add_flag(backGround, LV_OBJ_FLAG_CLICKABLE);
	return backGround;
}

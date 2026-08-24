/*
 * backGround.cpp
 *
 *  Created on: Apr 4, 2022
 *      Author: dig
 */

#include "StartScreen.h"
#include "lcd.h"
#include "softwareVersions.h"
#include "styles.h"
#include <stdio.h>

#ifdef LV_CONF_INCLUDE_SIMPLE
#include "lvgl.h"
#else
#include "lvgl/lvgl.h"
#endif

#ifndef LV_HOR_RES_MAX
#define LV_HOR_RES_MAX BSP_LCD_H_RES
#define LV_VER_RES_MAX BSP_LCD_V_RES
#endif

#define SPACING 100
#define XPOS 10

#define LABELWIDTH (LV_HOR_RES_MAX-XPOS)
#define LABELHEIGTH (SPACING/2)

StartScreen::StartScreen(void) {
	screen = lv_obj_create(NULL);
	char str[80];


	lv_obj_set_size(screen, LV_HOR_RES_MAX, LV_VER_RES_MAX);
	lv_obj_clear_flag(screen, LV_OBJ_FLAG_SCROLLABLE);
	lv_obj_set_pos(screen, 0, 0);
	lv_obj_add_style(screen, &styleStartScreen, 0);

	captionLabel = lv_label_create(screen);
	swVersionNameLabel = lv_label_create(screen);
	swVersionLabel = lv_label_create(screen);
	swVersionDateLabel = lv_label_create(screen);

	lv_obj_add_style(captionLabel, &styleStartScreen, 0);
	lv_obj_set_pos(captionLabel, XPOS, 50);
	lv_obj_set_size(captionLabel, LABELWIDTH, LABELHEIGTH);
	lv_label_set_text(captionLabel,"Thermostaat");

	lv_obj_set_size(swVersionNameLabel, LABELWIDTH, LABELHEIGTH);
	lv_obj_add_style(swVersionNameLabel, &styleStartScreen, 0);
	lv_obj_align_to(swVersionNameLabel, captionLabel, LV_ALIGN_TOP_RIGHT, 0, SPACING);
	lv_label_set_text (swVersionNameLabel,"Softwareversie:");

	lv_obj_set_size(swVersionLabel, LABELWIDTH, LABELHEIGTH);
	lv_obj_add_style(swVersionLabel, &styleStartScreen, 0);
	lv_obj_align_to(swVersionLabel, swVersionNameLabel, LV_ALIGN_TOP_RIGHT, 0, SPACING);
	lv_label_set_text_fmt (swVersionLabel,(char *) FIRMWARE_VERSION);

	lv_obj_set_size(swVersionDateLabel, LABELWIDTH, LABELHEIGTH);
	lv_obj_add_style(swVersionDateLabel, &styleStartScreen, 0);
	sprintf( str, "%s %s", __DATE__, __TIME__);
	lv_label_set_text_fmt(swVersionDateLabel,str);
	lv_obj_align_to(swVersionDateLabel, swVersionLabel, LV_ALIGN_TOP_RIGHT, 0, SPACING);
}

void StartScreen::show() { lv_scr_load(screen); }

StartScreen::~StartScreen() {
	// TODO Auto-generated destructor stub
}
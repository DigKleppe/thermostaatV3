/*
 * styles.h
 *
 *  Created on: Apr 4, 2022
 *      Author: dig
 */

#ifndef GUI_STYLES_H_
#define GUI_STYLES_H_
#ifdef LV_CONF_INCLUDE_SIMPLE
#include "lvgl.h"
#else
#include "lvgl/lvgl.h"
#endif
#include "fonts.h"

extern lv_style_t style_background;
extern lv_style_t styleValue; // value in meas screen
extern lv_style_t styleMeasName; // name in meas screen
extern lv_style_t styleMeasUnit; // name in meas screen
extern  lv_style_t styleNavArrows;

extern lv_style_t styleSpin;
extern lv_style_t styleSpinButtonName;
extern lv_style_t styleSpinButton;
extern lv_style_t styleClock;
extern lv_style_t styleInfo;
extern lv_style_t styleSymbol;

//  lv_palette_main(LV_PALETTE_GREY)
#define DEFAULTVALUECOLOR			lv_color_make(255, 255, 255)
#define DEFAULTBORDERCOLOR			lv_color_make(0, 255, 0)
#define DEFAULTBACKGROUNDCOLOR 		lv_color_black()

#define MEASFONT					thin75 //cantarelLight75  //dejavusansEL75
#define MEASUNITFONT				thin50 //cantarelLight50 // insloata60_4bppSub
#define MEASNAMEFONT 				cantarel25  //cantarelRegular20 // lv_font_montserrat_20
#define MEASVALUECOLOR				DEFAULTVALUECOLOR
#define MEASVALUEBGCOLOR			DEFAULTBACKGROUNDCOLOR
#define MEASNAMECOLOR				MEASVALUECOLOR

#define SPINBUTTONFONT				dejavusansEL44
#define SPINBUTTONNAMEFONT			cantarel25 //lv_font_montserrat_20
#define SPINBUTTONVALUECOLOR		DEFAULTVALUECOLOR
#define SPINBUTTONVALUEBGCOLOR		DEFAULTBACKGROUNDCOLOR
#define SPINBUTTONBACKGROUNDCOLOR	lv_palette_darken(LV_PALETTE_GREY, 3)
#define SPINBUTTONAME_BGCOLOR		DEFAULTVALUECOLOR
#define SPINBUTTONNAMECOLOR			DEFAULTVALUECOLOR
#define SPINBUTTONVALUECOLOR		DEFAULTVALUECOLOR


#define NAVIGARROWS_FONT 			dejavusansEL44  //lv_font_montserrat_44
#define NAVARROWSBGCOLOR			SPINBUTTONBACKGROUNDCOLOR
#define NAVIGARROWSCOLOR			DEFAULTVALUECOLOR


#define CLOCKFONT					thin50 // cantarelLight50// cantarel25 //lv_font_montserrat_20 //dejavusansEL44
#define	CLOCKCOLOR					DEFAULTVALUECOLOR
#define CLOCKBGCOLOR				DEFAULTBACKGROUNDCOLOR


#define INFOFONT					SPINBUTTONNAMEFONT
#define INFOCOLOR					DEFAULTVALUECOLOR
#define INFOBGCOLOR					DEFAULTBACKGROUNDCOLOR


#define SYMBOLFONT					AwsomeSymbols40
#define SYMBOLCOLOR					DEFAULTVALUECOLOR
#define SYMBOLBGCOLOR				DEFAULTBACKGROUNDCOLOR

#define STATUSLINE_FONT             insolata25


void initStyles (void);

#endif /* GUI_STYLES_H_ */

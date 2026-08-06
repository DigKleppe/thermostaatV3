/*
 * StatusLine.cpp
 *
 *  Created on: Mar 7, 2021
 *      Author: dig
 */

#include "StatusLine.h"
#include "styles.h"

//#include "lv_drv_conf.h"


LV_FONT_DECLARE(insolata25);

LV_FONT_DECLARE(lv_font_montserrat_24)

//#define STATUSLINE_FONT lv_font_montserrat_24

//#define STATUSLINE_FONT insolata25


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


static lv_style_t statusLineStyle;

StatusLine::StatusLine(lv_obj_t * parent) {

	lv_style_init(&statusLineStyle);

	lv_style_set_text_color(&statusLineStyle, lv_palette_lighten(LV_PALETTE_YELLOW, 1));
	lv_style_set_bg_color(&statusLineStyle, lv_palette_main(LV_PALETTE_LIGHT_BLUE));
//	lv_style_set_bg_color(&statusLineStyle, lv_color_black());

//	lv_style_set_radius(&statusLineStyle, LV_STATE_DEFAULT, 0);
	lv_style_set_bg_opa(&statusLineStyle, LV_OPA_COVER);
	lv_style_set_border_width(&statusLineStyle,  2);
	lv_style_set_border_color(&statusLineStyle,  lv_palette_main(LV_PALETTE_GREY));

	lv_style_set_pad_top(&statusLineStyle,  5);
	lv_style_set_pad_bottom(&statusLineStyle,  5);
	lv_style_set_pad_left(&statusLineStyle, 5);
	lv_style_set_pad_right(&statusLineStyle, 5);

//	lv_style_set_text_color(&statusLineStyle, LV_STATE_DEFAULT, LV_COLOR_WHITE);
	lv_style_set_text_letter_space(&statusLineStyle,  5);
//	lv_style_set_text_line_space(&statusLineStyle, 20);

	lv_style_set_text_font(&statusLineStyle, &STATUSLINE_FONT);

	statusLine = lv_label_create(parent);

//	lv_label_set_long_mode(statusLine,LV_LABEL_LONG_CROP);
	lv_obj_add_style(statusLine, &statusLineStyle,0);
	memset( text,'8' , MAXCHARS);
	text[MAXCHARS+1]= 0;
	lv_label_set_text(statusLine,text);

    lv_obj_set_size(statusLine, LV_HOR_RES_MAX, 40);
    lv_obj_set_pos(statusLine, 0, LV_VER_RES_MAX - 140);

//	lv_obj_set_x(statusLine, 0);
//	lv_coord_t coord =lv_obj_get_height(statusLine);
//	lv_obj_set_y(statusLine, LV_VER_RES_MAX-coord);



	//	lv_label_set_align(display, LV_LABEL_ALIGN_CENTER);       /*Center aligned lines*/

	//	lv_obj_set_size(btn1, 200, 50);

	//	lv_obj_set_width(display, 318);
	//	lv_obj_align(display, NULL, LV_ALIGN_CENTER, 0,5);

}

void StatusLine::setText(const char * str){
	if (str != NULL) {
		int len = strlen(str);
		if ( len >= MAXCHARS){
			len = MAXCHARS;
		}
		memcpy( text, str, len);
		for ( int n = len; n<MAXCHARS;n++)
			text[n] = ' ';
		lv_label_set_text(statusLine,text);
		lv_obj_clear_flag((lv_obj_t*)statusLine, LV_OBJ_FLAG_HIDDEN);
		
	}
	else
		lv_obj_add_flag((lv_obj_t*)statusLine, LV_OBJ_FLAG_HIDDEN);
		
}



StatusLine::~StatusLine() {
	// TODO Auto-generated destructor stub
}


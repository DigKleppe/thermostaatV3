/*
 * backGround.h
 *
 *  Created on: Apr 4, 2022
 *      Author: dig
 */

#ifndef COMPONENTS_GUI_BACKGROUND_H_
#define COMPONENTS_GUI_BACKGROUND_H_

#ifdef LV_CONF_INCLUDE_SIMPLE
#include "lvgl.h"
#else
#include "lvgl/lvgl.h"
#endif

#include "styles.h"


lv_obj_t *  makeBackGround ( lv_obj_t * parent);

#endif /* COMPONENTS_GUI_BACKGROUND_H_ */

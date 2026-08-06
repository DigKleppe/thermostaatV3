/*
 * NavigArrows.h
 *
 *  Created on: Mar 21, 2022
 *      Author: dig
 */

#ifndef GUI_NAVIGARROWS_H_
#define GUI_NAVIGARROWS_H_

#ifdef LV_CONF_INCLUDE_SIMPLE
#include "lvgl.h"
#else
#include "lvgl/lvgl.h"
#endif

#include "styles.h"

class NavigArrows {
public:
	NavigArrows(lv_obj_t * parent, bool right, bool left,  lv_style_t* style = &styleNavArrows );
	virtual ~NavigArrows();
	void setStyle(lv_style_t * style);

private:
	lv_obj_t * buttonRight;
	lv_obj_t * buttonLeft;
	lv_obj_t * _parent;
};

#endif /* GUI_NAVIGARROWS_H_ */

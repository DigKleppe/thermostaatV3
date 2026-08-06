/*
 * StatusIndicator.h
 *
 *  Created on: Apr 13, 2022
 *      Author: dig
 */

#ifndef GUI_STATUSINDICATOR_H_
#define GUI_STATUSINDICATOR_H_

#ifdef LV_CONF_INCLUDE_SIMPLE
#include "lvgl.h"
#else
#include "lvgl/lvgl.h"
#endif

#include "styles.h"



#define HEATERON_SIMBOL 	"\xEF\x9D\xA9"  // F769
#define COOLINGON_SYMBOL 	"\xEF\x9D\xAB"  // F76B

class StatusIndicator {
public:
	StatusIndicator(lv_obj_t * parent);
	virtual ~StatusIndicator();
	void setSymbol (const char *);
private:
	lv_obj_t * _parent;
	lv_obj_t * label;
};

#endif /* GUI_STATUSINDICATOR_H_ */

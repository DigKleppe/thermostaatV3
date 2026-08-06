/*
 * StatusLine.h
 *
 *  Created on: Mar 7, 2021
 *      Author: dig
 */

#ifndef GUI_STATUSLINE_H_
#define GUI_STATUSLINE_H_
#include "lvgl.h"
class StatusLine {
public:
	StatusLine(lv_obj_t * parent);
	virtual ~StatusLine();
	void setText(const char *);
private:
	lv_obj_t * statusLine;
	static const int MAXCHARS =  30;
	char  text[MAXCHARS+1];  // sets the width of the value1 label
};

#endif /* GUI_STATUSLINE_H_ */

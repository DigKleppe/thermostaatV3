#ifdef LV_CONF_INCLUDE_SIMPLE
#include "lvgl.h"
#else
#include "lvgl/lvgl.h"
#endif

class StartScreen { // public Stream{
public:
	StartScreen();
	virtual ~StartScreen();
	void show();
    lv_obj_t *screen;
    private:
    lv_obj_t *backGround;
    	lv_obj_t * captionLabel;
        lv_obj_t * swVersionNameLabel;
        lv_obj_t * swVersionLabel;
        lv_obj_t * swVersionDateLabel;

};

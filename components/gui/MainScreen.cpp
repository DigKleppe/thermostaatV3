/**
 * @file main
 *
 */

/*********************
 *      INCLUDES
 *********************/
#include "MainScreen.h"
#include "backGround.h"
#include "fonts.h"
#include "settings.h"
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#define PADDING 20
#define ITEMHEIGHT 95
#define ITEMWIDTH 300

// static mainScreenVars_t vars;

const char *cbText[NR_CHECKBOXES] = {"Verwarming", "Koeling"};
const bool *cbVar[NR_CHECKBOXES] = {&userSettings.heatingOn, &userSettings.coolingOn};

const SpinBoxDescr_t settingsScreenDescr[] = {{
												  .name = "Gewenste temperatuur:",
												  .format = "%2.1f",
												  .maxVal = 30.0,
												  .minVal = 10.0,
												  .step = 0.1,
												  .var = &userSettings.temperatureSetpoint,
											  },
											  {
												  .name = "Schermverlichting:",
												  .format = "%2.0f",
												  .maxVal = 80,
												  .minVal = 15,
												  .step = 5,
												  .var = &userSettings.backLight,
											  },
											  {
												  .name = NULL,
												  .format = NULL,
												  .maxVal = 100,
												  .minVal = 0,
												  .step = 0.1,
												  .var = NULL,
											  }};

// SpinBoxDescr_t spinBoxDescrTemperatuur = {
// 	.name = "Gewenste temperatuur:",
// 	.format = "%2.1f",
// 	.maxVal = 25.0,
// 	.minVal = 10.0,
// 	.step = 0.1,
// 	.var = &userSettings.temperatureSetpoint,
// };

// void MainScreen::event_handler(lv_event_t * e)
// static void event_handler(lv_event_t * e)
void MainScreen::event_handler(lv_event_t *e) {
	lv_event_code_t code = lv_event_get_code(e);
	lv_obj_t *obj = lv_event_get_target(e);
	bool *p = (bool *)obj->user_data;
	if (code == LV_EVENT_VALUE_CHANGED) {
		const char *txt = lv_checkbox_get_text(obj);
		*p = (lv_obj_get_state(obj) & LV_STATE_CHECKED);
		const char *state = lv_obj_get_state(obj) & LV_STATE_CHECKED ? "Checked" : "Unchecked";
		LV_LOG_USER("%s: %s", txt, state);
		settingsChanged = true;
	}
}

// void MainScreen::setTemperatureDisplayValue( float value) {
//	measDisplay->setValue ( value );
// }
//
// void MainScreen::setTemperatureDisplayText( char *text) {
//	measDisplay->setText (text);
// }

MainScreen::MainScreen() {
	screen = lv_obj_create(NULL);
	const SpinBoxDescr_t *descr = settingsScreenDescr;
	backGround = makeBackGround(screen);
	//	clockDisplay = new ClockDisplay(backGround);
	//	statusIndicator = new StatusIndicator ( backGround);
	//	measDisplay = new MeasDisplay(backGround, 40,"Temperatuur", "\xC2\xB0" "C", "%2.1f");

	int n = -1;
	memset(spinBox, 0, sizeof(spinBox));
	do {
		n++;
		if (descr->name != NULL)
			spinBox[n] = new SpinBox(backGround, n, descr);
		descr++;
	} while ((n < (NR_SPINBOXES - 1)) && (descr->name != NULL));

	navigArrows = new NavigArrows(backGround, true, true);

	//	spinBoxTemperatuur = new SpinBox(backGround, 1, &spinBoxDescrTemperatuur);

	for (int n = 0; n < NR_CHECKBOXES; n++) {
		cb[n] = lv_checkbox_create(screen);

		//lv_obj_set_size(cb[n], 400, 40);
		lv_checkbox_set_text(cb[n], cbText[n]);
		lv_obj_add_event_cb(cb[n], event_handler, LV_EVENT_ALL, NULL);
		lv_obj_add_style(cb[n], &styleSpinButtonName, 0);
		lv_obj_set_user_data(cb[n], (void *)cbVar[n]);

		lv_coord_t box_size = 48;
		const lv_font_t *font = lv_obj_get_style_text_font(cb[n], LV_PART_MAIN);
		lv_coord_t font_h = lv_font_get_line_height(font);
		lv_coord_t pad = (box_size - font_h) / 2;
		lv_obj_set_style_pad_left(cb[n], pad, LV_PART_INDICATOR);
		lv_obj_set_style_pad_right(cb[n], pad, LV_PART_INDICATOR);
		lv_obj_set_style_pad_top(cb[n], pad, LV_PART_INDICATOR);
		lv_obj_set_style_pad_bottom(cb[n], pad, LV_PART_INDICATOR);

		lv_obj_update_layout(cb[n]);

		lv_coord_t h = lv_obj_get_height(cb[n]);

		// lv_obj_set_pos(cb[n], 30, n * (h + PADDING) + 275);  // 
		lv_obj_set_pos(cb[n],30 +  n * 240 , 320);  // 
	}
	navigArrows = new NavigArrows(backGround, true, true);

	update();
}

// void MainScreen::setValues(mainScreenVars_t *p){
//	vars = *p;
//	update();
//
// }
//
// void MainScreen::getValues(mainScreenVars_t *p){
//	*p = vars;
// }

void MainScreen::update(void) {

	int n = 0;
	do {
		if (spinBox[n] != NULL)
			spinBox[n]->upDate();
		else
			n = NR_SPINBOXES;
		n++;
	} while (n < NR_SPINBOXES);

	if (userSettings.heatingOn)
		lv_obj_add_state(cb[CBHEATING], LV_STATE_CHECKED);
	else
		lv_obj_clear_state(cb[CBHEATING], LV_STATE_CHECKED);

	if (userSettings.coolingOn)
		lv_obj_add_state(cb[CBCOOLING], LV_STATE_CHECKED);
	else
		lv_obj_clear_state(cb[CBCOOLING], LV_STATE_CHECKED);
}

void MainScreen::show() {
	lv_scr_load(screen);
	update();
}

MainScreen::~MainScreen() {
	// TODO Auto-generated destructor stub
}

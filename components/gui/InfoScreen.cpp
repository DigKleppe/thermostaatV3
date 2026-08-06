/*
 * InfoScreen.cpp
 *
 *  Created on: Apr 15, 2022
 *      Author: dig
 */

#include "InfoScreen.h"
#include "backGround.h"

//
//
//infoDescr_t infoDesc[] = {
//		{"Netwerk:" , wifiSettings.SSID},
//		{"IPadres:",ipstr }
//};


InfoScreen::InfoScreen(const infoDescr_t * descr ) {
	int n = -1;
	lv_obj_t * backGround;

	screen = lv_obj_create(NULL);
	backGround =  makeBackGround(screen);
	NavigArrows * navigArrows = new NavigArrows(backGround, true, true);

	memset (display,0 , sizeof (display));
	do {
		n++;
		if ( descr -> name != NULL)
			display[n] = new InfoDisplay ( backGround, n, descr);
		descr++;
	}  while((n < MAXNR_INFO) && (descr->name != NULL));

}

void InfoScreen::upDate (void) {
	int n = 0;
	do {
		if (display[n] != NULL)
			display[n]->Update();
		else
			n = MAXNR_INFO;
	}  while(n++ < MAXNR_INFO);
}

void InfoScreen::show (void) {
	upDate();
	lv_scr_load(screen);
}



InfoScreen::~InfoScreen() {
	// TODO Auto-generated destructor stub
}


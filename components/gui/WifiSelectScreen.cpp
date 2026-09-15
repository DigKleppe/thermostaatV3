#include "WifiSelectScreen.h"
#include "backGround.h"
#include "esp_event.h"
#include "esp_log.h"
#include "esp_wifi.h"
#include "freertos/FreeRTOS.h"
#include "freertos/event_groups.h"
#include "freertos/task.h"
#include "guiTask.h"
#include "lvgl.h"
#include "nvs_flash.h"
#include "settings.h"
#include "styles.h"
#include "wifiConnect.h"
#include <stdio.h>
#include <string.h>

static const char *TAG = "WifiSelect";

// /* ---------- WiFi status ---------- */
// static EventGroupHandle_t s_wifi_event_group;
// #define WIFI_CONNECTED_BIT BIT0
// #define WIFI_FAIL_BIT BIT1
// static int s_retry_num = 0;

/* ---------- LVGL objects ---------- */
static lv_obj_t *dd_ssid;
static lv_obj_t *ta_password;
static lv_obj_t *kb;
static lv_obj_t *btn_connect;
static lv_obj_t *label_status;

static char ssid_options[1024]; /* buffer voor dropdown opties */

#define LABELWIDTH 320

/* ============================================================
 *  Event callbacks
 * ============================================================ */
void WifiSelectScreen::kb_event_cb(lv_event_t *e) {
	resetScreenTimer();
	wpsOff = true;
	lv_event_code_t code = lv_event_get_code(e);
	if (code == LV_EVENT_READY || code == LV_EVENT_CANCEL) {
		lv_obj_add_flag(kb, LV_OBJ_FLAG_HIDDEN);
		lv_obj_clear_flag(ta_password, LV_OBJ_FLAG_HIDDEN); /* toon textarea weer */
	}
}

void WifiSelectScreen::ta_event_cb(lv_event_t *e) {
	resetScreenTimer();
	lv_event_code_t code = lv_event_get_code(e);
	if (code == LV_EVENT_CLICKED || code == LV_EVENT_FOCUSED) {
		lv_obj_clear_flag(kb, LV_OBJ_FLAG_HIDDEN);
		lv_keyboard_set_textarea(kb, ta_password);
	}
}

void WifiSelectScreen::btn_connect_event_cb(lv_event_t *e) {
	char ssid[64] = {0};
	char password[64] = {0};

	resetScreenTimer();
	lv_obj_add_flag(kb, LV_OBJ_FLAG_HIDDEN);

	/* Lees geselecteerde SSID uit dropdown */
	lv_dropdown_get_selected_str(dd_ssid, ssid, sizeof(ssid));

	/* Lees wachtwoord uit textarea */
	const char *pwd = lv_textarea_get_text(ta_password);
	if (pwd) {
		if ((strlen(pwd) >= 8) && (strlen(ssid) > 0)) {
			strncpy(password, pwd, sizeof(password) - 1);
			strcpy(wifiSettings.SSID, ssid);
			strcpy(wifiSettings.pwd, password);
			saveSettings();
			lv_label_set_text(label_status, "Verbinden...");
			vTaskDelay(100 / portTICK_PERIOD_MS);
			restartWifi();
		}
	}
}

void WifiSelectScreen::btn_scan_event_cb(lv_event_t *e) {
	// lv_label_set_text(label_status, "Scannen...");
	// perform_wifi_scan();
	// lv_label_set_text(label_status, "Scan voltooid");
	// resetScreenTimer();
}

void WifiSelectScreen::update() {
	/* Bouw de optiestring voor de dropdown */
	int idx = 0;
	ssid_options[0] = '\0';

	for (int i = 0; i < ap_count; i++) {
		if (i > 0)
			strcat(ssid_options, "\n");
		strcat(ssid_options, (char *)ap_records[i].ssid);
		if ( strcmp ((char *) ap_records[i].ssid , (char *) wifiSettings.SSID ) == 0)	
			idx = i;

	}

	if (ap_count == 0) {
		strcpy(ssid_options, "Geen netwerken gevonden");
	}

	lv_dropdown_set_options(dd_ssid, ssid_options);
	lv_dropdown_set_selected(dd_ssid, idx);
}

/* ============================================================
 *  Bouw de LVGL UI
 * ============================================================ */
WifiSelectScreen::WifiSelectScreen() {
	lv_obj_t *scr = screen = lv_obj_create(NULL);
	backGround = makeBackGround(screen);

	/* Titel */
	lv_obj_t *title = lv_label_create(scr);
	lv_label_set_text(title, "WiFi Configuratie");
	lv_obj_align(title, LV_ALIGN_TOP_MID, 0, 10);
	lv_obj_add_style(title, &styleWifiSettings, 0);

	/* Dropdown voor SSID */
	lv_obj_t *label_ssid = lv_label_create(scr);
	lv_label_set_text(label_ssid, "Netwerk:");
	lv_obj_align(label_ssid, LV_ALIGN_TOP_LEFT, 10, 50);
	lv_obj_add_style(label_ssid, &styleWifiSettings, 0);

	dd_ssid = lv_dropdown_create(scr);
	lv_dropdown_set_options(dd_ssid, "Scan om netwerken te vinden");
	lv_obj_set_width(dd_ssid, LABELWIDTH);
	lv_obj_align(dd_ssid, LV_ALIGN_TOP_LEFT, 10, 70);
	lv_obj_add_style(dd_ssid, &styleWifiSettings, 0);

	// /* Scan-knop */
	// lv_obj_t *btn_scan = lv_btn_create(scr);
	// lv_obj_align(btn_scan, LV_ALIGN_TOP_RIGHT, -10, 65);
	// lv_obj_t *btn_scan_label = lv_label_create(btn_scan);
	// lv_obj_add_style(btn_scan_label, &styleWifiSettings, 0);
	// lv_label_set_text(btn_scan_label, "Scan");
	// lv_obj_add_event_cb(btn_scan, btn_scan_event_cb, LV_EVENT_CLICKED, NULL);

	/* Wachtwoord label */
	lv_obj_t *label_pwd = lv_label_create(scr);
	lv_label_set_text(label_pwd, "Wachtwoord:");
	lv_obj_add_style(label_pwd, &styleWifiSettings, 0);
	lv_obj_align(label_pwd, LV_ALIGN_TOP_LEFT, 10, 130);

	/* Wachtwoord textarea */
	ta_password = lv_textarea_create(scr);
	lv_obj_add_style(ta_password, &styleWifiSettings, 0);
	//	lv_textarea_set_password_mode(ta_password, true);
	lv_textarea_set_one_line(ta_password, true);
	lv_obj_set_width(ta_password, LABELWIDTH);
	lv_obj_align(ta_password, LV_ALIGN_TOP_LEFT, 10, 150);
	lv_obj_add_event_cb(ta_password, ta_event_cb, LV_EVENT_ALL, NULL);

	/* Verbind-knop */
	btn_connect = lv_btn_create(scr);
	lv_obj_align(btn_connect, LV_ALIGN_TOP_RIGHT, 0, 150);
	lv_obj_t *btn_connect_label = lv_label_create(btn_connect);
	lv_obj_add_style(btn_connect_label, &styleWifiSettings, 0);
	lv_label_set_text(btn_connect_label, "Verbinden");
	lv_obj_add_event_cb(btn_connect, btn_connect_event_cb, LV_EVENT_CLICKED, NULL);

	/* Status label */
	label_status = lv_label_create(scr);
	lv_obj_add_style(label_status, &styleWifiSettings, 0);
	lv_label_set_text(label_status, "Kies netwerk");
	lv_obj_align(label_status, LV_ALIGN_BOTTOM_MID, 0, -10);

	/* Virtueel toetsenbord (standaard verborgen) */
	kb = lv_keyboard_create(scr);
	lv_obj_set_size(kb, LV_PCT(100), LV_PCT(50));
	lv_obj_align(kb, LV_ALIGN_BOTTOM_MID, 0, 0);
	lv_obj_add_flag(kb, LV_OBJ_FLAG_HIDDEN);
	lv_obj_add_event_cb(kb, kb_event_cb, LV_EVENT_ALL, NULL);

	navigArrows = new NavigArrows(backGround, true, true);
}
void WifiSelectScreen::show() {

	lv_scr_load(screen);
	update();
}
WifiSelectScreen::~WifiSelectScreen() {
	// TODO Auto-generated destructor stub
}
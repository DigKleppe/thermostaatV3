/*
 * settings.c
 *
 *  Created on: Nov 30, 2017
 *      Author: dig
 */
#include "wifiConnect.h"
#include "settings.h"
#include "nvs_flash.h"
#include "nvs.h"
#include "driver/gpio.h"
#include "esp_err.h"
#include "esp_log.h"
#include <string.h>
#include <cerrno>

#define TAG "settings"


#define STORAGE_NAMESPACE "storage"

extern settingsDescr_t settingsDescr[];
extern int myRssi;
bool settingsChanged;

char checkstr[MAX_STRLEN+1];


advancedSettings_t advancedSettingsDefaults = {CONFIG_FIXED_LAST_IP_DIGIT, {ADVUSERSETTINGS_CHECKSTR}};
systemInfo_t systemInfoDefaults = { 0,0,0,0,0,0,{SYSTEMINFO_CHECKSTR }};


const userSettings_t userSettingsDefaults = {
	{"Thermostaat"},
	19.0, 	//temperatureSetpoint
	2,  	//temperatureOffset
	0.0,	//RHoffset
	5.0,	//PIDp
	0.2, 	//PIDi
	40, 	//PIDmaxi
	15,		// controlInterval
	true,	//heatingOn
	true,   //coolingOn
	false, // isCalibrated
	15.0,		//backLight
	USERSETTINGS_CHECKSTR };


systemInfo_t systemInfo;
userSettings_t userSettings;
advancedSettings_t advSettings;

esp_err_t saveSettings(void) {
	nvs_handle_t my_handle;
	esp_err_t err = 0;

	err = nvs_open(STORAGE_NAMESPACE, NVS_READWRITE, &my_handle);
	if (err != ESP_OK) {
		ESP_LOGE(TAG, "Error (%s) opening NVS handle!", esp_err_to_name(err));
	} else {
		err = nvs_set_blob(my_handle, "WifiSettings", (const void *)&wifiSettings, sizeof(wifiSettings_t));
		err |= nvs_set_blob(my_handle, "userSettings", (const void *)&userSettings, sizeof(userSettings_t));
		err |= nvs_set_blob(my_handle, "advSettings", (const void *)&advSettings, sizeof(advancedSettings_t));
		err |= nvs_set_blob(my_handle, "systemInfo", (const void *)&systemInfo, sizeof(systemInfo_t));
		switch (err) {
		case ESP_OK:
			ESP_LOGI(TAG, "settings written");
			break;
		default:
			ESP_LOGE(TAG, "Error (%s) writing!", esp_err_to_name(err));
		}
		err = nvs_commit(my_handle);

		switch (err) {
		case ESP_OK:
			ESP_LOGI(TAG, "Committed");
			break;
		default:
			ESP_LOGE(TAG, "Error (%s) commit", esp_err_to_name(err));
		}
		nvs_close(my_handle);
	}
	return err;
}

esp_err_t loadSettings() {
	nvs_handle_t my_handle;
	esp_err_t err = 0;
	bool doSave = false;

	err = nvs_open(STORAGE_NAMESPACE, NVS_READONLY, &my_handle);
	size_t len;
	if (err != ESP_OK) {
		ESP_LOGE(TAG, "Error (%s) opening NVS handle!", esp_err_to_name(err));
	} else {
		ESP_LOGI(TAG, "reading SSID and password");
		len = sizeof(wifiSettings_t);
		err = nvs_get_blob(my_handle, "WifiSettings", (void *)&wifiSettings, &len);
		len = sizeof(userSettings_t);
		err |= nvs_get_blob(my_handle, "userSettings", (void *)&userSettings, &len);
		len = sizeof(advancedSettings_t);
		err |= nvs_get_blob(my_handle, "advSettings",(void *) &advSettings, &len);
		len = sizeof(systemInfo_t);
		err |= nvs_get_blob(my_handle, "systemInfo",(void *) &systemInfo, &len);
	
		switch (err) {
		case ESP_OK:
			ESP_LOGI(TAG, "SSID: %s", wifiSettings.SSID);
			break;
		case ESP_ERR_NVS_NOT_FOUND:
			ESP_LOGE(TAG, "The value is not initialized yet!");
			break;
		default:
			ESP_LOGE(TAG, "Error (%s) reading!", esp_err_to_name(err));
		}
		nvs_close(my_handle);
	}

	if (strncmp(userSettings.checkstr, USERSETTINGS_CHECKSTR, strlen(USERSETTINGS_CHECKSTR)) != 0) {
		userSettings = userSettingsDefaults;
		wifiSettings = wifiSettingsDefaults;
		ESP_LOGE(TAG, "default usersettings loaded");
		doSave = true;
	}

	if (strncmp(advSettings.checkstr, ADVUSERSETTINGS_CHECKSTR, strlen(ADVUSERSETTINGS_CHECKSTR)) != 0) {
		advSettings = advancedSettingsDefaults;
		ESP_LOGE(TAG, "default advanced settings loaded");
		doSave = true;
	}

	if (strncmp(systemInfo.checkstr, SYSTEMINFO_CHECKSTR, strlen(SYSTEMINFO_CHECKSTR)) != 0) {
		systemInfo =  systemInfoDefaults;
		ESP_LOGE(TAG, "default systemInfo loaded");
		doSave = true;
	}

	if (doSave)
		return saveSettings();
	else {
		ESP_LOGI(TAG, "usersettings loaded");
	}
	return err;
}


void setUserDefaults(void) {
	userSettings = userSettingsDefaults;
	ESP_LOGI(TAG, "user settings defaults");
	saveSettings();
}

void setAdvDefaults(void) {
	advSettings = advancedSettingsDefaults;
	ESP_LOGI(TAG, "advanced settings defaults");
	saveSettings();
}
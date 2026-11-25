/*
 * settings.c
 *
 *  Created on: Nov 30, 2017
 *      Author: dig
 */
#include "settings.h"
#include "driver/gpio.h"
#include "esp_err.h"
#include "esp_log.h"
#include "nvs.h"
#include "nvs_flash.h"
#include "wifiConnect.h"
#include <cerrno>
#include <string.h>

#define TAG "settings"

#define STORAGE_NAMESPACE "storage"

extern settingsDescr_t settingsDescr[];
extern int myRssi;
bool settingsChanged;

char checkstr[MAX_STRLEN + 1];

advancedSettings_t advancedSettingsDefaults = {CONFIG_FIXED_LAST_IP_DIGIT, {ADVUSERSETTINGS_CHECKSTR}};
systemInfo_t systemInfoDefaults = {0, 0, 0, 0, 0, 0, {SYSTEMINFO_CHECKSTR}};

const userSettings_t userSettingsDefaults = {{"Thermostaat"},
											 21.0, // temperatureSetpoint
											 4.4,	// temperatureOffset
											 0.0,	// RHoffset
											 100,	// PIDp
											 10,	// PIDi
											 30,	// PIDd
											 40,	// PIDmaxi
											 30, 	// PID dtime	
											 15,	// controlInterval
											 true,	// heatingOn
											 true,	// coolingOn
											 false, // isCalibrated
											 50,	// backLight
											 USERSETTINGS_CHECKSTR};

systemInfo_t systemInfo;
userSettings_t userSettings;
oldUserSettings_t oldUserSettings;
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
		len = sizeof(userSettings_t);
		err = nvs_get_blob(my_handle, "userSettings", (void *)&userSettings, &len);
		if (err == ESP_OK) {
			if (strncmp(userSettings.checkstr, USERSETTINGS_CHECKSTR, strlen(USERSETTINGS_CHECKSTR)) !=
				0) { // if usersettings fails try old userSettings
				len = sizeof(oldUserSettings_t);
				err = nvs_get_blob(my_handle, "userSettings", (void *)&oldUserSettings, &len);
				if (err == ESP_OK) {
					if (strncmp(oldUserSettings.checkstr, OLDUSERSETTINGS_CHECKSTR, strlen(OLDUSERSETTINGS_CHECKSTR)) != 0)
						err = ESP_FAIL;
					else { // copy old usersettings to new usersettings
						ESP_LOGI(TAG, "New settings loaded from old settings");
						userSettings.backLight = oldUserSettings.backLight;
						userSettings.controlInterval = oldUserSettings.controlInterval;
						userSettings.coolingOn = oldUserSettings.coolingOn;
						userSettings.heatingOn = oldUserSettings.heatingOn;
						userSettings.isCalibrated = oldUserSettings.isCalibrated;
						strcpy(userSettings.moduleName, oldUserSettings.moduleName);
						userSettings.PIDd = userSettingsDefaults.PIDd;
						userSettings.PIDi = oldUserSettings.PIDi;
						userSettings.PIDi = oldUserSettings.PIDi;
						userSettings.PIDmaxi = oldUserSettings.PIDmaxi;
						userSettings.RHoffset = oldUserSettings.RHoffset;
						userSettings.temperatureOffset = oldUserSettings.temperatureOffset;
						userSettings.temperatureSetpoint = oldUserSettings.temperatureSetpoint;
						strcpy(userSettings.checkstr, USERSETTINGS_CHECKSTR);
						doSave = true;
					}
				}
			}
		}
		ESP_LOGI(TAG, "reading settings");
		len = sizeof(wifiSettings_t);
		err |= nvs_get_blob(my_handle, "WifiSettings", (void *)&wifiSettings, &len);
		len = sizeof(advancedSettings_t);
		err |= nvs_get_blob(my_handle, "advSettings", (void *)&advSettings, &len);
		len = sizeof(systemInfo_t);
		err |= nvs_get_blob(my_handle, "systemInfo", (void *)&systemInfo, &len);

		switch (err) {
		case ESP_OK:
			ESP_LOGI(TAG, "SSID: %s PWD: %s", wifiSettings.SSID, wifiSettings.pwd);
			break;
		case ESP_ERR_NVS_NOT_FOUND:
			ESP_LOGE(TAG, "The value is not initialized yet!");
			break;
		default:
			ESP_LOGE(TAG, "Error (%s) reading!", esp_err_to_name(err));
		}
	}
	nvs_close(my_handle);

	if (err) {
		userSettings = userSettingsDefaults;
		wifiSettings = wifiSettingsDefaults;
		advSettings = advancedSettingsDefaults;
		ESP_LOGE(TAG, "defaults loaded");
		doSave = true;
	} else {
		if (strncmp(advSettings.checkstr, ADVUSERSETTINGS_CHECKSTR, strlen(ADVUSERSETTINGS_CHECKSTR)) != 0) {
			advSettings = advancedSettingsDefaults;
			ESP_LOGE(TAG, "default advanced settings loaded");
			doSave = true;
		}

		if (strncmp(systemInfo.checkstr, SYSTEMINFO_CHECKSTR, strlen(SYSTEMINFO_CHECKSTR)) != 0) {
			systemInfo = systemInfoDefaults;
			ESP_LOGE(TAG, "default systemInfo loaded");
			doSave = true;
		}
	}
	if (doSave)
		return saveSettings();
	else {
		ESP_LOGI(TAG, "All settings loaded");
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
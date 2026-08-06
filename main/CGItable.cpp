/*
 * CGItable.cpp
 *
 *  Created on: Aug 5, 2024
 *      Author: dig
 */

#include "CGItable.h"
#include "CGIcommonScripts.h"
#include "log.h"
#include "sensirionTask.h"
#include <cstddef>

// @formatter:off
// do not alter
const tCGI CGIurls[] = {
	{"/cgi-bin/getSettings", (tCGIHandler_t)readCGIvalues, (CGIresponseFileHandler_t)getSettingsScript},
	{"/cgi-bin/getAdvSettings", (tCGIHandler_t)readCGIvalues, (CGIresponseFileHandler_t)getAdvSettingsScript},
	{"/cgi-bin/getLogMeasValues", (tCGIHandler_t)readCGIvalues, (CGIresponseFileHandler_t)getAllLogsScript},
	{"/cgi-bin/getRTMeasValues", (tCGIHandler_t)readCGIvalues, (CGIresponseFileHandler_t)getRTMeasValuesScript},
	{"/cgi-bin/getInfoValues", (tCGIHandler_t)readCGIvalues, (CGIresponseFileHandler_t)getInfoValuesScript},
	{"/cgi-bin/getCalValues", (tCGIHandler_t)readCGIvalues, (CGIresponseFileHandler_t)getCalValuesScript},
	{"/cgi-bin/getSensorName", (tCGIHandler_t)readCGIvalues, (CGIresponseFileHandler_t)getSensorNameScript},
	{"/cgi-bin/clearLog", (tCGIHandler_t)readCGIvalues, (CGIresponseFileHandler_t)clearLogScript},
	{"/cgi-bin/forgetWifi", (tCGIHandler_t)readCGIvalues, (CGIresponseFileHandler_t)forgetWifiScript},
	// { "/cgi-bin/saveSettings", (tCGIHandler_t) readCGIvalues, (CGIresponseFileHandler_t) saveSettingsScript},
	// { "/cgi-bin/cancelSettings", (tCGIHandler_t) readCGIvalues, (CGIresponseFileHandler_t) cancelSettingsScript},
	{NULL, NULL, NULL} // last
};

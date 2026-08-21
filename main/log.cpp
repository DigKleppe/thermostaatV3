/*
 * log.cpp
 *
 *  Created on: Nov 3, 2023
 *      Author: dig
 */
#include "log.h"
#include <math.h>
#include <stdio.h>
#include <string.h>
#include <esp_heap_caps.h>

int printLog(log_t *logToPrint, char *pBuffer);
extern int scriptState;

int logRxIdx;
int logTxIdx;
// static log_t measLog[ MAXLOGVALUES];

static log_t *measLog = NULL;

log_t *initLog(void) {
	measLog = (log_t *)heap_caps_malloc(MAXLOGVALUES * sizeof(log_t), MALLOC_CAP_SPIRAM);
	memset(measLog, 0, MAXLOGVALUES * sizeof(log_t));
	return measLog;
}

void addToLog(log_t logValue) {
	if (measLog) {
		measLog[logTxIdx] = logValue;
		measLog[logTxIdx].timeStamp = timeStamp;
		logTxIdx++;
		if (logTxIdx >= MAXLOGVALUES)
			logTxIdx = 0;
	}
}

// reads all available data from log
// issued as first request.

int getAllLogsScript(char *pBuffer, int count) {
	static time_t oldTimeStamp = 0;
	static int logsToSend = 0;
	int left, len = 0;
	if (measLog) {
		int n;
		if (scriptState == 0) { // find oldest value in cyclic logbuffer
			logRxIdx = 0;
			if (measLog[logRxIdx].timeStamp == 0)
				return 0; // empty

			oldTimeStamp = 0;
			for (n = 0; n < MAXLOGVALUES; n++) {
				if (measLog[logRxIdx].timeStamp < oldTimeStamp)
					break;
				else {
					oldTimeStamp = measLog[logRxIdx++].timeStamp;
				}
			}
			if (measLog[logRxIdx].timeStamp == 0) { // then log not full
				// not written yet?
				logRxIdx = 0;
				logsToSend = n;
			} else
				logsToSend = MAXLOGVALUES;
			scriptState++;
		}
		if (scriptState == 1) { // send complete buffer
			if (logsToSend) {
				len = 0;
				do {
					len += printLog(&measLog[logRxIdx], pBuffer + len);
					logRxIdx++;
					if (logRxIdx >= MAXLOGVALUES)
						logRxIdx = 0;
					left = count - len;
					logsToSend--;

				} while (logsToSend && (left > 40));
			}
		}
	}
	return len;
}

// these functions only work for one user!

int getNewLogsScript(char *pBuffer, int count) {
	int left, len = 0;
	if (measLog) {
		if (logRxIdx != (logTxIdx)) { // something to send?
			do {
				len += printLog(&measLog[logRxIdx], pBuffer + len);
				logRxIdx++;
				if (logRxIdx >= MAXLOGVALUES)
					logRxIdx = 0;
				left = count - len;

			} while ((logRxIdx != logTxIdx) && (left > 40));
		}
	}
	return len;
}

int clearLogScript(char *pBuffer, int count) {
	if (measLog) {
		if (scriptState == 0) {
			logRxIdx = 0;
			logTxIdx = 0;
			memset(measLog, 0, MAXLOGVALUES * sizeof(log_t));
			strcpy(pBuffer, "OK");
			scriptState++;
			return 3;
		}
	}
	return 0;
}

// void testLog( void) {
// 	log_t testlog;
// 	testlog.unit[0]= 'V';
// 	testlog.unit[1]= 0;
// 	for ( int n = 0; n < MAXLOGVALUES; n++) {
// 		testlog.measValue = sin ((float)n / 100);
// 		timeStamp++;
// 		addToLog(testlog);
// 	}
// }

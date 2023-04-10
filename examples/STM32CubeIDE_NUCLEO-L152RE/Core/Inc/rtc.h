/*
 * rtc.h
 *
 *  Created on: Apr 9, 2023
 *      Author: Martin
 */

#ifndef INC_RTC_H_
#define INC_RTC_H_

#include "stm32l1xx_hal.h"
#include "settings.h"

#define minutesinhour 60
#define hoursinday 24
#define minutesinday minutesinhour*hoursinday
#define daysinweek 7
#define alldays 8
#define noPendingAlarm -1

extern RTC_HandleTypeDef hrtc;

extern RTC_TimeTypeDef timeRtc;
extern RTC_DateTypeDef dateRtc;
extern RTC_AlarmTypeDef alarmRtc;

typedef struct {
	unsigned int skipAlarmCnt;
	unsigned int maxskipAlarmCnt;
	bool alarmFlag;
	unsigned int nextAlarmIndex;
} alarmState_t;

extern alarmState_t alarmState;

HAL_StatusTypeDef Rtc_GetDateTime();
HAL_StatusTypeDef Rtc_SetDate();
HAL_StatusTypeDef Rtc_SetTime();
HAL_StatusTypeDef Rtc_GetAlarm();
HAL_StatusTypeDef Rtc_SetAlarm();
void Find_NextAlarm();

#endif /* INC_RTC_H_ */

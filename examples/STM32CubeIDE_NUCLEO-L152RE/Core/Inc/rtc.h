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

//the following three values have to be held consistend manually!
#define skipAlarmStepping 3		//2^skipAlarmStepping number of steps required to change skipAlarmCnt, should multiplied by maxAlarm not exceed unsigned char
#define skipAlarmhalfStep 4		// = (2^(skipAlarmStepping-1)   savety margin to alow for backlight, but avoid alarm change
#define skipAlarmMask 0xF8		// = (0x100-(2^skipAlarmStepping))

#define minutesinhour 60
#define hoursinday 24
#define minutesinday minutesinhour*hoursinday
#define daysinweek 7
#define alldays 8

extern RTC_HandleTypeDef hrtc;

extern RTC_TimeTypeDef sTimeRtc;
extern RTC_DateTypeDef sDateRtc;

typedef struct {
	unsigned int skipAlarmCnt;
	unsigned int maxskipAlarmCnt;
	bool alarmFlag;
} alarmState_t;

extern alarmState_t alarmState;

HAL_StatusTypeDef Rtc_GetDateTime();
HAL_StatusTypeDef Rtc_SetDate();
HAL_StatusTypeDef Rtc_SetTime();

unsigned int Find_NextAlarm();

#endif /* INC_RTC_H_ */

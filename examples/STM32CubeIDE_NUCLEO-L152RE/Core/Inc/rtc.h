/*
 * rtc.h
 *
 *  Created on: Apr 9, 2023
 *      Author: Martin
 */

#ifndef INC_RTC_H_
#define INC_RTC_H_

#include "stm32l1xx_hal.h"

extern RTC_HandleTypeDef hrtc;

extern RTC_TimeTypeDef sTimeRtc;
extern RTC_DateTypeDef sDateRtc;

void Rtc_GetDateTime();
void Rtc_SetDate();
void Rtc_SetTime();

#endif /* INC_RTC_H_ */

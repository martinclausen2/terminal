#include "rtc.h"

RTC_TimeTypeDef sTimeRtc = {0};
RTC_DateTypeDef sDateRtc = {0};


void Rtc_GetDateTime()
{
	HAL_RTC_GetTime(&hrtc, &sTimeRtc, RTC_FORMAT_BIN);
	HAL_RTC_GetDate(&hrtc, &sDateRtc, RTC_FORMAT_BIN);
}

void Rtc_SetDate()
{
	HAL_RTC_SetDate(&hrtc, &sDateRtc, RTC_FORMAT_BIN);
}

void Rtc_SetTime()
{
	HAL_RTC_SetTime(&hrtc, &sTimeRtc, RTC_FORMAT_BIN);
}

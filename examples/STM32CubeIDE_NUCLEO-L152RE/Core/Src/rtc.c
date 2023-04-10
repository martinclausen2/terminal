#include "rtc.h"

RTC_TimeTypeDef sTimeRtc = {0};
RTC_DateTypeDef sDateRtc = {0};

alarmState_t alarmState = {0};

HAL_StatusTypeDef Rtc_GetDateTime()
{
	return HAL_ERROR & (HAL_RTC_GetTime(&hrtc, &sTimeRtc, RTC_FORMAT_BIN) | HAL_RTC_GetDate(&hrtc, &sDateRtc, RTC_FORMAT_BIN));
}

HAL_StatusTypeDef Rtc_SetDate()
{
	return HAL_RTC_SetDate(&hrtc, &sDateRtc, RTC_FORMAT_BIN);
}

HAL_StatusTypeDef Rtc_SetTime()
{
	return HAL_RTC_SetTime(&hrtc, &sTimeRtc, RTC_FORMAT_BIN);
}

//Checks for the next alarm, returns 0 if no alarm was found
//considers skipAlarmCnt, not fast, but memory effective
unsigned int Find_NextAlarm()
{
	unsigned int j;
	unsigned int i;
	signed int curdifference;		//time between alarm and curent time
	signed int mindifference=0;		//maximum difference is 7*24*60 Minutes = 10080 Minutes
									//0 is no alarm on yet found
	signed int curmindifference=0;	//minimum for this skipped alarm iteration, goes up with every skip iteration

	unsigned int minAlarm=0;		//number of the alarm with the smallest difference

	Rtc_GetDateTime();

	for(i=0; i<=(alarmState.skipAlarmCnt>>skipAlarmStepping); i++)
		{
		alarmState.maxskipAlarmCnt=0;
		for(j=0; j<maxAlarm+1; j++)		//go through all alarms
			{
			alarm_t curAlarm = GLOBAL_settings_ptr->Alarm[j];
			if (curAlarm.weekday)			//is this alarm on at all?
				{
				//calculate time difference between now and alarm
				if (alldays==curAlarm.weekday)
					{
					curdifference = 0;
					}
				else
					{
					curdifference = (curAlarm.weekday-sDateRtc.WeekDay)*minutesinday;
					}
				curdifference += (curAlarm.hour-sTimeRtc.Hours)*minutesinhour;
				curdifference += curAlarm.minute-sTimeRtc.Minutes;
				if (0>curdifference)		//warp into next week
					{
					curdifference+=daysinweek*minutesinday;
					}
				alarmState.maxskipAlarmCnt++;

				// set alarm no if alarm is closer or no alarm is set yet
				if (((mindifference>curdifference) || 0==mindifference) && curmindifference<curdifference)
					{
					mindifference=curdifference;
					minAlarm=j+1;		//shift result by one => no alarm = 0
					}
				}
			}
		curmindifference=mindifference;			//save this iterations smallest time difference
		mindifference=0;						//reset search
		}
	alarmState.maxskipAlarmCnt=(alarmState.maxskipAlarmCnt<<skipAlarmStepping)+skipAlarmhalfStep;		//do not allow to skip more alarms than active with in one week
	return minAlarm;
}

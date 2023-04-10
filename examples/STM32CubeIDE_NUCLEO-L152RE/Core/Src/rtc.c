#include "rtc.h"

RTC_TimeTypeDef timeRtc = {0};
RTC_DateTypeDef dateRtc = {0};
RTC_AlarmTypeDef alarmRtc = {.Alarm = RTC_ALARM_A,
								.AlarmDateWeekDaySel = RTC_ALARMDATEWEEKDAYSEL_WEEKDAY};

alarmState_t alarmState = {0};

HAL_StatusTypeDef Rtc_GetDateTime()
{
	return HAL_ERROR & (HAL_RTC_GetTime(&hrtc, &timeRtc, RTC_FORMAT_BIN)
			| HAL_RTC_GetDate(&hrtc, &dateRtc, RTC_FORMAT_BIN));
}

HAL_StatusTypeDef Rtc_SetDate()
{
	return HAL_ERROR & (HAL_RTC_SetDate(&hrtc, &dateRtc, RTC_FORMAT_BIN)
			| Rtc_SetAlarm());
}

HAL_StatusTypeDef Rtc_SetTime()
{
	return HAL_ERROR & (HAL_RTC_SetTime(&hrtc, &timeRtc, RTC_FORMAT_BIN)
			| Rtc_SetAlarm());
}

HAL_StatusTypeDef Rtc_GetAlarm()
{
	return HAL_RTC_GetAlarm(&hrtc, &alarmRtc, RTC_ALARM_A, RTC_FORMAT_BIN);
}

HAL_StatusTypeDef Rtc_SetAlarm()
{
	Find_NextAlarm();
	HAL_StatusTypeDef status = HAL_RTC_DeactivateAlarm(&hrtc, alarmRtc.Alarm);
	if (noPendingAlarm != alarmState.nextAlarmIndex)
	{
		alarmRtc.AlarmDateWeekDay = GLOBAL_settings_ptr->Alarm[alarmState.nextAlarmIndex].weekday;
		alarmRtc.AlarmTime.Hours = GLOBAL_settings_ptr->Alarm[alarmState.nextAlarmIndex].hour;
		alarmRtc.AlarmTime.Minutes = GLOBAL_settings_ptr->Alarm[alarmState.nextAlarmIndex].minute;
		status |= HAL_RTC_SetAlarm_IT(&hrtc, &alarmRtc, RTC_FORMAT_BIN);
	}
	return HAL_ERROR & status;
}

/*

//stop count down to acustic alarm
void AlarmEnd()
{
	Minutes2Signal=0;
	RefreshTime=1;
}

//menu to stop count down to acustic alarm
void AlarmSnoozeEnd()
{
	unsigned long i;
	unsigned char j = 0;			//set to "snooze" = 0

	for(i=menutimeout; i; i--)
		{
		if (TimerFlag)
			{
			PWM_StepDim();		// do next dimming step to permit LCD backlight fadein to finish
			TimerFlag=0;
			}

		if((!CheckKeyPressed()) || ((12==rCounter) && (RC5Addr==rAddress || RC5Addr_front==rAddress || RC5Addr_back==rAddress || RC5Addr_com==rAddress)))
			{
			break;			//exit on key pressed or RC5 command received
			}
		else if (EncoderSetupValue(&j, maxAlarmEndMode,0))
			{
			i=menutimeout;		//reset timeout due to user action
			}
		PCON=MCUIdle;			//go idel, wake up by any int
		}
	// 0==j, nothing to do: timeout, RC5 command received or user keeps snooze
	// 0==i, timeout
	if (0!=j && 0!=i)			//end alarm
		{
		AlarmEnd();
		if (2==j || 4==j)
			{			//send end alarm via RC5, send anyway since use selected to do so
			SendRC5(RC5Addr_com, RC5Cmd_AlarmEnd, 1, ComModeOff, RC5Cmd_Repeats);
			}
		if (4==j)
			{			//send standby via RC5, send anyway since use selected to do so
			CommandPause();		//wait after sending AlarmEnd required
			SendRC5(RC5Addr_com, RC5Cmd_Off, 1, ComModeOff, RC5Cmd_Repeats);
			}
		if (3<=j)
			{			//goto standby
			SwAllLightOff();
			}
		if (3>j)
			{
			LCD_SendBrightness(FocusBacklight+1);
			}
		}
	RefreshTime=1;
}

//wake-up light dimming
void Alarm_StepDim(unsigned char i)
{
	if (AlarmDim_Cnt[i])
		{
		--AlarmDim_Cnt[i];				//count down step
		}
	else							//dimming step
		{
		if (Brightness[i+1] < Read_EEPROM(EEAddr_AlarmFrontBrightness+i))
			{
			AlarmDim_Cnt[i]=AlarmDim_Cnt_Reload[i];	//reload countdown
			PWM_SetupDim(i+1, Brightness_steps, 1);	//setup brightness
			}
		else
			{
			Alarmflag=0;				//we reached targetbrightness!
			}
		}
}

//wake-up light active
void Alarm_StepDim_all()
{
	if (Alarmflag)
		{
		Alarm_StepDim(0);
		Alarm_StepDim(1);
		}
}

//prepare wake-up light
void SetupAlarmDim(unsigned char i)
{
	AlarmDim_Cnt_Reload[i]=(Read_EEPROM(EEAddr_LightFading)*RTCIntfrequ*60)/Read_EEPROM(EEAddr_AlarmFrontBrightness+i);
	AlarmDim_Cnt[i]=AlarmDim_Cnt_Reload[i];
}


//execute an alarm
void Alarm()
{
	if (0==Alarmflag)
		{
		SendRC5(RC5Addr_com, RC5Cmd_AlarmStart, 1, ComModeAlarm, RC5Cmd_Repeats);
		SetupAlarmDim(0);		// fade in to required brightness in on or off,
					// no fade in if already brighter, Alarm_StepDim() takes care of this behavior
		SetupAlarmDim(1);
		LightOn=1;
		Alarmflag=1;
		Minutes2Signal=Read_EEPROM(EEAddr_AlarmTime2Signal);
		}
}

*/


//Checks for the next alarm, returns -1 if no alarm was found
//considers skipAlarmCnt, not fast, but memory effective
void Find_NextAlarm()
{
	unsigned int j;
	unsigned int i;
	signed int curdifference;		//time between alarm and curent time
	signed int mindifference=0;		//maximum difference is 7*24*60 Minutes = 10080 Minutes
									//0 is no alarm on yet found
	signed int curmindifference=0;	//minimum for this skipped alarm iteration, goes up with every skip iteration

	int minAlarm = noPendingAlarm;	//number of the alarm with the smallest difference

	Rtc_GetDateTime();

	for(i=0; i<=alarmState.skipAlarmCnt; i++)
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
					curdifference = (curAlarm.weekday-dateRtc.WeekDay)*minutesinday;
					}
				curdifference += (curAlarm.hour-timeRtc.Hours)*minutesinhour;
				curdifference += curAlarm.minute-timeRtc.Minutes;
				if (0>curdifference)		//warp into next week
					{
					curdifference+=daysinweek*minutesinday;
					}
				alarmState.maxskipAlarmCnt++;

				// set alarm no if alarm is closer or no alarm is set yet
				if (((mindifference>curdifference) || 0==mindifference) && curmindifference<curdifference)
					{
					mindifference=curdifference;
					minAlarm=j;
					}
				}
			}
		curmindifference=mindifference;			//save this iterations smallest time difference
		mindifference=0;						//reset search
		}
	alarmState.nextAlarmIndex = minAlarm;
}

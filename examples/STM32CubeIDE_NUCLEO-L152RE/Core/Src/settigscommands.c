#include <settingscommands.h>

//TODO command for setting & reading current brightness, reading ext brightness

void SettingsCommands_Init()
{
	CLI_AddCmd("bright", BrightnessCmd, 1, TMC_None, "set brightness values - [type] <-c channel_no> <-b brightness_value>");
	CLI_AddCmd("getextbright", GetExtBrightCmd, 0, TMC_PrintStartTime | TMC_PrintStopTime, "getextbright <-d duration>");
	CLI_AddCmd("remote", RemoteControlCmd, 0, TMC_None, "set infrared remote parameters - <-a address> <-r receiver mode> <-s sender mode>");
	CLI_AddCmd("time", SetTimeCmd, 3, TMC_None, "set time of RTC - [hour] [minute] [second]");
	CLI_AddCmd("date", SetDateCmd, 4, TMC_None, "set date of RTC - [2 digit year] [month] [day] [w] with w weekday");
	CLI_AddCmd("timestamp", GetTimestampCmd, 0, TMC_None, "get date and time from RTC");
	CLI_AddCmd("alarmschedule", AlarmScheduleCmd, 1, TMC_None, "set alarm schedule - [alarm no] <-w weekday> <-h hour> <-m minute>");
	CLI_AddCmd("alarmsetting", AlarmSettingsCmd, 0, TMC_None, "set alarm parameters - <-f time to fade-in light> <-si time to signal> <-sn snooze time>");
	CLI_AddCmd("alarm", AlarmCmd, 0, TMC_None, "trigger, reset, set alarm skip count - <-a 1 | 0> <-s alarm skip count>");
	CLI_AddCmd("beep", 	SetBeepVolumeCmd, 0, TMC_None, "set beep volume - <-v volume>");
	CLI_AddCmd("reset", ResetSettingsCmd, 0, TMC_None, "reset settings to factory defaults");
}

void printValueArray(unsigned char(*values)[maxChannel])
{
	int i;
	for (i = 0; i < maxChannel;	i++)
	{
		CLI_Printf("\r\nchannel: %d value: %d", i, (int)*(*values+i));
	};
}

void printIntValueArray(unsigned int(*values)[maxChannel])
{
	int i;
	for (i = 0; i < maxChannel;	i++)
	{
		CLI_Printf("\r\nchannel: %d value: %d", i, (int)*(*values+i));
	};
}


char* printMinuteOff(char* str, unsigned char Value)
{
	if (0==Value)
		{
		strcpy(str, "off");
		}
	else if (1==Value)
		{
		strcpy(str, "instantly");
		}
	else if (2==Value)
		{
		strcpy(str, "1 min.");
		}
	else
		{
		sprintf(str, "%2d min.", Value-1);
		}
	return &(str[0]);
}


// ***************** implementation commands ****************

uint8_t BrightnessCmd()
{
	uint32_t channel_no = 0;
	uint32_t brightness_value = 0;

	// be sure arguments
	uint32_t type = CLI_GetArgHex(0);

	// optional arguments
	bool cflag = CLI_GetArgDecByFlag("-c", &channel_no) & (channel_no <= maxChannel);
	bool bflag = CLI_GetArgDecByFlag("-b", &brightness_value) & (brightness_value <= maxBrightnessLimit);

	switch (type)
	{
		case 0:
			CLI_Printf("\r\nBrightness last used before lights were switched off.");
			if (cflag & bflag)
			{
				GLOBAL_settings_ptr->Brightness_start[channel_no]=brightness_value;
			}
			printValueArray(&(GLOBAL_settings_ptr->Brightness_start));
			break;
		case 1:
			CLI_Printf("\r\nMinimum brightness when switched on:");
			if (cflag & bflag)
			{
				GLOBAL_settings_ptr->minBrightness[channel_no]=brightness_value;
			}
			printValueArray(&(GLOBAL_settings_ptr->minBrightness));
			break;
		case 2:
			CLI_Printf("\r\nMaximum brightness when switched on:");
			if (cflag & bflag)
			{
				GLOBAL_settings_ptr->maxBrightness[channel_no]=brightness_value;
			}
			printValueArray(&(GLOBAL_settings_ptr->maxBrightness));
			break;
		case 3:
			CLI_Printf("\r\nMaximum brightness when an alarm is triggered:");
			if (cflag & bflag)
			{
				GLOBAL_settings_ptr->AlarmBrightness[channel_no]=brightness_value;
			}
			printValueArray(&(GLOBAL_settings_ptr->AlarmBrightness));
			break;
		case 4:
			CLI_Printf("\r\nPWM offset for brightness:");
			if (cflag & bflag)
			{
				GLOBAL_settings_ptr->PWM_Offset[channel_no]=brightness_value;
			}
			printIntValueArray(&(GLOBAL_settings_ptr->PWM_Offset));
			break;
		default:
			return TE_ArgErr;
			break;
	}

	SettingsWrite();

	return TE_OK;
}

uint8_t GetExtBrightCmd()
{
	uint32_t duration = 0;

	// optional arguments
	// without parameters print last external brightness and current external brightness
	// with parameter -c print external brightness for duration in seconds
	CLI_GetArgHexByFlag("-d", &duration);

	//TODO get data

	return TE_OK;
}

uint8_t RemoteControlCmd()
{
	uint32_t address = 0;
	uint32_t receiverMode = 0;
	uint32_t senderMode = 0;

	// optional arguments
	if (CLI_GetArgDecByFlag("-a", &address) & (address <= maxRC5Addr))
	{
		GLOBAL_settings_ptr->RC5Addr = (uint8_t)address;
	}
	if (CLI_GetArgDecByFlag("-r", &receiverMode) & (receiverMode <= maxComMode))
	{
		GLOBAL_settings_ptr->ReceiverMode = (uint8_t)receiverMode;
	}
	if (CLI_GetArgDecByFlag("-s", &senderMode) & (senderMode <= maxComMode))
	{
		GLOBAL_settings_ptr->SenderMode = (uint8_t)senderMode;
	}

	CLI_Printf("\r\nRC5 address: %d\r\nreceiver mode: %d %s\r\nsender mode: %d %s",
			(int)GLOBAL_settings_ptr->RC5Addr,
			(int)GLOBAL_settings_ptr->ReceiverMode,
			ComModetext[GLOBAL_settings_ptr->ReceiverMode],
			(int)GLOBAL_settings_ptr->SenderMode,
			ComModetext[GLOBAL_settings_ptr->SenderMode]);

	SettingsWrite();

	return TE_OK;
}

uint8_t SetTimeCmd()
{
	RTC_TimeTypeDef sTimeRtcTemp;

	sTimeRtcTemp.Hours = CLI_GetArgDec(0);
	sTimeRtcTemp.Minutes = CLI_GetArgDec(1);
	sTimeRtcTemp.Seconds = CLI_GetArgDec(2);

	if (IS_RTC_HOUR24(sTimeRtcTemp.Hours) &
			IS_RTC_MINUTES(sTimeRtcTemp.Minutes) &
			IS_RTC_SECONDS(sTimeRtcTemp.Seconds))
	{
		//copy over values being assigned
		timeRtc.Hours = sTimeRtcTemp.Hours;
		timeRtc.Minutes = sTimeRtcTemp.Minutes;
		timeRtc.Seconds = sTimeRtcTemp.Seconds;
		Rtc_SetTime();
		GetTimestampCmd();
		return TE_OK;
	}
	return TE_ArgErr;
}

uint8_t SetDateCmd()
{
	RTC_DateTypeDef sDateRtcTemp;

	sDateRtcTemp.Year = CLI_GetArgDec(0);
	sDateRtcTemp.Month = CLI_GetArgDec(1);
	sDateRtcTemp.Date = CLI_GetArgDec(2);
	sDateRtcTemp.WeekDay = CLI_GetArgDec(3);

	if (IS_RTC_YEAR(sDateRtcTemp.Year) &
			IS_RTC_MONTH(sDateRtcTemp.Month) &
			IS_RTC_DATE(sDateRtcTemp.Date) &
			IS_RTC_WEEKDAY(sDateRtcTemp.WeekDay))
	{
		dateRtc = sDateRtcTemp;
		Rtc_SetDate();
		GetTimestampCmd();
		return TE_OK;
	}
	return TE_ArgErr;
}

uint8_t GetTimestampCmd()
{
	Rtc_GetDateTime();
	CLI_Printf("\r\nTimestamp: %02d-%02d-%02d %s %02d:%02d:%02d",
			dateRtc.Date, dateRtc.Month, dateRtc.Date, WeekdayNames[dateRtc.WeekDay],
			timeRtc.Hours, timeRtc.Minutes, timeRtc.Seconds)
	return TE_OK;
}

uint8_t AlarmSettingsCmd()
{
	uint32_t fading = 0;
	uint32_t signal = 0;
	uint32_t snooze = 0;
	char strf[12];
	char strsi[12];

	// optional arguments
	if (CLI_GetArgDecByFlag("-f", &fading))
	{
		GLOBAL_settings_ptr->LightFading = (uint8_t)fading;
	}
	if (CLI_GetArgDecByFlag("-si", &signal) & (signal < 100) )
	{
		GLOBAL_settings_ptr->AlarmTime2Signal = (uint8_t)signal;
	}
	if (CLI_GetArgDecByFlag("-sn", &snooze) & (snooze <= 30))
	{
		GLOBAL_settings_ptr->AlarmTimeSnooze = (uint8_t)snooze;
	}

	SettingsWrite();

	CLI_Printf("\r\ntime to fade-in light: %s, time to signal: %s, snooze time: %d min.",
			printMinuteOff(strf, GLOBAL_settings_ptr->LightFading),
			printMinuteOff(strsi, GLOBAL_settings_ptr->AlarmTime2Signal),
			(int)GLOBAL_settings_ptr->AlarmTimeSnooze);

	return TE_OK;
}

uint8_t AlarmScheduleCmd()
{
	uint32_t weekday = 0;
	uint32_t hour = 0;
	uint32_t minute = 0;

	// be sure arguments
	uint32_t i = CLI_GetArgDec(0);

	if (i <= maxAlarm)
	{
		// optional arguments
		// weekday 0 = off, weekday 8 = any weekday
		bool wflag = CLI_GetArgDecByFlag("-w", &weekday) & (weekday <= 8);
		bool bflag = CLI_GetArgDecByFlag("-h", &hour) & (hour < 24);
		// in case minutes are not set the full hour is assumed
		CLI_GetArgDecByFlag("-m", &minute);
		bool mflag = minute < 60;

		if (wflag & bflag & mflag)
		{
			GLOBAL_settings_ptr->Alarm[i].weekday = weekday;
			GLOBAL_settings_ptr->Alarm[i].hour = hour;
			GLOBAL_settings_ptr->Alarm[i].minute = minute;
		}

		CLI_Printf("\r\nAlarm no %d\r\nweekday: %d %s\r\ntime: %02d:%02d",
				(int)i,
				(int)GLOBAL_settings_ptr->Alarm[i].weekday,
				WeekdayNames[GLOBAL_settings_ptr->Alarm[i].weekday],
				(int)GLOBAL_settings_ptr->Alarm[i].hour,
				(int)GLOBAL_settings_ptr->Alarm[i].minute);

		SettingsWrite();
		Rtc_SetAlarm();

		return TE_OK;
	}
	return TE_ArgErr;
}

uint8_t AlarmCmd()
{
	uint32_t skipcnt = 0;
	uint32_t set = 0;

	// optional arguments
	if (CLI_GetArgDecByFlag("-s", &skipcnt) & (skipcnt <=alarmState.maxskipAlarmCnt))
	{
		alarmState.skipAlarmCnt = skipcnt;
		Rtc_SetAlarm();
	}

	if (CLI_GetArgDecByFlag("-a", &set) & (set <=1))
	{
		alarmState.alarmFlag = (set == 1);
	}

	if (noPendingAlarm == alarmState.nextAlarmIndex)
	{
		CLI_Printf("\r\nNo pending alarm.");
	}
	else
	{
		Rtc_GetAlarm();
		CLI_Printf("\r\nNext alarm no %d\r\nweekday: %d %s\r\ntime: %02d:%02d",
				(int)alarmState.nextAlarmIndex,
				(int)alarmRtc.AlarmDateWeekDay,
				WeekdayNames[alarmRtc.AlarmDateWeekDay],
				(int)alarmRtc.AlarmTime.Hours,
				(int)alarmRtc.AlarmTime.Minutes);
		CLI_Printf("\r\nSkipping #%d, current maximum skip count %d", alarmState.skipAlarmCnt, alarmState.maxskipAlarmCnt);
	}
	return TE_OK;
}


uint8_t SetBeepVolumeCmd()
{
	uint32_t volume = 0;

	// optional arguments
	if (CLI_GetArgDecByFlag("-v", &volume) & (volume <= maxBeepVolume))
	{
		GLOBAL_settings_ptr->BeepVolume = (uint8_t)volume;
	}

	CLI_Printf("\r\nBeep volume: %d", (int)GLOBAL_settings_ptr->BeepVolume);

	SettingsWrite();

	return TE_OK;
}

uint8_t ResetSettingsCmd()
{
	CLI_Printf("\r\nReset settings to factory defaults.");
	SettingsReset2Defaults();

	Rtc_SetAlarm();

	return TE_OK;
}

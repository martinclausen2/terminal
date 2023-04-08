#include <settingscommands.h>

void SettingsCommands_Init()
{
	CLI_AddCmd("alarmschedule", AlarmScheduleCmd, 1, TMC_PrintStartTime | TMC_PrintStopTime, "alarmschedule set schedule for the alarms: alarm no -w weekday -h hour -m minute");
	CLI_AddCmd("alarmsetting", AlarmSettingsCmd, 0, TMC_PrintStartTime | TMC_PrintStopTime, "alarmsetting set parameters for the alarms: -f time to fade -in light -si time to signal -sn snooze time");
	CLI_AddCmd("reset", ResetSettingsCmd, 0, TMC_PrintStartTime | TMC_PrintStopTime, "reset set settings to factory defaults");
	CLI_AddCmd("bright", BrightnessCmd, 1, TMC_PrintStartTime | TMC_PrintStopTime, "bright -t type -c channel_no -b brightness_value");
	CLI_AddCmd("getextbright", GetExtBrightCmd, 0, TMC_PrintStartTime | TMC_PrintStopTime, "getextbright -d duration");
	CLI_AddCmd("remote", RemoteControlCmd, 0, TMC_PrintStartTime | TMC_PrintStopTime, "remote -a address -r receiver mode -s sender mode");
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

// ***************** implementation commands ****************

uint8_t BrightnessCmd()
{
	uint32_t channel_no = 0;
	uint32_t brightness_value = 0;

	// be sure arguments
	uint32_t type = CLI_GetArgHex(0);

	// optional arguments
	bool cflag = CLI_GetArgDecByFlag("-c", &channel_no) & (channel_no <= maxChannel);
	bool bflag = CLI_GetArgDecByFlag("-b", &brightness_value);

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
			CLI_Printf("\r\nUnknown type");
			break;
	}

	SettingsWrite();

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

		CLI_Printf("\r\nAlarm no %d\r\nweekday: %d %s\r\ntime: %d:%d",
				(int)i,
				(int)GLOBAL_settings_ptr->Alarm[i].weekday,
				WeekdayNames[GLOBAL_settings_ptr->Alarm[i].weekday],
				(int)GLOBAL_settings_ptr->Alarm[i].hour,
				(int)GLOBAL_settings_ptr->Alarm[i].minute);

		SettingsWrite();

		return TE_OK;
	}
	else
	{
		return TE_ArgErr;
	}
}


uint8_t AlarmSettingsCmd()
{
	uint32_t fading = 0;
	uint32_t signal = 0;
	uint32_t snooze = 0;

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

	CLI_Printf("\r\ntime to fade-in light: %d min., time to signal: %d min., snooze time: %d min.",
			(int)GLOBAL_settings_ptr->LightFading,
			(int)GLOBAL_settings_ptr->AlarmTime2Signal,
			(int)GLOBAL_settings_ptr->AlarmTimeSnooze)

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

uint8_t ResetSettingsCmd()
{
	CLI_Printf("\r\nReset settings to factory defaults.");
	SettingsReset2Defaults();
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

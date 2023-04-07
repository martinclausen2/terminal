#include <settingscommands.h>

void SettingsCommands_Init()
{
	CLI_AddCmd("reset", ResetSettingsCmd, 0, TMC_PrintStartTime | TMC_PrintStopTime, "reset resets settings to factory defaults");
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
	bool cflag = CLI_GetArgDecByFlag("-c", &channel_no);
	if (channel_no > maxChannel)
	{
		cflag = false;
	}
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
	if (CLI_GetArgHexByFlag("-a", &address))
	{
		GLOBAL_settings_ptr->RC5Addr = (uint8_t)address;
	}
	if (CLI_GetArgHexByFlag("-r", &receiverMode))
	{
		GLOBAL_settings_ptr->ReceiverMode = (uint8_t)receiverMode;
	}
	if (CLI_GetArgHexByFlag("-s", &senderMode))
	{
		GLOBAL_settings_ptr->SenderMode = (uint8_t)senderMode;
	}

	CLI_Printf("\r\nRC5 address: %d receiver mode: %d sender mode: %d",
			(int)GLOBAL_settings_ptr->RC5Addr,
			(int)GLOBAL_settings_ptr->ReceiverMode,
			(int)GLOBAL_settings_ptr->SenderMode)

	SettingsWrite();

	return TE_OK;
}

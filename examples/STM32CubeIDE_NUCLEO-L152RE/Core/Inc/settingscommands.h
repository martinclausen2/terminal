#ifndef _COMMANDS_H_
#define _COMMANDS_H_

#include "terminal_helper.h"
#include <stdint.h>
#include "settings.h"

void SettingsCommands_Init();

uint8_t BrightnessCmd();
uint8_t GetExtBrightCmd();
uint8_t ResetSettingsCmd();
uint8_t RemoteControlCmd();

#endif // _COMMANDS_H_

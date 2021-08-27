#ifndef SETTINGS_H
#define SETTINGS_H

#include "motorfunctions.h"

typedef struct sSettings {
  motorFunction   Function;
  uint8_t         Strength;
  int8_t          currentValue;
  uint32_t        timerCounter;
  bool            lastButtonState;
  motorDirection  lastMotorDirection;
} tSettings;

motorFunction getSettingsFunction (uint8_t buttonId, uint8_t portId);
uint8_t getSettingsCurrentValue (uint8_t buttonId, uint8_t portId);
uint32_t getSettingsTimerCounter (uint8_t buttonId, uint8_t portId);
bool getSettingsLastButtonState (uint8_t buttonId, uint8_t portId);
motorDirection getSettingsLastMotorDirection(uint8_t buttonId, uint8_t portId);

// return true if value changed
bool setSettingsFunction (uint8_t  buttonId, uint8_t portId, motorFunction function);
bool setSettingsCurrentValue (uint8_t  buttonId, uint8_t portId, uint8_t value);

void setSettingsTimerCounter (uint8_t  buttonId, uint8_t portId, uint32_t timer, bool lastButtonState, motorDirection lastMotorDirection);
bool countDownSettingsTimer (uint8_t  buttonId, uint8_t portId);


#endif

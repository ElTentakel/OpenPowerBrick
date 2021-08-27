#include "settings.h"
#include "motorfunctions.h"

static tSettings SettingsMatrix[4][4] = { 
  [0] = {{ FullForward, 4, 0, 0, false }, { No, 0, 0, 0, false }, { No, 0, 0, 0, false }, { No, 0, 0, 0, false }},
  [1] = {{ No, 0, 0, 0, false }, { FullForward, 4, 0, 0, false }, { No, 0, 0, 0, false }, { No, 0, 0, 0, false }},
  [2] = {{ No, 0, 0, 0, false }, { No, 0, 0, 0, false }, { FullForward, 4, 0, 0, false }, { No, 0, 0, 0, false }},
  [3] = {{ No, 0, 0, 0, false }, { No, 0, 0, 0, false }, { No, 0, 0, 0, false }, { FullForward, 4, 0, 0, false }}
};

motorFunction getSettingsFunction (uint8_t buttonId, uint8_t portId)
{
  return SettingsMatrix [buttonId][portId].Function;
}

// return true if value changed
bool setSettingsFunction (uint8_t  buttonId, uint8_t portId, motorFunction function)
{
  bool ret = false;
  if (getSettingsFunction (buttonId, portId) != function)
    ret = true;
    
  SettingsMatrix [buttonId][portId].Function = function;
  return ret;
}

uint8_t getSettingsCurrentValue (uint8_t buttonId, uint8_t portId)
{
  return SettingsMatrix [buttonId][portId].currentValue;
}

// return true if value changed
bool setSettingsCurrentValue (uint8_t  buttonId, uint8_t portId, uint8_t value)
{
  bool ret = false;
  if (getSettingsCurrentValue (buttonId, portId) != value)
    ret = true;
  
  SettingsMatrix [buttonId][portId].currentValue = value;
  return ret;
}

uint32_t getSettingsTimerCounter (uint8_t buttonId, uint8_t portId)
{
  return SettingsMatrix [buttonId][portId].timerCounter;
}

bool getSettingsLastButtonState (uint8_t buttonId, uint8_t portId)
{
  return SettingsMatrix [buttonId][portId].lastButtonState;
}

motorDirection getSettingsLastMotorDirection(uint8_t buttonId, uint8_t portId)
{
  return SettingsMatrix [buttonId][portId].lastMotorDirection;
}

void setSettingsTimerCounter (uint8_t  buttonId, uint8_t portId, uint32_t timer, bool lastButtonState, motorDirection lastMotorDirection)
{
  SettingsMatrix [buttonId][portId].timerCounter = timer;
  SettingsMatrix [buttonId][portId].lastButtonState = lastButtonState;
  SettingsMatrix [buttonId][portId].lastMotorDirection = lastMotorDirection;
}

bool countDownSettingsTimer (uint8_t  buttonId, uint8_t portId)
{
  if (SettingsMatrix [buttonId][portId].timerCounter > 0)
  {
    SettingsMatrix [buttonId][portId].timerCounter = SettingsMatrix [buttonId][portId].timerCounter - 1;
    return true;
  }
  return false;
}

#ifndef SETTINGS_H
#define SETTINGS_H

#include "motorfunctions.h"

typedef struct sSettings {
  motorFunction   Function;
  uint8_t         Strength;
  int8_t          currentValue;
} tSettings;

motorFunction getSettingsFunction (uint8_t buttonId, uint8_t portId);
uint8_t getSettingsCurrentValue (uint8_t buttonId, uint8_t portId);

// return true if value changed
bool setSettingsFunction (uint8_t  buttonId, uint8_t portId, motorFunction function);
// return true if value changed
bool setSettingsCurrentValue (uint8_t  buttonId, uint8_t portId, uint8_t value);

#endif

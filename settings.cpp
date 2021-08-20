#include "settings.h"
#include "motorfunctions.h"

tSettings SettingsMatrix[4][4] = { [0] = {{ FullForward, 4 , 0 }, { No, 0, 0 }, { No, 0, 0 }, { No, 0, 0 }},
  [1] = {{ No, 0, 0 }, { FullForward, 4, 0 }, { No, 0, 0 }, { No, 0, 0 }},
  [2] = {{ No, 0, 0 }, { No, 0, 0 }, { FullForward, 4, 0 }, { No, 0, 0 }},
  [3] = {{ No, 0, 0 }, { No, 0, 0 }, { No, 0, 0 }, { FullForward, 4, 0 }}
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

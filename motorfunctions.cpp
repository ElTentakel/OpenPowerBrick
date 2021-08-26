#include "motorfunctions.h"
#include "motor.h"
#include "settings.h"
#include "display.h"
#include "hub.h"
#include "menu.h"

motorFunction motorFunctionMax = static_cast<motorFunction>(static_cast<int>(motorFunctionLast) - 1);

int8_t MotorFunctionNo  (int8_t currentValue, motorDirection Action, bool buttonPressed, int8_t Steps)
{
  return currentValue;
}

int8_t MotorFunctionFullForward  (int8_t currentValue, motorDirection Action, bool buttonPressed, int8_t Steps)
{
  if (buttonPressed && Action == Forward)
  {
    return Steps;
  }
  else if (buttonPressed && Action == Backward)
  {
    return -Steps;
  }
  else
  {
    return 0;
  }
}

int8_t MotorFunctionFullBackward (int8_t currentValue, motorDirection Action, bool buttonPressed, int8_t Steps)
{
  if (buttonPressed && Action == Forward)
  {
    return -Steps;
  }
  else if (buttonPressed && Action == Backward)
  {
    return Steps;
  }
  else
  {
    return 0;
  }
}

int8_t MotorFunctionStepForward  (int8_t currentValue, motorDirection Action, bool buttonPressed, int8_t Steps)
{
  if (buttonPressed && Action == Forward && currentValue < Steps)
  {
    return currentValue + 1;
  }
  else if (buttonPressed && Action == Backward && currentValue > -Steps)
  {
    return currentValue - 1;
  }
  else if (buttonPressed && Action == Stop)
  {
    return 0;
  }
  else
  {
    return currentValue;
  }
}
int8_t MotorFunctionStepBackward (int8_t currentValue, motorDirection Action, bool buttonPressed, int8_t Steps)
{
  if (buttonPressed && Action == Forward && currentValue > -Steps)
  {
    return currentValue - 1;
  }
  else if (buttonPressed && Action == Backward && currentValue < Steps)
  {
    return currentValue + 1;
  }
  else if (buttonPressed && Action == Stop)
  {
    return 0;
  }
  else
  {
    return currentValue;
  }
}

void updateMotorFunction (uint8_t buttonId, uint8_t portId, bool reverse)
{
  motorFunction function = getSettingsFunction(buttonId, portId);

  if (reverse == false)
  {
    if (static_cast<int>(function) < static_cast<int>(motorFunctionMax))
    {
      function = static_cast<motorFunction>(static_cast<int>(function) + 1);
    }
    else
    {
      function = No;
    }
  }
  else
  {
    if (static_cast<int>(function) > (static_cast<int>(Off)))
    {
      function = static_cast<motorFunction>(static_cast<int>(function) - 1);
    }
    else
    {
      function = motorFunctionMax;
    }
  }
  setSettingsFunction (buttonId, portId, function);
  MotorResetCalibtation(portId);
}

void controlMotorFuntion (uint8_t id, motorDirection dir, bool buttonPressed)
{
  int8_t newValue;
  for ( uint8_t i = 0; i <= getMaxPortId(); i++)
  {
    switch (getSettingsFunction(id,i))
    {
      case FullForward:
        Serial.println("FullForward");
        newValue = MotorFunctionFullForward  (getSettingsCurrentValue(id,i), dir, buttonPressed, functionParameters[FullForward].Steps);
        break;
      case FullBackward:
        Serial.println("FullBackward");
        newValue = MotorFunctionFullBackward (getSettingsCurrentValue(id,i), dir, buttonPressed, functionParameters[FullBackward].Steps);
        break;
      case StepForward:
        Serial.println("StepForward");
        newValue = MotorFunctionStepForward  (getSettingsCurrentValue(id,i), dir, buttonPressed, functionParameters[StepForward].Steps);
        break;
      case StepBackward:
        Serial.println("StepBackward");
        newValue = MotorFunctionStepBackward (getSettingsCurrentValue(id,i), dir, buttonPressed, functionParameters[StepBackward].Steps);
        break;
      case StepForward8:
        Serial.println("StepForward8");
        newValue = MotorFunctionStepForward  (getSettingsCurrentValue(id,i), dir, buttonPressed, functionParameters[StepForward8].Steps);
        break;
      case StepBackward8:
        Serial.println("StepBackward8");
        newValue = MotorFunctionStepBackward (getSettingsCurrentValue(id,i), dir, buttonPressed, functionParameters[StepBackward8].Steps);
        break;
      case SteeringForward:
        Serial.println("SteeringForward");
        newValue = MotorFunctionFullForward  (getSettingsCurrentValue(id,i), dir, buttonPressed, functionParameters[SteeringForward].Steps);
        break;
      case SteeringBackward:
        Serial.println("SteeringBackward");
        newValue = MotorFunctionFullBackward (getSettingsCurrentValue(id,i), dir, buttonPressed, functionParameters[SteeringBackward].Steps);
        break;
      default:
        newValue = getSettingsCurrentValue(id,i);
        break;
    }

    if (setSettingsCurrentValue(id,i, newValue))
    {
      if (id <= getMaxRemoteButton())
      {
        if ((getSettingsFunction(id,i) == SteeringForward ||
             getSettingsFunction(id,i) == SteeringBackward) &&
             MotorIsCalibrated (getHubPort(i))
           )
        {
          setSteeringMotorPosition (i, newValue, functionParameters[getSettingsFunction (id, i)].Steps, getHub(i), getHubPort(i));
        }
        else
        {
          // Todo: use hubType for second PU Hub
          setSimpleMotorSpeed (i, newValue, functionParameters[getSettingsFunction (id, i)].Steps, getHub(i), getHubPort(i));
        }
      }

      // Todo: use animation values
      if (newValue == 0)
        display_set (i+1,id+1, 0, 0, 0, true);

      if (newValue > 0)
        display_set (i+1,id+1, 0, 255, 0, true);

      if (newValue < 0)
        display_set (i+1,id+1, 255, 0, 0, true);
    }
  }
}

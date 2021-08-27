#include "motorfunctions.h"
#include "motor.h"
#include "settings.h"
#include "display.h"
#include "hub.h"
#include "menu.h"

static motorFunction motorFunctionMax = static_cast<motorFunction>(static_cast<int>(motorFunctionLast) - 1);

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

void checkTimedMotorFunctions ()
{
  for ( uint8_t i = 0; i <= getMaxPortId(); i++)
  {
    for ( uint8_t j = 0; j <= getMaxRemoteButton(); j++)
    {
      if (countDownSettingsTimer(j, i))
      {
        controlMotorFuntion(j, i, getSettingsLastMotorDirection(j,i), getSettingsLastButtonState(j,i), true);
      }
    }
  }
}

void controlMotorFuntions (uint8_t buttonId, motorDirection dir, bool buttonPressed)
{
  for ( uint8_t i = 0; i <= getMaxPortId(); i++)
  {
    controlMotorFuntion (buttonId, i, dir, buttonPressed, false);
  }
}

void controlMotorFuntion (uint8_t buttonId, uint8_t portId, motorDirection dir, bool buttonPressed, bool isEvent)
{
  int8_t newValue;
  switch (getSettingsFunction(buttonId,portId))
  {
    case FullForward:
      Serial.println("FullForward");
      newValue = MotorFunctionFullForward  (getSettingsCurrentValue(buttonId,portId), dir, buttonPressed, functionParameters[FullForward].Steps);
      break;
    case FullBackward:
      Serial.println("FullBackward");
      newValue = MotorFunctionFullBackward (getSettingsCurrentValue(buttonId,portId), dir, buttonPressed, functionParameters[FullBackward].Steps);
      break;
    case StepForward:
      Serial.println("StepForward");
      newValue = MotorFunctionStepForward  (getSettingsCurrentValue(buttonId,portId), dir, buttonPressed, functionParameters[StepForward].Steps);
      break;
    case StepBackward:
      Serial.println("StepBackward");
      newValue = MotorFunctionStepBackward (getSettingsCurrentValue(buttonId,portId), dir, buttonPressed, functionParameters[StepBackward].Steps);
      break;
    case StepForward8:
      Serial.println("StepForward8");
      newValue = MotorFunctionStepForward  (getSettingsCurrentValue(buttonId,portId), dir, buttonPressed, functionParameters[StepForward8].Steps);
      break;
    case StepBackward8:
      Serial.println("StepBackward8");
      newValue = MotorFunctionStepBackward (getSettingsCurrentValue(buttonId,portId), dir, buttonPressed, functionParameters[StepBackward8].Steps);
      break;
    case SteeringForward:
      Serial.println("SteeringForward");
      newValue = MotorFunctionFullForward  (getSettingsCurrentValue(buttonId,portId), dir, buttonPressed, functionParameters[SteeringForward].Steps);
      break;
    case SteeringBackward:
      Serial.println("SteeringBackward");
      newValue = MotorFunctionFullBackward (getSettingsCurrentValue(buttonId,portId), dir, buttonPressed, functionParameters[SteeringBackward].Steps);
      break;
    default:
      newValue = getSettingsCurrentValue(buttonId,portId);
      break;
  }

  if (setSettingsCurrentValue(buttonId,portId, newValue))
  {
    if (buttonId <= getMaxRemoteButton())
    {
      if ((getSettingsFunction(buttonId,portId) == SteeringForward ||
           getSettingsFunction(buttonId,portId) == SteeringBackward) &&
           MotorIsCalibrated (getHubPort(portId))
         )
      {
        setSteeringMotorPosition (portId, newValue, functionParameters[getSettingsFunction (buttonId, portId)].Steps, getHub(portId), getHubPort(portId));
      }
      else
      {
        setSimpleMotorSpeed (portId, newValue, functionParameters[getSettingsFunction (buttonId, portId)].Steps, getHub(portId), getHubPort(portId));
      }
    }

    // Todo: use animation values
    if (newValue == 0)
      display_set (portId+1,buttonId+1, 0, 0, 0, true);

    if (newValue > 0)
      display_set (portId+1,buttonId+1, 0, 255, 0, true);

    if (newValue < 0)
      display_set (portId+1,buttonId+1, 255, 0, 0, true);
  }
}

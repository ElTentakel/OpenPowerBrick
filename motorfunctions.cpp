#include "motorfunctions.h"
#include "motor.h"

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

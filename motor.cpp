#include "motor.h"

void setSimpleMotorSpeed (uint8_t id, int8_t currentValue, int8_t maxValue, Lpf2Hub* pPUHub, byte* PUport)
{
  int Speed =  (100 * currentValue) / maxValue;

  Serial.println("setSimpleMotorSpeed ID:" + String(id) + " Speed:" + String(currentValue));

  pPUHub->setBasicMotorSpeed(PUport[id], Speed);
}

#ifndef MOTOR_H
#define MOTOR_H

#include <stdint.h> 
#include "motorfunctions.h"
#include "Lpf2Hub.h"

typedef struct sSettings {
  motorFunction   Function;
  uint8_t         Strength;
  int8_t          currentValue;
} tSettings;

void setSimpleMotorSpeed (uint8_t id, int8_t currentValue, int8_t maxValue, Lpf2Hub* pPUHub, byte PUport);

#endif

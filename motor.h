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

enum MotorState_      { motorInit = 0,
                        motorNotConnected = 1,
                        motorNoCalibration = 2,
                        motorCalibration = 3,
                        motorNegativeCalibration = 4,
                        motorPositiveCalibration = 5,
                        motorCalibrated,
                      };

void setSimpleMotorSpeed (uint8_t id, int8_t currentValue, int8_t maxValue, Lpf2Hub* pPUHub, byte PUport);

bool MotorIsCalibrated (byte port);

void tachoMotorCallback(void *hub, byte portNumber, DeviceType deviceType, uint8_t *pData);

#endif

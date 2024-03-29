#ifndef MOTOR_H
#define MOTOR_H

#include <stdint.h> 
#include "motorfunctions.h"
#include "Lpf2Hub.h"

enum MotorState_      { motorInit = 0,
                        motorNotConnected,
                        motorNoCalibration,
                        motorReadyForCalibration,
                        motorStartCalibration,
                        motorNegativeCalibration,
                        motorPositiveCalibration,
                        motorCalibrated,
                      };

void setSimpleMotorSpeed (uint8_t id, int8_t currentValue, int8_t maxValue, Lpf2Hub* pPUHub, byte PUport);
void setSteeringMotorPosition (uint8_t id, int8_t currentValue, int8_t maxValue, Lpf2Hub* pPUHub, byte PUport);

void checkMotorCalibration();
void MotorResetCalibtation (int port);
bool MotorStartCalibration (int port);
bool MotorIsCalibrating (int port);
bool MotorCalibrationStep (void *hub, byte portNumber);
bool MotorIsCalibrated (int port);

void tachoMotorCallback(void *hub, byte portNumber, DeviceType deviceType, uint8_t *pData);

#endif

#include "motor.h"

MotorState_ MotorState[4] = {motorInit, motorInit, motorInit, motorInit};
int MotorPosition[4] = {0,0,0,0};
int MotorCalibrationLeft[4] = {0,0,0,0};
int MotorCalibrationRight[4] = {0,0,0,0};
int MotorCalibrationCounter[4] = {0,0,0,0};

void setSimpleMotorSpeed (uint8_t id, int8_t currentValue, int8_t maxValue, Lpf2Hub* pPUHub, byte PUport)
{
  int Speed =  (100 * currentValue) / maxValue;

  Serial.println("setSimpleMotorSpeed ID:" + String(id) + " Speed:" + String(currentValue));

  pPUHub->setBasicMotorSpeed(PUport, Speed);
}
/*
void setSteeringMotorPosition (unsigned int value int8_t maxValue, Lpf2Hub* pPUHub, byte PUport)
{
  int Angle; 
  // Todo: calculate motor position
  // myHub->setAbsoluteMotorPosition(portNumber, 100, 0);
}
*/
// callback function to handle updates of sensor values
void tachoMotorCallback(void *hub, byte portNumber, DeviceType deviceType, uint8_t *pData)
{
  Lpf2Hub *myHub = (Lpf2Hub *)hub;
  int rotation = 0;

  // Serial.print("sensorMessage callback for port: ");
  // Serial.println(portNumber, DEC);

  if (deviceType == DeviceType::MEDIUM_LINEAR_MOTOR
      || deviceType == DeviceType::TECHNIC_MEDIUM_ANGULAR_MOTOR
      || deviceType == DeviceType::TECHNIC_LARGE_ANGULAR_MOTOR
      || deviceType == DeviceType::TECHNIC_LARGE_LINEAR_MOTOR
      || deviceType == DeviceType::TECHNIC_XLARGE_LINEAR_MOTOR
     )
  {
    if (MotorState[(int)portNumber] == motorInit)
    {
      Serial.println("motorCalibration");
      MotorState[(int)portNumber] = motorReadyForCalibration;
    }
    rotation = myHub->parseTachoMotor(pData);
    MotorPosition[(int)portNumber] = rotation;
    Serial.print("rotation:");
    Serial.println(rotation, DEC);
  }
  else
  {
    MotorState[(int)portNumber] = motorNoCalibration;
  }
}

bool MotorStartCalibration (int port)
{
  bool ret = false;
  if (MotorState [port] == motorReadyForCalibration)
  {
    MotorState [port] = motorStartCalibration;
    ret = true;
  }
  return ret;
}

bool MotorIsCalibrated (int port)
{
  bool ret = false;
  if (MotorState [port] == motorCalibrated)
  {
    ret = true;
  }
  return ret;
}

bool MotorIsCalibrating (int port)
{
  bool ret = false;
  if (MotorState [port] == motorStartCalibration ||
      MotorState [port] == motorNegativeCalibration ||
      MotorState [port] == motorPositiveCalibration)
  {
    ret = true;
  }
  return ret;
}

// return true if no calibration is ongoing
bool MotorCalibrationStep (void *hub, byte portNumber)
{
  Lpf2Hub *myHub = (Lpf2Hub *)hub;
  bool ret = false;

  if (MotorState [(int)portNumber] == motorNoCalibration ||
      MotorState [(int)portNumber] == motorCalibrated)
  {
    return true;
  }

  // we need some time to get some callback messages
  if (MotorCalibrationCounter[(int)portNumber] < 5)
  {
    MotorCalibrationCounter[(int)portNumber]++;
    return ret;
  }

  MotorCalibrationCounter[(int)portNumber] = 0;

  /* Debug info
    Serial.print ((int)portNumber);
    Serial.print ("/");
    Serial.print (MotorPosition[(int)portNumber]);
    Serial.print ("/");
    Serial.print (MotorCalibrationLeft[(int)portNumber]);
    Serial.print ("/");
    Serial.println (MotorCalibrationRight[(int)portNumber]);
  */

  switch (MotorState [(int)portNumber])
  {
    case motorInit:
    case motorReadyForCalibration:
      // wait for motor
      break;
    case motorStartCalibration:
      MotorState [(int)portNumber] = motorNegativeCalibration;
      MotorCalibrationLeft[(int)portNumber] = MotorPosition[(int)portNumber] + 20;
      myHub->setTachoMotorSpeedForTime(portNumber, -70, 300, 100);
      Serial.print("motorNegativeCalibration:");
      break;
    case motorNegativeCalibration:
      if (MotorPosition[(int)portNumber] < MotorCalibrationLeft[(int)portNumber])
      {
        myHub->setTachoMotorSpeedForTime(portNumber, -70, 300, 100);
        MotorCalibrationLeft[(int)portNumber] = MotorPosition[(int)portNumber];
      }
      else
      {
        Serial.print("motorPositiveCalibration:");
        MotorCalibrationRight[(int)portNumber] = MotorPosition[(int)portNumber] - 20;
        myHub->setTachoMotorSpeedForTime(portNumber, +70, 300, 10050);
        MotorState [(int)portNumber] = motorPositiveCalibration;
      }
      break;;
    case motorPositiveCalibration:
      if (MotorPosition[(int)portNumber] > MotorCalibrationRight[(int)portNumber])
      {
        myHub->setTachoMotorSpeedForTime(portNumber, +70, 300, 100);
        MotorCalibrationRight[(int)portNumber] = MotorPosition[(int)portNumber];
      }
      else
      {
        MotorState [(int)portNumber] = motorCalibrated;
        ret = true;
        Serial.print("motorCalibrated Motor ");
        Serial.print ((int)portNumber);
        Serial.print(":");
        Serial.print (MotorCalibrationLeft[(int)portNumber]);
        Serial.print ("/");
        Serial.println (MotorCalibrationRight[(int)portNumber]);
        myHub->setAbsoluteMotorPosition(portNumber, 100, 0);
      }
      break;
    default:
      ret = true;
      break;
  }
  return ret;
}

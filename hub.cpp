#include "hub.h"
#include <stdint.h>
#include "Lpf2Hub.h"
#include "menu.h"
#include "display.h"

static Lpf2Hub   PUHub[2];
static byte      PUport[4]       = {(byte)PoweredUpHubPort::A, (byte)PoweredUpHubPort::B, (byte)PoweredUpHubPort::A, (byte)PoweredUpHubPort::B };
static InitState HubInitState[2] = {notConnected, notConnected};
static hubType   type[2]         = {noHub, noHub};


Lpf2Hub *getHub(uint8_t port)
{
  if (type[0] == type[1] == PoweredUpHub && port > 1)
  {
    return &PUHub[1];
  }
  else
  {
    return &PUHub[0];
  }
}

hubType getHubType(uint8_t hub)
{
  return type[hub];
}

byte getHubPort(uint8_t port)
{
  return PUport[port];
}

void connectedHub(uint8_t i)
{
  if (!PUHub[i].isConnected())
  {
    PUHub[i].init();
  }
}

void configureHub (uint8_t id)
{
  HubInitState[id] = Configured;
}

void tryConnectHubs()
{
  if (PUHub[0].isConnecting())
  {
    if (PUHub[0].getHubType() == HubType::POWERED_UP_HUB)
    {
      PUHub[0].connectHub();
      PUHub[0].setLedColor(GREEN);
      Serial.println("powered up hub 1 connected.");
      PUHub[0].setLedColor(GREEN);
      display_set(1, 0, 0, 255, 0);
      display_set(2, 0, 0, 255, 0, true);

      PUport[0] = (byte)PoweredUpHubPort::A;
      PUport[1] = (byte)PoweredUpHubPort::B;

      type[0] = PoweredUpHub;

      PUHub[0].activatePortDevice(PUport[0], tachoMotorCallback);
      PUHub[0].activatePortDevice(PUport[1], tachoMotorCallback);

      setSubStateMax(Menu2);
      connectedHub(1);
    }
    else if (PUHub[0].getHubType() == HubType::CONTROL_PLUS_HUB)
    {
      PUHub[0].connectHub();
      PUHub[0].setLedColor(GREEN);
      Serial.println("control plus hub connected.");
      PUHub[0].setLedColor(GREEN);
      display_set(1, 0, 0, 255, 0);
      display_set(2, 0, 0, 255, 0);
      display_set(3, 0, 0, 255, 0);
      display_set(4, 0, 0, 255, 0, true);

      // Switch Port Configuration to Control+
      PUport[0] = (byte)ControlPlusHubPort::A;
      PUport[1] = (byte)ControlPlusHubPort::B;
      PUport[2] = (byte)ControlPlusHubPort::C;
      PUport[3] = (byte)ControlPlusHubPort::D;

      type[0] = ControlPlusHub;

      PUHub[0].activatePortDevice(PUport[0], tachoMotorCallback);
      PUHub[0].activatePortDevice(PUport[1], tachoMotorCallback);
      PUHub[0].activatePortDevice(PUport[2], tachoMotorCallback);
      PUHub[0].activatePortDevice(PUport[3], tachoMotorCallback);

      setSubStateMax(Menu4);
    }
  }
  else if (PUHub[1].isConnecting())
  {
    if (PUHub[1].getHubType() == HubType::POWERED_UP_HUB)
    {
      PUHub[1].connectHub();
      PUHub[1].setLedColor(GREEN);
      Serial.println("powered up hub 2 connected.");
      PUHub[1].setLedColor(GREEN);
      display_set(3, 0, 0, 255, 0);
      display_set(4, 0, 0, 255, 0, true);
      
      PUport[2] = (byte)PoweredUpHubPort::A;
      PUport[3] = (byte)PoweredUpHubPort::B;
      
      type[1] = PoweredUpHub;
      
      PUHub[2].activatePortDevice(PUport[0], tachoMotorCallback);
      PUHub[3].activatePortDevice(PUport[1], tachoMotorCallback);
      
      setSubStateMax(Menu4);
    }
  }
}

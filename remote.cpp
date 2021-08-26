#include "remote.h"
#include "menu.h"
#include "display.h"
#include "motorfunctions.h"
#include "Lpf2Hub.h"
#include <stdint.h> 

Lpf2Hub  Remote[2];
byte RemotePortLeft[2] = { (byte)PoweredUpRemoteHubPort::LEFT, (byte)PoweredUpRemoteHubPort::LEFT};
byte RemotePortRight[2] = { (byte)PoweredUpRemoteHubPort::RIGHT, (byte)PoweredUpRemoteHubPort::RIGHT};

// callback function to handle updates of remote buttons
void remoteCallback(void *hub, byte portNumber, DeviceType deviceType, uint8_t *pData)
{
  Lpf2Hub *myRemoteHub = (Lpf2Hub *)hub;

  // Serial.print("remote callback port: ");
  // Serial.println(portNumber, DEC);
  if (deviceType == DeviceType::REMOTE_CONTROL_BUTTON)
  {
    ButtonState buttonState = myRemoteHub->parseRemoteButton(pData);
    Serial.print("Buttonstate: ");
    Serial.println((byte)buttonState, HEX);

    switch (getState())
    {
      case Init:
        break;
      case Menu1:
        switch (buttonState)
        {
          case ButtonState::STOP:
            controlMotorFuntion (portNumber, Stop, true);
            break;

          case ButtonState::UP:
            controlMotorFuntion (portNumber, Forward, true);
            break;

          case ButtonState::DOWN:
            controlMotorFuntion (portNumber, Backward, true);
            break;

          case ButtonState::RELEASED:
            controlMotorFuntion (portNumber, Backward, false);
            break;

          default:
            break;
        }
        break;
      default:
        if (portNumber == 0 && buttonState == ButtonState::UP)
          updateSubState (true);
        else if (portNumber == 0 && buttonState == ButtonState::DOWN)
          updateSubState ();
        else if (portNumber == 1 && buttonState == ButtonState::UP)
        {
          updateMotorFunction (getRemoteButton(), getPortId());
        }
        else if (portNumber == 1 && buttonState == ButtonState::DOWN)
        {
          updateMotorFunction (getRemoteButton(), getPortId(), true);
        }
        display_redrawFunctions();
        display_resetAnimationCounter();
        display_counterDelay(250);
        break;
    }
  }
}

// Wrapper for second Remote
void remoteCallback2(void *hub, byte portNumber, DeviceType deviceType, uint8_t *pData)
{
  remoteCallback(hub, portNumber + 2, deviceType, pData);
}

void configureRemote (uint8_t id)
{
  Serial.println("Registering remote:" + String(static_cast<uint8_t>(id)));
  switch (id)
  {
    case 0:
      Remote[id].activatePortDevice(RemotePortLeft[id], remoteCallback);
      delay(50);
      Remote[id].activatePortDevice(RemotePortRight[id], remoteCallback);
      delay(50);
      Remote[id].setLedColor(WHITE);
      break;
    case 1:
      Remote[id].activatePortDevice(RemotePortLeft[id], remoteCallback2);
      delay(50);
      Remote[id].activatePortDevice (RemotePortRight[id], remoteCallback2);
      delay(50);
      Remote[id].setLedColor(YELLOW);
      break;
    default:
      Serial.println("Error, unknown remote:");
      break;
  }
}

void tryConnectRemotes ()
{
  if (Remote[0].isConnecting())
  {
    if (Remote[0].getHubType() == HubType::POWERED_UP_REMOTE)
    {
      //This is the right device
      if (!Remote[0].connectHub())
      {
        Serial.println("Unable to connect to hub");
      }
      else
      {
        Remote[0].setLedColor(GREEN);
        Serial.println("Remote 0 connected.");
      }
    }
    Remote[0].setLedColor(GREEN);
    display_set(0, 1, 0, 255, 0);
    display_set(0, 2, 0, 255, 0, true);
    setStateMax(Menu3);
    // enable registration for 2. Remote
    Remote[1].init();
  }

  if (Remote[1].isConnecting())
  {
    if (Remote[1].getHubType() == HubType::POWERED_UP_REMOTE)
    {
      //This is the right device
      if (!Remote[1].connectHub())
      {
        Serial.println("Unable to connect to hub");
      }
      else
      {
        Remote[1].setLedColor(YELLOW);
        Serial.println("Remote 1 connected.");
      }
    }
    Remote[1].setLedColor(GREEN);
    display_set(0, 3, 0, 255, 0);
    display_set(0, 4, 0, 255, 0, true);
    setStateMax(Menu5);
  }
}

void connectedRemote(uint8_t i)
{
  if (!Remote[i].isConnected())
  {
    Remote[i].init();
  }
}

Lpf2Hub *getRemote(uint8_t port)
{
  return &Remote[port];
}

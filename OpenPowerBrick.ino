/**
   M5Stack Matrix Atom Application to control PoweredUp! and Control+ Devices

   Plattform: ESP32 Pico Kit

   2021 by Richard Jeske
   Released GPL 2.0 Licence

*/

#include "M5Atom.h"
#include "hub.h"
#include "remote.h"
#include "menu.h"
#include "motor.h"
#include "motorfunctions.h"
#include "display.h"
#include "settings.h"

void updateState ()
{
  display_clear ();

  if (updateAndResetState())
  {
    // Do calibration stuff for Servos
    for (int j = 0; j <= getMaxPortId(); j++)
    {
      Serial.println("");
      Serial.print("Port:" + String (j) + ":");

      for (int i = 0; i <= getMaxRemoteButton(); i++)
      {
        Serial.print(String(getSettingsFunction(i,j)));

        if ((getSettingsFunction(i,j) == SteeringForward ||
             getSettingsFunction(i,j) == SteeringBackward)  &&
            !MotorIsCalibrated (getHubPort(j)) &&
            !MotorIsCalibrating (getHubPort(j)))
        {
          Serial.print("*");
          MotorStartCalibration (getHubPort(j));
          break;
        }
        Serial.print(",");
      }
    }
    Serial.println("");
  }

  if (getState() != Init && getState() != Menu1)
  {
    resetSubState();
    updateSubState();
    display_redrawFunctions();
    display_resetAnimationCounter();
    display_counterDelay(250);
  }

  Serial.println("State: " + String(static_cast<uint8_t>(getState())));
  display_set (0, static_cast<uint8_t>(getState()) - 1, 255, 255, 255, true);
}

void setup() {
  Serial.begin(115200);

  getRemote(0)->init();
  getHub(0)->init();

  M5.begin(true, false, true);
  display_counterDelay(50);
  display_init();
}

void loop()
{
  // connect flow
  if (getState() == Init)
  {
    tryConnectRemotes();
    tryConnectHubs();
    
    connectedRemote(0);
    connectedHub(0);

    if (M5.Btn.wasPressed())
    {
      configureHub (0);
      configureRemote (0);
      if (getMaxState() == Menu5)
      {
        configureRemote (1);
      }
      updateState();
    }

    // Animation
    display_initAnimation();
  }
  else
  {
    // menu flow
    switch (getState())
    {
      case Init:
        break;
      case Menu1:
        checkMotorCalibration();
        checkTimedMotorFunctions(5); // check time function only every 5th call
        break;
      default:
        checkMotorCalibration();
        display_drawFunctionAnimation();
        break;
    }

    if (M5.Btn.wasPressed())
    {
      updateState();
    }
  }
  display_counterDelay (50);
  M5.update();
}

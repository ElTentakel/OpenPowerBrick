/**
   M5Stack Matrix Atom Application to control PoweredUp! and Control+ Devices

   Plattform: ESP32 Pico Kit

   2021 by Richard Jeske
   Released GPL 2.0 Licence

*/

#include "Lpf2Hub.h"
#include "M5Atom.h"

Lpf2Hub  Remote[2];
Lpf2Hub  PUHub[2];

byte RemotePortLeft[2] = { (byte)PoweredUpRemoteHubPort::LEFT, (byte)PoweredUpRemoteHubPort::LEFT};
byte RemotePortRight[2] = { (byte)PoweredUpRemoteHubPort::RIGHT, (byte)PoweredUpRemoteHubPort::RIGHT};
byte PUport[4] = { (byte)PoweredUpHubPort::A, (byte)PoweredUpHubPort::B, (byte)PoweredUpHubPort::A, (byte)PoweredUpHubPort::B };

#include "hub.h"
#include "menu.h"
#include "motor.h"
#include "motorfunctions.h"
#include "display.h"
#include "settings.h"

bool initAnimationState = false;
InitState HubInitState[2] = {notConnected, notConnected};

typedef struct sRemoteState
{
  bool  RightPressed;
  bool  LeftPressed;
} tRemoteState;

tRemoteState RemoteState[2];

// remoteButton = state - 2, hubPort = substate - 1

MenuState stateMax = Menu1;
MenuState subStateMax = Init;
motorFunction motorFunctionMax = static_cast<motorFunction>(static_cast<int>(motorFunctionLast) - 1);
uint8_t timer_counter = 0;
uint8_t timer_counter_step = 0;

MenuState     state = Init;
MenuState     substate = Init;
hubType       hub = noHub;

void counter_delay (uint32_t d)
{
  timer_counter += d / 50;
  delay (d);
}

// Update Counter for animation
bool update_counter ()
{
  timer_counter ++;
  if (timer_counter > 20)
  {
    timer_counter = 0;
    return true;
  }
  return false;
}

void updateMotorFunction (uint8_t buttonId, uint8_t portId, bool reverse = false)
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
  Serial.println("set function [" + String(static_cast<uint8_t>(buttonId)) + "," + String(static_cast<uint8_t>(portId)) + "]=" +String(function));
}

void controlMotorFuntion (uint8_t id, motorDirection dir, bool buttonPressed)
{
  Serial.println("controlMotorFuntion ID:" + String(id) + " buttonPressed:" + String(buttonPressed));

  int8_t newValue;
  for ( uint8_t i = 0; i <= subStateMax - 1; i++)
  {
    switch (getSettingsFunction(id,i))
    {
      case FullForward:
        Serial.println("FullForward");
        newValue = MotorFunctionFullForward  (getSettingsCurrentValue(id,i), dir, buttonPressed, functionParameters[FullForward].Steps);
        break;
      case FullBackward:
        Serial.println("FullBackward");
        newValue = MotorFunctionFullBackward (getSettingsCurrentValue(id,i), dir, buttonPressed, functionParameters[FullBackward].Steps);
        break;
      case StepForward:
        Serial.println("StepForward");
        newValue = MotorFunctionStepForward  (getSettingsCurrentValue(id,i), dir, buttonPressed, functionParameters[StepForward].Steps);
        break;
      case StepBackward:
        Serial.println("StepBackward");
        newValue = MotorFunctionStepBackward (getSettingsCurrentValue(id,i), dir, buttonPressed, functionParameters[StepBackward].Steps);
        break;
      case StepForward8:
        Serial.println("StepForward8");
        newValue = MotorFunctionStepForward  (getSettingsCurrentValue(id,i), dir, buttonPressed, functionParameters[StepForward8].Steps);
        break;
      case StepBackward8:
        Serial.println("StepBackward8");
        newValue = MotorFunctionStepBackward (getSettingsCurrentValue(id,i), dir, buttonPressed, functionParameters[StepBackward8].Steps);
        break;
      case SteeringForward:
        Serial.println("SteeringForward");
        newValue = MotorFunctionFullForward  (getSettingsCurrentValue(id,i), dir, buttonPressed, functionParameters[SteeringForward].Steps);
        break;
      case SteeringBackward:
        Serial.println("SteeringBackward");
        newValue = MotorFunctionFullBackward (getSettingsCurrentValue(id,i), dir, buttonPressed, functionParameters[SteeringBackward].Steps);
        break;
      default:
        newValue = getSettingsCurrentValue(id,i);
        break;
    }

    if (setSettingsCurrentValue(id,i, newValue))
    {
      Serial.println("set value [" + String(static_cast<uint8_t>(id)) + "," + String(static_cast<uint8_t>(i)) + "]=" +String(newValue));

      if (id < subStateMax)
      {
        if ((getSettingsFunction(id,i) == SteeringForward ||
             getSettingsFunction(id,i) == SteeringBackward) &&
             MotorIsCalibrated (PUport[i])
           )
        {
          setSteeringMotorPosition (i, newValue, functionParameters[getSettingsFunction (id, i)].Steps, &PUHub[0], PUport[i]);
        }
        else
        {
          // Todo: use hubType for second PU Hub
          setSimpleMotorSpeed (i, newValue, functionParameters[getSettingsFunction (id, i)].Steps, &PUHub[0], PUport[i]);
        }
      }

      // Todo: use animation values
      if (newValue == 0)
        display_set (i+1,id+1, 0, 0, 0, true);

      if (newValue > 0)
        display_set (i+1,id+1, 0, 255, 0, true);

      if (newValue < 0)
        display_set (i+1,id+1, 255, 0, 0, true);
    }
  }
}

void updateSubState (bool reverse = false)
{
  display_set (static_cast<uint8_t>(substate), 0, 0, 0, 0, true);

  if (reverse == false)
  {
    if (static_cast<int>(substate) < static_cast<int>(subStateMax))
    {
      substate = static_cast<MenuState>(static_cast<int>(substate) + 1);
    }
    else
    {
      substate = Menu1;
    }
  }
  else
  {
    if (static_cast<int>(substate) > static_cast<int>(Menu1))
    {
      substate = static_cast<MenuState>(static_cast<int>(substate) - 1);
    }
    else
    {
      substate = subStateMax;
    }
  }
  Serial.println("substate" + String(static_cast<uint8_t>(substate)));
  display_set (static_cast<uint8_t>(substate), 0, 255, 255, 255, true);
}

void updateState ()
{
  display_clear ();

  if (static_cast<int>(state) < static_cast<int>(stateMax))
  {
    state = static_cast<MenuState>(static_cast<int>(state) + 1);
  }
  else
  {
    state = Menu1;

    // Do calibration stuff for Servos
    for (int j = 1; j <= subStateMax; j++)
    {
      Serial.println("");
      Serial.print("Sub:" + String (j) + ":");

      for (int i = 2; i <= stateMax; i++)
      {
        Serial.print(String(getSettingsFunction(i-2,j-1)));

        if ((getSettingsFunction(i-2,j-1) == SteeringForward ||
             getSettingsFunction(i-2,j-1) == SteeringBackward)  &&
            !MotorIsCalibrated (PUport[j-1]) &&
            !MotorIsCalibrating (PUport[j-1]))
        {
          Serial.print("*");
          MotorStartCalibration (PUport[j-1]);
          break;
        }
        Serial.print(",");
      }
    }
    Serial.println("");
  }

  if (state != Init && state != Menu1)
  {
    substate = Init;
    updateSubState();
    display_redrawFunctions();
    timer_counter_step = 0;
    counter_delay(250);
  }

  Serial.println("State" + String(static_cast<uint8_t>(state)));
  display_set (0, static_cast<uint8_t>(state) - 1, 255, 255, 255, true);
}

void display_redrawFunctions()
{
  for (int i = 0; i < subStateMax; i++)
  {
    display_setRowArray(i + 1, 1, 4, functionParameters[getSettingsFunction(state - 2,i)].Graphics[functionParameters[getSettingsFunction(state - 2,i)].Steps], true);
  }
}

void configure_remote (uint8_t id)
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

bool connect_remote (uint8_t id)
{
  // 0: 1, 2
  // 1: 3, 4
  display_set(0, id * 2 + 1 , 64, 64, 64);
  display_set(0, id * 2 + 2 , 64, 64, 64, true);
  Remote[id].connectHub();

  if (Remote[id].isConnected())
  {
    Serial.println("Connected to Remote");
    Remote[id].setLedColor(WHITE);
    return true;
  }
  else
  {
    display_set(0, 1, 255, 000, 0);
    display_set(0, 2, 255, 000, 0);
    display_set(0, 3, 255, 000, 0);
    display_set(0, 4, 255, 000, 0, true);
    Serial.println("Failed to connect to Remote");
    return false;
  }
}

void configure_hub (uint8_t id)
{
  HubInitState[id] = Configured;
}

bool connect_hub (uint8_t id)
{
  // 0: 1, 2
  // 1: 3, 4
  display_set(1, 0, 64, 64, 64);
  display_set(2, 0, 64, 64, 64, true);
  PUHub[id].connectHub();

  if (PUHub[id].isConnected())
  {
    HubInitState[id] = Connected;
    Serial.println("Connected to Hub");
    PUHub[id].setLedColor(WHITE);
    return true;
  }
  else
  {
    display_set(1, 0, 255, 000, 0);
    display_set(2, 0, 255, 000, 0);
    display_set(3, 0, 255, 000, 0);
    display_set(4, 0, 255, 000, 0, true);
    Serial.println("Failed to connect to Remote");
    return false;
  }
}

void InitAnimation ()
{
  if (update_counter ())
  {
    if (initAnimationState)
    {
      initAnimationState = false;
      display_set(0, 0, 0, 0, 0, true);
    }
    else
    {
      initAnimationState = true;
      display_set(0, 0, 0, 0, 255, true);
    }
  }
}

// callback function to handle updates of remote buttons
void remoteCallback(void *hub, byte portNumber, DeviceType deviceType, uint8_t *pData)
{
  Lpf2Hub *myRemoteHub = (Lpf2Hub *)hub;

  Serial.print("sensorMessage callback for port: ");
  Serial.println(portNumber, DEC);
  if (deviceType == DeviceType::REMOTE_CONTROL_BUTTON)
  {
    ButtonState buttonState = myRemoteHub->parseRemoteButton(pData);
    Serial.print("Buttonstate: ");
    Serial.println((byte)buttonState, HEX);

    switch (state)
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
          updateMotorFunction (state - 2, substate - 1);
        }
        else if (portNumber == 1 && buttonState == ButtonState::DOWN)
        {
          updateMotorFunction (state - 2, substate - 1, true);
        }
        display_redrawFunctions();
        timer_counter_step = 0;
        counter_delay(250);
        break;
    }
  }
}

void check_motor_calibration ()
{
  for (int i = 0; i < subStateMax; i++)
  {
    if (MotorIsCalibrating(PUport[i]))
    {
      MotorCalibrationStep (&PUHub[0], PUport[i]);
    }
  }
}

// Wrapper for second Remote
void remoteCallback2(void *hub, byte portNumber, DeviceType deviceType, uint8_t *pData)
{
  remoteCallback(hub, portNumber + 2, deviceType, pData);
}

void setup() {
  Serial.begin(115200);

  Remote[0].init();
  PUHub[0].init();

  M5.begin(true, false, true);
  counter_delay(50);

  display_init();
}

void loop()
{
  // connect flow
  if (state == Init)
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
      stateMax = Menu3;
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
      stateMax = Menu5;
    }

    if (PUHub[0].isConnecting())
    {
      if (PUHub[0].getHubType() == HubType::POWERED_UP_HUB)
      {
        PUHub[0].connectHub();
        PUHub[0].setLedColor(GREEN);
        Serial.println("powered up hub connected.");
        PUHub[0].setLedColor(GREEN);
        display_set(1, 0, 0, 255, 0);
        display_set(2, 0, 0, 255, 0, true);

        PUHub[0].activatePortDevice(PUport[0], tachoMotorCallback);
        PUHub[0].activatePortDevice(PUport[1], tachoMotorCallback);

        subStateMax = Menu2;
      }
      if (PUHub[0].getHubType() == HubType::CONTROL_PLUS_HUB)
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

        PUHub[0].activatePortDevice(PUport[0], tachoMotorCallback);
        PUHub[0].activatePortDevice(PUport[1], tachoMotorCallback);
        PUHub[0].activatePortDevice(PUport[2], tachoMotorCallback);
        PUHub[0].activatePortDevice(PUport[3], tachoMotorCallback);

        subStateMax = Menu4;
      }
    }

    if (!Remote[0].isConnected())
    {
      Remote[0].init();
    }

    if (!PUHub[0].isConnected())
    {
      PUHub[0].init();
    }

    if (M5.Btn.wasPressed())
    {
      configure_hub (0);
      configure_remote (0);
      if (stateMax == Menu5)
      {
        configure_remote (1);
      }
      updateState();
    }

    // Animation
    InitAnimation();
  }
  else
  {
    switch (state)
    {
      case Init:
        break;
      case Menu1:
        check_motor_calibration();
        break;
      default:
        check_motor_calibration();
        // Animation
        if (update_counter ())
        {
          timer_counter_step++;
          if (timer_counter_step > functionParameters[getSettingsFunction(state - 2, substate - 1)].Steps)
          {
            timer_counter_step = 0;
          }
          display_setRowArray(substate, 1, 4, functionParameters[getSettingsFunction(state - 2, substate - 1)].Graphics[timer_counter_step], true);
        }
        break;
    }

    if (M5.Btn.wasPressed())
    {
      updateState();
    }

  }
  counter_delay (50);
  M5.update();
}

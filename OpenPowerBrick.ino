/**
   M5Stack Matrix Atom Application to control PoweredUp! and Control+ Devices

   (For now menu system only)
  +
   (c) Copyright 2020 -  Richard Jeske
   Released GPL 2.0 Licence

*/

#include "PoweredUpRemote.h"
#include "PoweredUpHub.h"
#include "M5Atom.h"

PoweredUpRemote Remote[2];
PoweredUpHub PUHub[2];

PoweredUpRemote::Port RemotePortLeft[2] = { PoweredUpRemote::Port::LEFT, PoweredUpRemote::Port::LEFT};
PoweredUpRemote::Port RemotePortRight[2] = { PoweredUpRemote::Port::RIGHT, PoweredUpRemote::Port::RIGHT};

PoweredUpHub::Port PUportA[2] = { PoweredUpHub::Port::A, PoweredUpHub::Port::A };
PoweredUpHub::Port PUportB[2] = { PoweredUpHub::Port::B, PoweredUpHub::Port::B };

#include "menu.h"
#include "motor.h"
#include "motorfunctions.h"

bool initAnimationState = false;
InitState RemoteInitState[2] = {notConnected, notConnected};
InitState HubInitState[2] = {notConnected, notConnected};

typedef struct sRemoteState
{
  bool  RightPressed;
  bool  LeftPressed;
} tRemoteState;

tRemoteState RemoteState[2];

tSettings SettingsMatrix[4][4] = { [0] = {{ FullForward, 4 , 0 }, { No, 0, 0 }, { No, 0, 0 }, { No, 0, 0 }},
  [1] = {{ No, 0, 0 }, { FullForward, 4, 0 }, { No, 0, 0 }, { No, 0, 0 }},
  [2] = {{ No, 0, 0 }, { No, 0, 0 }, { FullForward, 4, 0 }, { No, 0, 0 }},
  [3] = {{ No, 0, 0 }, { No, 0, 0 }, { No, 0, 0 }, { FullForward, 4, 0 }}
};

MenuState stateMax = Menu1;
MenuState subStateMax = Init;
motorFunction motorFunctionMax = static_cast<motorFunction>(static_cast<int>(motorFunctionLast) - 1);
uint8_t timer_counter = 0;
uint8_t timer_counter_step = 0;

MenuState     state = Init;
MenuState     substate = Init;

uint8_t DisBuff[2 + 5 * 5 * 3];

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

void setBuff(const uint8_t Pos, const uint8_t Rdata, const uint8_t Gdata, const uint8_t Bdata, const bool updateBuff = false)
{
  DisBuff[2 + Pos * 3 + 0] = Rdata;
  DisBuff[2 + Pos * 3 + 1] = Gdata;
  DisBuff[2 + Pos * 3 + 2] = Bdata;

  if (updateBuff)
  {
    M5.dis.displaybuff(DisBuff);
  }
}

void setBuffRowArray (const uint8_t Row, const uint8_t Start, const uint8_t Lenght, const uint8_t* Array, const bool updateBuff = false)
{
  uint8_t j = 0;
  for (int i = Start; i < 5; i++)
  {
    if (i < (Start + Lenght))
    {
      setBuff (Row * 5 + i, Array[j++], Array[j++], Array[j++]);
    }
  }

  if (updateBuff)
  {
    M5.dis.displaybuff(DisBuff);
  }
}

void clearBuff(void)
{
  DisBuff[0] = 0x05;
  DisBuff[1] = 0x05;

  for (int i = 0; i < 25; i++)
  {
    setBuff (i, 0, 0, 0);
  }
}

void updateMotorFunction (bool reverse = false)
{
  motorFunction function = SettingsMatrix [state - 2][substate - 1].Function;

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

  Serial.println("motor" + String(static_cast<uint8_t>(substate)));
  setBuffRowArray(substate, 1, 4, functionParameters[function].Graphics[functionParameters[function].Steps], true);
  SettingsMatrix [state - 2][substate - 1].Function = function;
  timer_counter_step = 0;
  counter_delay(250);
}

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

void setMotorSpeed (uint8_t id, int8_t currentValue, int8_t maxValue)
{
  int Speed =  (100 * currentValue) / maxValue;

  Serial.println("setMotorSpeed ID:" + String(id) + " Speed:" + String(currentValue));

  switch (id)
  {
    case 0:
      PUHub[0].setMotorSpeed(PUportA[0], Speed);
      break;
    case 1:
      PUHub[0].setMotorSpeed(PUportB[0], Speed);
      break;
    case 2:
      PUHub[1].setMotorSpeed(PUportA[1], Speed);
      break;
    case 3:
      PUHub[1].setMotorSpeed(PUportB[1], Speed);
      break;
  }
}

void controlMotorFuntion (uint8_t id, motorDirection dir, bool buttonPressed)
{
  Serial.println("controlMotorFuntion ID:" + String(id) + " buttonPressed:" + String(buttonPressed));

  int8_t newValue;
  for ( uint8_t i = 1; i <= subStateMax; i++)
  {
    switch (SettingsMatrix[id][i - 1].Function)
    {
      case FullForward:
        Serial.println("FullForward");
        newValue = MotorFunctionFullForward  (SettingsMatrix[id][i - 1].currentValue, dir, buttonPressed, functionParameters[FullForward].Steps);
        break;
      case FullBackward:
        Serial.println("FullBackward");
        newValue = MotorFunctionFullBackward (SettingsMatrix[id][i - 1].currentValue, dir, buttonPressed, functionParameters[FullBackward].Steps);
        break;
      case StepForward:
        Serial.println("StepForward");
        newValue = MotorFunctionStepForward  (SettingsMatrix[id][i - 1].currentValue, dir, buttonPressed, functionParameters[StepForward].Steps);
        break;
      case StepBackward:
        Serial.println("StepBackward");
        newValue = MotorFunctionStepBackward (SettingsMatrix[id][i - 1].currentValue, dir, buttonPressed, functionParameters[StepBackward].Steps);
        break;
      case StepForward8:
        Serial.println("StepForward8");
        newValue = MotorFunctionStepForward  (SettingsMatrix[id][i - 1].currentValue, dir, buttonPressed, functionParameters[StepForward8].Steps);
        break;
      case StepBackward8:
        Serial.println("StepBackward8");
        newValue = MotorFunctionStepBackward (SettingsMatrix[id][i - 1].currentValue, dir, buttonPressed, functionParameters[StepBackward8].Steps);
        break;
      default:
        newValue = SettingsMatrix[id][i - 1].currentValue;
        break;
    }

    if (newValue != SettingsMatrix[id][i - 1].currentValue)
    {
      SettingsMatrix[id][i - 1].currentValue = newValue;
      setMotorSpeed (i - 1, newValue, functionParameters[SettingsMatrix[id][i - 1].Function].Steps);

      if (newValue == 0)
        setBuff (5 * (1 + id) + i, 0, 0, 0, true);

      if (newValue > 0)
        setBuff (5 * (1 + id) + i, 0, 255, 0, true);

      if (newValue < 0)
        setBuff (5 * (1 + id) + i, 255, 0, 0, true);
    }
  }
}

void updateSubState (bool reverse = false)
{
  setBuff (static_cast<uint8_t>(substate) * 5, 0, 0, 0, true);

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
  setBuff (static_cast<uint8_t>(substate) * 5, 255, 255, 255, true);

  for (int i = 0; i < subStateMax; i++)
  {
    setBuffRowArray(i + 1, 1, 4, functionParameters[SettingsMatrix[state - 2][i].Function].Graphics[functionParameters[SettingsMatrix[state - 2][i].Function].Steps], true);
  }
  timer_counter_step = 0;

  counter_delay(250);
}

void updateState ()
{
  clearBuff ();

  if (static_cast<int>(state) < static_cast<int>(stateMax))
  {
    state = static_cast<MenuState>(static_cast<int>(state) + 1);
  }
  else
  {
    state = Menu1;
  }

  if (state != Init && state != Menu1)
  {
    substate = Init;
    updateSubState ();

    for (int i = 0; i < subStateMax; i++)
    {
      setBuffRowArray(i + 1, 1, 4, functionParameters[SettingsMatrix[state - 2][i].Function].Graphics[functionParameters[SettingsMatrix[state - 2][i].Function].Steps], true);
    }
  }

  Serial.println("State" + String(static_cast<uint8_t>(state)));
  setBuff (static_cast<uint8_t>(state) - 1, 255, 255, 255, true);
}

void configure_remote (uint8_t id)
{
  Remote[id].activateButtonReports();
  Remote[id].activatePortDevice(RemotePortLeft[id], 55);
  Remote[id].activatePortDevice(RemotePortRight[id], 55);
  RemoteInitState[id] = Configured;
}

bool connect_remote (uint8_t id)
{
  // 0: 1, 2
  // 1: 3, 4
  setBuff(id * 2 + 1 , 64, 64, 64);
  setBuff(id * 2 + 2 , 64, 64, 64, true);
  Remote[id].connectHub();

  if (Remote[id].isConnected())
  {
    RemoteInitState[id] = Connected;
    Serial.println("Connected to Remote");
    Remote[id].setLedColor(WHITE);
    return true;
  }
  else
  {
    setBuff(1, 255, 000, 0);
    setBuff(2, 255, 000, 0);
    setBuff(3, 255, 000, 0);
    setBuff(4, 255, 000, 0, true);
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
  setBuff(id * 5 + 5 , 64, 64, 64);
  setBuff(id * 5 + 10 , 64, 64, 64, true);
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
    setBuff(5, 255, 000, 0);
    setBuff(10, 255, 000, 0);
    setBuff(15, 255, 000, 0);
    setBuff(20, 255, 000, 0, true);
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
      setBuff(0, 0, 0, 0, true);
    }
    else
    {
      initAnimationState = true;
      setBuff(0, 0, 0, 255, true);
    }
  }
}

void setup() {
  Serial.begin(115200);

  Remote[0].init();

  M5.begin(true, false, true);
  counter_delay(50);
  clearBuff();
  M5.dis.displaybuff(DisBuff);
}

void loop()
{
  // connect flow
  if (state == Init)
  {
    if (RemoteInitState[0] == Connected)
    {
      configure_remote (0);
      Remote[0].setLedColor(GREEN);
      setBuff(1, 0, 255, 0);
      setBuff(2, 0, 255, 0, true);
      stateMax = Menu3;
      RemoteInitState[0] = Finished;
      PUHub[0].init();
    }

    if (Remote[0].isConnecting() && RemoteInitState[0] == notConnected)
    {
      connect_remote (0);
    }

    if (RemoteInitState[0] == Finished && HubInitState[0] == Connected)
    {
      configure_hub (0);
      PUHub[0].setLedColor(GREEN);
      setBuff(5, 0, 255, 0);
      setBuff(10, 0, 255, 0, true);
      subStateMax = Menu2;
    }

    if (RemoteInitState[0] == Finished && HubInitState[0] == notConnected)
    {
      connect_hub (0);
    }

    if (M5.Btn.wasPressed())
    {
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
        if (Remote[0].isLeftRemoteStopButtonPressed())
        {
          if (RemoteState[0].LeftPressed == false)
            controlMotorFuntion (0, Stop, true);
          RemoteState[0].LeftPressed = true;
        }
        else if (Remote[0].isLeftRemoteDownButtonPressed())
        {
          if (RemoteState[0].LeftPressed == false)
            controlMotorFuntion (0, Backward, true);
          RemoteState[0].LeftPressed = true;
        }
        else if (Remote[0].isLeftRemoteUpButtonPressed())
        {
          if (RemoteState[0].LeftPressed == false)
            controlMotorFuntion (0, Forward, true);
          RemoteState[0].LeftPressed = true;
        }
        else if (RemoteState[0].LeftPressed)
        {
          if (RemoteState[0].LeftPressed == true)
            controlMotorFuntion (0, Backward, false);
          RemoteState[0].LeftPressed = false;
        }

        if (Remote[0].isRightRemoteStopButtonPressed())
        {
          if (RemoteState[0].RightPressed == false)
            controlMotorFuntion (1, Stop, true);
          RemoteState[0].RightPressed = true;
        }
        else if (Remote[0].isRightRemoteDownButtonPressed())
        {
          if (RemoteState[0].RightPressed == false)
            controlMotorFuntion (1, Backward, true);
          RemoteState[0].RightPressed = true;
        }
        else if (Remote[0].isRightRemoteUpButtonPressed())
        {
          if (RemoteState[0].RightPressed == false)
            controlMotorFuntion (1, Forward, true);
          RemoteState[0].RightPressed = true;
        }
        else if (RemoteState[0].RightPressed)
        {
          if (RemoteState[0].RightPressed == true)
            controlMotorFuntion (1, Backward, false);
          RemoteState[0].RightPressed = false;
        }

        break;
      default:
        if (Remote[0].isLeftRemoteUpButtonPressed())
        {
          updateSubState (true);
        }
        else if (Remote[0].isLeftRemoteDownButtonPressed())
        {
          updateSubState ();
        }
        else if (Remote[0].isRightRemoteUpButtonPressed())
        {
          updateMotorFunction ();
        }
        else if (Remote[0].isRightRemoteDownButtonPressed())
        {
          updateMotorFunction (true);
        }

        // Animation
        if (update_counter ())
        {
          timer_counter_step++;
          if (timer_counter_step > functionParameters[SettingsMatrix[state - 2][substate - 1].Function].Steps)
          {
            timer_counter_step = 0;
          }
          setBuffRowArray(substate, 1, 4, functionParameters[SettingsMatrix[state - 2][substate - 1].Function].Graphics[timer_counter_step], true);
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

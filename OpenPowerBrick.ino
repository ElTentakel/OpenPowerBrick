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
hubType       hub = noHub;

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

  if (id < subStateMax)
  {
    PUHub[0].setBasicMotorSpeed(PUport[id], Speed);
      // Todo: use hubType for second PU Hub
      // PUHub[1].setBasicMotorSpeed(PUport[id], Speed);
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

      // Todo: use animation values
      if (newValue == 0)
        setBuff (5 * i + id + 1, 0, 0, 0, true);

      if (newValue > 0)
        setBuff (5 * i + id + 1, 0, 255, 0, true);

      if (newValue < 0)
        setBuff (5 * i + id + 1, 255, 0, 0, true);
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
  Remote[id].activatePortDevice(RemotePortLeft[id], remoteCallback);
  Remote[id].activatePortDevice(RemotePortRight[id], remoteCallback);
  Remote[id].setLedColor(WHITE);
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
          updateMotorFunction ();
        else if (portNumber == 1 && buttonState == ButtonState::DOWN)
          updateMotorFunction (true); 
        break;
    }
  }
}

void setup() {
  Serial.begin(115200);

  Remote[0].init();
  PUHub[0].init();

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
          Serial.println("Remote connected.");
        }
      }
      Remote[0].setLedColor(GREEN);
      setBuff(1, 0, 255, 0);
      setBuff(2, 0, 255, 0, true);
      stateMax = Menu3;
      RemoteInitState[0] = Finished;
    }

    if (PUHub[0].isConnecting())
    {
      if (PUHub[0].getHubType() == HubType::POWERED_UP_HUB)
      {
        PUHub[0].connectHub();
        PUHub[0].setLedColor(GREEN);
        Serial.println("powered up hub connected.");
        PUHub[0].setLedColor(GREEN);
        setBuff(5, 0, 255, 0);
        setBuff(10, 0, 255, 0, true);
        subStateMax = Menu2;
      }
      if (PUHub[0].getHubType() == HubType::CONTROL_PLUS_HUB)
      {
        PUHub[0].connectHub();
        PUHub[0].setLedColor(GREEN);
        Serial.println("control plus hub connected.");
        PUHub[0].setLedColor(GREEN);
        setBuff(5, 0, 255, 0);
        setBuff(10, 0, 255, 0);
        setBuff(15, 0, 255, 0);
        setBuff(20, 0, 255, 0, true);
        subStateMax = Menu4;

        // Switch Port Configuration to Control+
        PUport[0] = (byte)ControlPlusHubPort::A;
        PUport[1] = (byte)ControlPlusHubPort::B;
        PUport[2] = (byte)ControlPlusHubPort::C;
        PUport[3] = (byte)ControlPlusHubPort::D;
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
        break;
      default:
      
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

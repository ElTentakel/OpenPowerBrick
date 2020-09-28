/**
 * M5Stack Matrix Atom Application to control PoweredUp! and Control+ Devices
 *
 * (For now menu system only)
 +
 * (c) Copyright 2020 -  Richard Jeske
 * Released GPL 2.0 Licence
 * 
 */

#include "PoweredUpRemote.h"
#include "M5Atom.h"

PoweredUpRemote myRemote;

PoweredUpRemote::Port _portLeft = PoweredUpRemote::Port::LEFT;
PoweredUpRemote::Port _portRight = PoweredUpRemote::Port::RIGHT;

enum MenuState      { Init = 0,
                      Menu1 = 1,
                      Menu2 = 2,
                      Menu3 = 3,
                      Menu4 = 4,
                      Menu5 = 5,
                      MenuLast
                    };

enum motorFunction  { No = 0,
                      FullForward = 1,
                      FullBackward = 2,
                      StepForward = 3,
                      StepBackward = 4,
                      motorFunctionLast
                    };

enum motorMode      { Off = 0,
                      Push,
                      PushRamp,
                      ToggleStep
                    };

enum motorDirection { Forward,
                      Backward
                    };

typedef struct sFunctionParameters {
      motorFunction   Function;
      motorMode       Mode;
      motorDirection  Direction;
      int8_t          Steps;
      uint8_t         Graphics [5][12];
} tFunctionParameters;

typedef struct sSettings {
      motorFunction   Function;
      uint8_t         Strength;
} tSettings;

tSettings SettingsMatrix[4][4] = { [0] = {{ FullForward, 4 }, { No, 0 }, { No, 0 }, { No, 0 }},
                                    [1] = {{ No, 0 }, { FullForward, 4 }, { No, 0 }, { No, 0 }},
                                    [2] = {{ No, 0 }, { No, 0 }, { FullForward, 4 }, { No, 0 }},
                                    [3] = {{ No, 0 }, { No, 0 }, { No, 0 }, { FullForward, 4 }}};

tFunctionParameters functionParameters [motorFunctionLast] = 
{
  { No,             Off, Forward,  0,       { [0] ={  0,  0,  0,   0,  0,  0,   0,  0,  0,    0,  0,  0 }}},
  { FullForward,   Push, Forward,  1,       { [0] ={  0,  0,  0,   0,  0,  0,   0,  0,  0,    0,  0,  0 },
                                              [1] ={  0,255,  0,   0,255,  0,   0,255,  0,    0,255,  0 }}},                                            
  { FullBackward,  Push, Backward, 1,       { [0] ={  0,  0,  0,   0,  0,  0,   0,  0,  0,    0,  0,  0 },
                                              [1] ={255,  0,  0, 255,  0,  0, 255,  0,  0,  255,  0,  0 }}},
  { StepForward,   ToggleStep, Forward , 4, { [0] ={  0,  0,  0,   0,  0,  0,   0,  0,  0,    0,  0,  0 },
                                              [1] ={  0, 64,  0,   0,  0,  0,   0,  0,  0,    0,  0,  0 },
                                              [2] ={  0, 64,  0,   0,128,  0,   0,  0,  0,    0,  0,  0 },
                                              [3] ={  0, 64,  0,   0,128,  0,   0,192,  0,    0,  0,  0 },
                                              [4] ={  0, 64,  0,   0,128,  0,   0,192,  0,    0,255,  0 }}},
  { StepBackward,   ToggleStep, Backward, 4, {[0] ={  0,  0,  0,   0,  0,  0,   0,  0,  0,    0,  0,  0 },
                                              [1] ={  64, 0,  0,  0,  0,   0,   0,  0,  0,    0,  0,  0 },
                                              [2] ={  64, 0,  0, 128,  0,  0,   0,  0,  0,    0,  0,  0 },
                                              [3] ={  64, 0,  0, 128,  0,  0, 192,  0,  0,    0,  0,  0 },
                                              [4] ={  64, 0,  0, 128,  0,  0, 192,  0,  0,  255,  0,  0 }}}
};

MenuState stateMax = Menu5;
MenuState subStateMax = Menu4;
motorFunction motorFunctionMax = StepBackward;
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

void setBuff(uint8_t Pos, uint8_t Rdata, uint8_t Gdata, uint8_t Bdata, bool updateBuff = false)
{
    DisBuff[2 + Pos * 3 + 0] = Rdata;
    DisBuff[2 + Pos * 3 + 1] = Gdata;
    DisBuff[2 + Pos * 3 + 2] = Bdata;

    if (updateBuff)
    {
        M5.dis.displaybuff(DisBuff);
    }
} 

void setBuffRowArray (uint8_t Row, uint8_t Start, uint8_t Lenght, uint8_t* Array, bool updateBuff = false)
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
    setBuff (i,0,0,0);
  }
}

void updateMotorFunction (bool reverse = false)
{
  motorFunction function = SettingsMatrix [state-2][substate-1].Function;
  
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
  SettingsMatrix [state-2][substate-1].Function = function;
  timer_counter_step = 0;
  counter_delay(250);
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
    setBuffRowArray(i + 1, 1, 4, functionParameters[SettingsMatrix[state-2][i].Function].Graphics[functionParameters[SettingsMatrix[state-2][i].Function].Steps], true);
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
      setBuffRowArray(i + 1, 1, 4, functionParameters[SettingsMatrix[state-2][i].Function].Graphics[functionParameters[SettingsMatrix[state-2][i].Function].Steps], true);
    }
  }

  Serial.println("State" + String(static_cast<uint8_t>(state)));
  setBuff (static_cast<uint8_t>(state)-1, 255, 255, 255, true); 
}

void setup() {
  Serial.begin(115200);
  myRemote.init();

  M5.begin(true, false, true);
  counter_delay(50);
  clearBuff();
  M5.dis.displaybuff(DisBuff);
} 

void loop() {

  // connect flow
  if (state == Init)
  {
    if (myRemote.isConnecting()) {
      setBuff(0,128,128,0, true);
      myRemote.connectHub();
      if (myRemote.isConnected()) {
        setBuff(0,0,255,0);
        Serial.println("Connected to Remote");
        myRemote.setLedColor(GREEN);
      } else {
        setBuff(0,255,000,0);
        Serial.println("Failed to connect to Remote");
      }
    }

    if (myRemote.isConnected()) {
      Serial.println("System is initialized");
      updateState();
      myRemote.activateButtonReports(); 
      myRemote.activatePortDevice(_portLeft, 55);
      myRemote.activatePortDevice(_portRight, 55);
      myRemote.setLedColor(WHITE);
    }
  }
  else
  {
    switch (state)
    {
      case Init:
        break;
      case Menu1:
          if (myRemote.isLeftRemoteUpButtonPressed())
          {
          }
          else if (myRemote.isLeftRemoteDownButtonPressed())
          {
          }
          else if (myRemote.isRightRemoteUpButtonPressed())
          {
          }
          else if (myRemote.isRightRemoteDownButtonPressed())
          {
          }
          break;
      default:
          if (myRemote.isLeftRemoteUpButtonPressed())
          {
            updateSubState (true);
          }
          else if (myRemote.isLeftRemoteDownButtonPressed())
          {
            updateSubState ();
          }
          else if (myRemote.isRightRemoteUpButtonPressed())
          {
            updateMotorFunction ();
          }
          else if (myRemote.isRightRemoteDownButtonPressed())
          {
            updateMotorFunction (true);
          }

          // Animation
          if (update_counter ())
          {
            timer_counter_step++;
            if (timer_counter_step > functionParameters[SettingsMatrix[state-2][substate-1].Function].Steps)
            {
              timer_counter_step = 0;
            }
            setBuffRowArray(substate, 1, 4, functionParameters[SettingsMatrix[state-2][substate-1].Function].Graphics[timer_counter_step], true);
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

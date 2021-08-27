#include "menu.h"
#include "display.h" 
#include <stdint.h>

// remoteButton = state - 2, hubPort = substate - 1
static MenuState stateMax = Menu1;
static MenuState subStateMax = Init;
static MenuState state = Init;
static MenuState substate = Init;

void updateSubState (bool reverse)
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
  display_set (static_cast<uint8_t>(substate), 0, 255, 255, 255, true);
}

uint8_t getMaxPortId()
{
  return subStateMax - 1;  
}

uint8_t getMaxRemoteButton()
{
  return stateMax - 2;  
}

MenuState getMaxSubState()
{
  return subStateMax;  
}

MenuState getMaxState()
{
  return stateMax;  
}

MenuState getSubState()
{
  return substate;  
}

MenuState getState()
{
  return state;  
}

void setStateMax(MenuState state)
{
  stateMax = state;
}

void setSubStateMax(MenuState state)
{
  subStateMax = state;
}

uint8_t getPortId()
{
  return substate - 1;  
}

uint8_t getRemoteButton()
{
  return state - 2;  
}

void resetSubState()
{
  substate = Init;
}

// Intermediate until hub is abstracted
// true on reset
bool updateAndResetState()
{
  bool ret = false;
  if (static_cast<int>(state) < static_cast<int>(stateMax))
  {
    state = static_cast<MenuState>(static_cast<int>(state) + 1);
    ret = false;
  }
  else
  {
    state = Menu1;
    ret = true;
  }
  return ret;
}

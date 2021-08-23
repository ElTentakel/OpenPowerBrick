#ifndef MENU_H
#define MENU_H

#include <stdint.h>

enum InitState      { notConnected = 0,
                      Connected = 1,
                      Configured = 2,
                      Finished = 3
};

enum MenuState      { Init = 0,
                      Menu1 = 1,
                      Menu2 = 2,
                      Menu3 = 3,
                      Menu4 = 4,
                      Menu5 = 5,
                      MenuLast
                    };


void updateSubState (bool reverse = false);
uint8_t getMaxPortId();
uint8_t getMaxRemoteButton();
uint8_t getPortId();
uint8_t getRemoteButton();

MenuState getMaxSubState();
MenuState getMaxState();
MenuState getSubState();
MenuState getState();

void setStateMax(MenuState state);
void setSubStateMax(MenuState state);

void resetSubState();
bool updateAndResetState();

#endif

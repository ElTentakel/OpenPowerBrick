#ifndef HUB_H
#define HUB_H

#include "Lpf2Hub.h"
#include "motor.h"
#include <stdint.h> 

enum hubType      { noHub = 0,
                    PoweredUpHub = 1,
                    ControlPlusHub = 2
};

Lpf2Hub *getHub(uint8_t port);
hubType getHubType(uint8_t hub);
byte getHubPort(uint8_t port);
void tryConnectHubs();
void configureHub (uint8_t id);
void connectedHub(uint8_t i);

#endif

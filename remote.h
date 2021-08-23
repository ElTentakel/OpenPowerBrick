#ifndef REMOTE_H
#define REMOTE_H

#include "Lpf2Hub.h"
#include <stdint.h> 

Lpf2Hub *getRemote(uint8_t port);
void tryConnectRemotes ();
void configureRemote (uint8_t id);
void connectedRemote(uint8_t i);

#endif

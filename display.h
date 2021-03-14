#ifndef DISPLAY_H
#define DISPLAY_H

#include "M5Atom.h"
#include <stdint.h> 

void display_init(void);
void display_set(const uint8_t Pos, const uint8_t Rdata, const uint8_t Gdata, const uint8_t Bdata, const bool updateBuff = false);
void display_setRowArray (const uint8_t Row, const uint8_t Start, const uint8_t Lenght, const uint8_t* Array, const bool updateBuff = false);
void display_clear(void);

#endif

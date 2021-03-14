#include "display.h"
uint8_t DisBuff[2 + 5 * 5 * 3];

void display_init(void)
{
  display_clear();
  M5.dis.displaybuff(DisBuff);
}

void display_set(const uint8_t Pos, const uint8_t Rdata, const uint8_t Gdata, const uint8_t Bdata, const bool updateBuff)
{
  DisBuff[2 + Pos * 3 + 0] = Rdata;
  DisBuff[2 + Pos * 3 + 1] = Gdata;
  DisBuff[2 + Pos * 3 + 2] = Bdata;

  if (updateBuff)
  {
    M5.dis.displaybuff(DisBuff);
  }
}

void display_setRowArray (const uint8_t Row, const uint8_t Start, const uint8_t Lenght, const uint8_t* Array, const bool updateBuff)
{
  uint8_t j = 0;
  for (int i = Start; i < 5; i++)
  {
    if (i < (Start + Lenght))
    {
      display_set (Row * 5 + i, Array[j++], Array[j++], Array[j++]);
    }
  }

  if (updateBuff)
  {
    M5.dis.displaybuff(DisBuff);
  }
}

void display_clear(void)
{
  DisBuff[0] = 0x05;
  DisBuff[1] = 0x05;

  for (int i = 0; i < 25; i++)
  {
    display_set (i, 0, 0, 0);
  }
}

#include "display.h"
#include "motorfunctions.h"
#include "menu.h"
#include "settings.h"

uint8_t display_timerCounter = 0;
uint8_t display_timerCounter_step = 0;
bool initAnimationState = false;

uint8_t DisBuff[2 + 5 * 5 * 3];

void display_init(void)
{
  display_clear();
  M5.dis.displaybuff(DisBuff);
}

void display_set(const uint8_t row, const uint8_t col, const uint8_t Rdata, const uint8_t Gdata, const uint8_t Bdata, const bool updateBuff)
{
  const uint8_t pos = row * 5 + col;
  DisBuff[2 + pos * 3 + 0] = Rdata;
  DisBuff[2 + pos * 3 + 1] = Gdata;
  DisBuff[2 + pos * 3 + 2] = Bdata;

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
      display_set (Row, i, Array[j++], Array[j++], Array[j++]);
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

  for (int i = 0; i < 5; i++)
  {  
    for (int j = 0; j < 5; j++)
    {
      display_set (i, j, 0, 0, 0);
    }
  }
}

void display_counterDelay (uint32_t d)
{
  display_timerCounter += d / 50;
  delay (d);
}

// Update Counter for animation
bool display_updateAnimationCounter ()
{
  display_timerCounter ++;
  if (display_timerCounter > 20)
  {
    display_timerCounter = 0;
    return true;
  }
  return false;
}

void display_resetAnimationCounter ()
{  
  display_timerCounter_step = 0;
}

void display_initAnimation ()
{
  if (display_updateAnimationCounter ())
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

void display_drawFunctionAnimation()
{
  if (display_updateAnimationCounter ())
  {
    display_timerCounter_step++;
    if (display_timerCounter_step > functionParameters[getSettingsFunction(getRemoteButton(), getPortId())].Steps)
    {
      display_resetAnimationCounter();
    }
    display_setRowArray(getSubState(), 1, 4, functionParameters[getSettingsFunction(getRemoteButton(), getPortId())].Graphics[display_timerCounter_step], true);
  }
}

void display_redrawFunctions()
{
  for (int i = 0; i <= getMaxPortId(); i++)
  {
    display_setRowArray(i + 1, 1, 4, functionParameters[getSettingsFunction(getRemoteButton(),i)].Graphics[functionParameters[getSettingsFunction(getRemoteButton(),i)].Steps], true);
  }
}

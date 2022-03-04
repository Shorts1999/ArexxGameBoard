#ifndef __AREXX_CLOCK_H
#define __AREXX_CLOCK_H

#include "Arexx_GameBoard.h"

extern const uint8_t clockBckGrnd[18] PROGMEM;
extern const uint32_t digitalNumbers[5] PROGMEM;

void drawDigit(uint8_t, uint8_t, uint8_t, uint16_t);
void displayClock();

void clockBackground();
#endif //__AREXX_CLOCK_H
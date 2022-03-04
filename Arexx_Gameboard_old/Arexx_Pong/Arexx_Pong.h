#ifndef __AREXX_PONG_H
#define __AREXX_PONG_H

#include "Arexx_GameBoard.h"

void mainPong();

void printScore(uint8_t Player1Score, uint8_t Player2Score);

void pongOver(uint8_t Player1Score, uint8_t Player2Score);

void pongBackground();
#endif //#ifdef __AREXX_PONG_H
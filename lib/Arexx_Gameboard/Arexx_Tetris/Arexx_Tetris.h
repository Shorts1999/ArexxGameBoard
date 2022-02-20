#ifndef __AREXX_TETRIS_H
#define __AREXX_TETRIS_H

#include "Arexx_GameBoard.h"


//Variables for a shape and rotation: in the shape map every four arguments is a shape and every one of its rotations.
extern int8_t shape;
extern int8_t rotation;

void mainTetris();

void drawShape();
void saveShape();
void removeShapeFromBuffer();

uint8_t leftCollisionCheck();
uint8_t rightCollisionCheck();
uint8_t bottomCollisionCheck();
uint8_t rotationCheck(int8_t desiredRotation);

uint8_t deathCheck();

void drawBoard();
void lineDetect();

void tetrisBackground();

#endif //#ifdef __AREXX_TETRIS_H
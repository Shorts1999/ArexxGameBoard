#ifndef __AREXX_GAMEBOARD_H
#define __AREXX_GAMEBOARD_H

//used libraries:
#include <Arduino.h>
#include <Adafruit_GFX.h> //Library for Graphics primitives for the NeoPixel and the scorepanel
#include <Adafruit_NeoMatrix.h> //Library for the Matrix object
#include <Adafruit_NeoPixel.h> //Base library for using NeoPixel/ws2812b LEDs
#include <PS2X_lib.h> //Library for PS2 controller
#include <Max72xxPanel.h> //Library for 8x8 Dot matrices using MAX72xx driver IC's
#include <DSRTCLib.h> //Library for DS1339 RTC module

//Amount of selectable items in the main menu (zero indexed)
#define gameAmount 2

//the score in PONG where the game is won:
#define PONGSCORE 20
//the width of the bar in PONG
#define BARWIDTH 3

//pins for PS2 controller
#define PS2_CMD   7
#define PS2_DAT   8
#define PS2_CLK   9
#define PS2_SEL   10

//pins for the RTC module Interrupt
#define INT_PIN 2
#define INT_NUMBER 0


// Color definitions
#define BLACK    0x0000 //turns a pixel off
#define BLUE     0x001F
#define RED      0xF800
#define GREEN    0x07E0
#define DARKGREEN 0x0362
#define CYAN     0x07FF
#define MAGENTA  0xF81F
#define YELLOW   0xFFE0
#define ORANGE   matrix.Color(0xff, 0xA5, 0x00)
#define WHITE    0xFFFF
#define BOARDCOLOUR matrix.Color(0x00, 0xCE,0xBF) //The colour a pixel will turn into once it has had a bottom collision in Tetris


//Starting position:
extern int8_t Xpos;
extern int8_t Ypos; //start of the board is at y=0, shape will start above the board and scroll inside

//A global variable in which to store the currently selected shape in Tetris:
extern uint16_t shapeMap;

//The amount of time before the block moves down 1 space. gets shorter proporionally to the score of the current round. (game gets faster).
extern uint16_t downDelay;

//variables for the millis() function.
extern unsigned long newTime;
extern unsigned long oldTime;

extern uint32_t Score;
extern unsigned int value;
extern int error;
extern uint8_t pendingSelect;

//definitions for the main LED-matrix/display:
#define LEDPIN 6
#define TILEWIDTH 10      //the amount of horizontal pixels in a single panel
#define TILEHEIGHT 20     //the amount of vertical pixels in a single panel
#define PANELTYPE  (NEO_MATRIX_BOTTOM + NEO_MATRIX_LEFT + NEO_MATRIX_COLUMNS + NEO_MATRIX_ZIGZAG)
#define XTILES 1          //the amount of "tiles" (matrices) in the horizontal direction
#define YTILES 1          //the amount of tiles in the vertical direction
#define TILEORIENTATION (NEO_TILE_TOP   + NEO_TILE_LEFT   + NEO_TILE_COLUMNS   + NEO_TILE_PROGRESSIVE)
extern Adafruit_NeoMatrix matrix;

//definitions for the scoreboard:
#define SCOREPIN 4
#define SCOREPANELAMOUNT 8
extern Max72xxPanel scorePanel;

//an array to store all the already placed blocks in
extern uint16_t Board[TILEHEIGHT + 5];

//Creating the controller object:
extern PS2X ps2x;

//creating the RTC object:
extern DS1339 RTC;

//Allows call to "resetFunc()" to reset the arduino via software
extern void(* resetFunc) (void);//declare reset function at address 0

#endif //#ifdef  __AREXX_GAMEBOARD_H
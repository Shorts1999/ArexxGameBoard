/*
   Multiple games for on Arexx Game Board
   Author: Sjors Smit
   Date: June 2nd 2020
   AREXX ENGINEERING
*/
#include <Arduino.h>
#include <Wire.h>
#include <SPI.h>
#include "math.h"

#include "Arexx_GameBoard.h"
#include "Arexx_Tetris/Arexx_Tetris.h"
#include "Arexx_Pong/Arexx_Pong.h"
#include "Arexx_Clock/Arexx_Clock.h"

/*
   Backgrounds. These are shown during the menu
*/


void setup() {
  Serial.begin(9600); //Initialize the Serial port
  delay(200); //give modules some time to boot up before initialization

  for (int i = 0; i < TILEHEIGHT + 4; i++) { //Setup the Board array with the empty field, bars on the side to indicate the walls for collision detection
    Board[i] =  0x8000 | (0x8000 >> (TILEWIDTH + 1));
  }
  //Also add one full row at the bottom (will be one pixel underneath the actual display)
  Board[TILEHEIGHT + 4] = 0xffff;

  matrix.begin(); //Initializing the NeoPixel display
  matrix.clear();
  matrix.show();
  matrix.setBrightness(180); //set Brightness of the main display

  //Set up controller
  while (!ps2x.config_gamepad(PS2_CLK, PS2_CMD, PS2_SEL, PS2_DAT, false, false)) {
  }

  scorePanel.setIntensity(2);   //set Brightness of the score display
  for (int i = 0; i < 8; i++) { //Rotate all
    scorePanel.setRotation(i, 1);
  }

  scorePanel.print("AREXX");
  scorePanel.write();

  pinMode(INT_PIN, INPUT_PULLUP);
  RTC.start(); //Initialize the RTC module

  oldTime = millis(); //set the timer to the current time
  newTime = oldTime;

  matrix.clear(); //empty the display
  randomSeed(analogRead(A0)); //Set a place for the random number generator using a floating pin
  delay(10);
  shape = random(0, 7); //get a "fake" random number to avoid starting at 0 every time, since random seemed to always boot at 0, even after setting a seed

}


void loop() { //main menu / game select screen.
  delay(5);
  ps2x.read_gamepad(); //Added delay before and after reading gamepad to improve stability.
  delay(5);
  if (ps2x.ButtonPressed(PSB_CROSS)) { //Pressing the Cross button cycles the game selection forwards
    if (pendingSelect >= gameAmount) {
      pendingSelect = 0;
    }
    else pendingSelect++;
  }
  if (ps2x.ButtonPressed(PSB_CIRCLE)) { //Pressing the Circle button cycles the game selection backwards
    if (pendingSelect == 0) {
      pendingSelect = gameAmount;
    }
    else pendingSelect--;
  }
  switch (pendingSelect) {
    case 0:
      matrix.setCursor(3, 3);
      matrix.clear();
      matrix.print(pendingSelect);
      clockBackground();
      matrix.show();
      if (ps2x.ButtonPressed(PSB_START)) {
        matrix.clear();
        displayClock();
      }
      break;
    case 1:
      matrix.setCursor(3, 3);
      matrix.clear();
      matrix.print(pendingSelect);
      tetrisBackground();
      matrix.show();
      if (ps2x.ButtonPressed(PSB_START)) { //pressing the start button launches the currently selected game (TETRIS)
        matrix.clear();
        mainTetris();
      }
      break;
    case 2:
      matrix.setCursor(3, 3);
      matrix.clear();
      matrix.print(pendingSelect);
      pongBackground();
      matrix.show();
      if (ps2x.ButtonPressed(PSB_START)) { //Pressing the start button launches the currently selected game (PONG)
        matrix.clear();
        mainPong();
      }
      break;
  }
}
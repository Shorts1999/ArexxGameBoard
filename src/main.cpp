/**
 * @file main.cpp
 * 
 * @author Sjors Smit
 * @brief Firmware for the Arexx Game Board v2.0 using an ESP32
 * @version 2.0
 * @date 2022-02-21
 */

#include <Arduino.h>
#include <SPI.h>
#include <math.h>

#include <Arexx_GameBoard.h>

//definitions for the main LED-matrix/display:
#define MATRIX_PIN 6
#define MATRIX_WIDTH 10      //the amount of horizontal pixels in a single panel
#define MATRIX_HEIGHT 20     //the amount of vertical pixels in a single panel
#define PANELTYPE  (NEO_MATRIX_BOTTOM + NEO_MATRIX_LEFT + NEO_MATRIX_COLUMNS + NEO_MATRIX_ZIGZAG)
#define TILEORIENTATION (NEO_TILE_TOP   + NEO_TILE_LEFT   + NEO_TILE_COLUMNS   + NEO_TILE_PROGRESSIVE)
ArexxGameBoard gameBoard(MATRIX_PIN, MATRIX_WIDTH, MATRIX_HEIGHT, PANELTYPE, NEO_GRB + NEO_KHZ800, 6);

void setup(){
    Serial.begin(115200);
    gameBoard.begin();
    gameBoard.setBrightness(10);
    gameBoard.show();

}

void loop(){

}
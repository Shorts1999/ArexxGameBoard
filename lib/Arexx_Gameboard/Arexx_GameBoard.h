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
#include <SPI.h>

//Amount of selectable items in the main menu (zero indexed)
#define gameAmount 2



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

/**
 * @brief Main "Gameboard" class, inheriting from the Adafruit matrix to include all required functions for writing and manipulating the display
 * 
 */
class ArexxGameBoard: public Adafruit_NeoMatrix{
public:
    /**
     * @brief Construct a new Arexx Game Board object
     * 
     * @param pin the pin connected to the display
     * @param width the amount of horizontal pixels
     * @param height the amount of vertical pixels
     * @param orientation "Adafruit_NeoMatrix" orientation style
     * @param ledType "Adafruit_NeoPixel" pixeltype
     * @param scorePin pin connected to the ChipSelect of the Score Panel
     */
    ArexxGameBoard(uint8_t pin , int width , int height , uint8_t orientation, neoPixelType ledType, uint8_t scorePin);

    /**
     * @brief Construct a new Arexx Game Board object
     * 
     * @param pin the pin connected to the display
     * @param width the amount of horizontal pixels
     * @param height the amount of vertical pixels
     * @param orientation "Adafruit_NeoMatrix" orientation style
     * @param ledType "Adafruit_NeoPixel" pixeltype
     * @param scoreComms SPI class to run the scoreboard on
     */
    ArexxGameBoard(uint8_t pin , int width , int height , uint8_t orientation, neoPixelType ledType, SPIClass scoreComms);

    void update();

    int8_t xPos;
    int8_t yPos;


private:
    
    unsigned long newTime;
    unsigned long oldTime;

    uint32_t score;
    uint16_t value;
    int16_t error;
    uint8_t selector;

    uint8_t BoardMemory[];

    Max72xxPanel scorePanel;
};



//Allows call to "resetFunc()" to reset the arduino via software
extern void(* resetFunc) (void);//declare reset function at address 0


#endif //#ifdef  __AREXX_GAMEBOARD_H
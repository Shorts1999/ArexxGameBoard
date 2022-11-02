#ifndef ___GAMEBOARD_H
#define ___GAMEBOARD_H

//used libraries:
#include <Arduino.h>
#include <Wire.h>
#include <FastLED.h>
// #include <Max72xxPanel.h> //Library for 8x8 Dot matrices using MAX72xx driver IC's
#include <SPI.h>

//Amount of selectable items in the main menu (zero indexed)
#define gameAmount 2

#ifndef MATRIXPIN
#define MATRIXPIN 13
#endif

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
class GameBoard {
public:
    /**
     * @brief Construct a new  Game Board object
     *
     * @param pin the pin connected to the display
     * @param width the amount of horizontal pixels
     * @param height the amount of vertical pixels
     * @param orientation "Adafruit_NeoMatrix" orientation style
     * @param ledType "Adafruit_NeoPixel" pixeltype
     * @param scorePin pin connected to the ChipSelect of the Score Panel
     */
    GameBoard(uint8_t pin, const uint32_t width, const uint32_t height, bool useRows);

    enum Orientation{
        Rows=true,
        Columns=false,
    };

    void (GameBoard::*setPixelFormat)(uint16_t x, uint16_t y, uint32_t colour);
    void setPixel(uint16_t, uint16_t, uint32_t);
    uint32_t (GameBoard::*getPixelFormat)(int16_t x, int16_t y);
    uint32_t getPixel(int16_t x, int16_t y);

    void moveCursor(int16_t x, int16_t y);
    void moveCursor(int16_t x, int16_t y, uint32_t colour);
    int8_t width();
    int8_t height();

    //column/row functions:
    void setPixelColumns(uint16_t x, uint16_t y, uint32_t colour);
    void setPixelRows(uint16_t x, uint16_t y, uint32_t colour);
    uint32_t getPixelColumns(int16_t, int16_t);
    uint32_t getPixelRows(int16_t, int16_t);

    //Graphic primitives:
    void drawLineVert(int y1, int y2, uint32_t colour);
    void drawLineHor(int x1, int x2, uint32_t colour);
    void drawRectangle(int x1, int y1, int x2, int y2, uint32_t colour, bool fill = false);

    int16_t xPos;
    int16_t yPos;
    CRGB *mMatrix;

private:

    unsigned long newTime;
    unsigned long oldTime;

    uint32_t score;
    uint16_t value;
    int16_t error;
    uint8_t selector;
    uint8_t mPin;

    int8_t mWidth;
    int8_t mHeight;

    //Max72xxPanel scorePanel;
};



//Allows call to "resetFunc()" to reset the arduino via software
extern void(*resetFunc) (void);//declare reset function at address 0


#endif //#ifdef  ___GAMEBOARD_H
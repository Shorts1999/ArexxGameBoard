#include "GameBoard.h"

const uint8_t BoardMemoryOverhead = 0;

GameBoard::GameBoard(uint8_t pin, uint32_t width, uint32_t height, uint8_t scorePin) {
    // Initialise position and time values as 0
    xPos = 0;
    yPos = 0;

    newTime = 0;
    oldTime = 0;

    score = 0;
    value = 0;
    error = 0;
    selector = 0;

    mWidth = width;
    mHeight = height;
    mMatrix = new CRGB[width * height];
    FastLED.addLeds<WS2812B, MATRIXPIN>(mMatrix, width * height);
    FastLED.clear();    //Empty all pixels in the buffer (needed with heap-allocated CRGB buffer)
}

void GameBoard::setPixel(uint16_t x, uint16_t y, uint32_t colour) {
    if ((x >= mWidth) || (y >= mHeight)) { return; } //Return if out of bounds
    //In zigzag pattern, switch between rows depending on height:
    if (y % 2) mMatrix[((mWidth - x - 1)) + (y * mWidth)] = colour;
    else mMatrix[x + (y * mWidth)] = colour;
}


//Gettters
uint8_t GameBoard::width() {
    return mWidth;
}
uint8_t GameBoard::height() {
    return mHeight;
}
uint32_t GameBoard::getPixel(int x, int y) {
    if (x > mWidth || y > mHeight) return 0; //return if out of bounds

    if (y % 2) {
        uint8_t red = mMatrix[((mWidth - x - 1)) + (y * mWidth)].red;
        uint8_t green = mMatrix[((mWidth - x - 1)) + (y * mWidth)].green;
        uint8_t blue = mMatrix[((mWidth - x - 1)) + (y * mWidth)].blue;

        uint32_t colour = red << 16 | green << 8 | blue;
        return colour;

    }
    else {
        uint8_t red = mMatrix[x + (y * mWidth)].red;
        uint8_t green = mMatrix[x + (y * mWidth)].green;
        uint8_t blue = mMatrix[x + (y * mWidth)].blue;

        uint32_t colour = red << 16 | green << 8 | blue;
        return colour;
    }
}

void GameBoard::moveCursor(int16_t x, int16_t y, uint32_t colour = 0) {
    setPixel(xPos, yPos, 0); //Clear current cursor position
    //check for valid movement on x-axis:
    if (xPos + x >= mWidth) xPos = mWidth - 1;
    else if (xPos + x < 0) xPos = 0;
    else xPos += x;
    //check for valid movement on y-axis
    if (yPos + y >= mHeight) yPos = mHeight - 1;
    else if (yPos + y < 0) yPos = 0;
    else yPos += y;

    setPixel(xPos, yPos, colour);
}
void GameBoard::moveCursor(int16_t x, int16_t y) {
    //check for valid movement on x-axis:
    if (xPos + x >= mWidth) xPos = mWidth - 1;
    else if (xPos + x < 0) xPos = 0;
    else xPos += x;
    //check for valid movement on y-axis
    if (yPos + y >= mHeight) yPos = mHeight - 1;
    else if (yPos + y < 0) yPos = 0;
    else yPos += y;
}
void (*resetFunc)(void);
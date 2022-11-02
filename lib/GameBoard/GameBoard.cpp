#include "GameBoard.h"

const uint8_t BoardMemoryOverhead = 0;

GameBoard::GameBoard(uint8_t pin, uint32_t width, uint32_t height, bool useRows=true) {
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

    //Add drawing handlers depending on the display begin made of rows or columns
    setPixelFormat = (useRows == Orientation::Rows ? (&GameBoard::setPixelRows) : (&GameBoard::setPixelColumns));
    getPixelFormat = (useRows == Orientation::Rows ? (&GameBoard::getPixelRows) : (&GameBoard::getPixelColumns));
}

void GameBoard::setPixel(uint16_t x, uint16_t y, uint32_t colour){
    (this->*setPixelFormat)(x,y,colour);
}

//Gettters
int8_t GameBoard::width() {
    return mWidth;
}
int8_t GameBoard::height() {
    return mHeight;
}

void GameBoard::setPixelRows(uint16_t x, uint16_t y, uint32_t colour) {
    if ((x >= mWidth) || (y >= mHeight)) { return; } //Return if out of bounds
    //In zigzag pattern, switch between rows depending on height:
    if (y % 2) mMatrix[((mWidth - x - 1)) + (y * mWidth)] = colour;
    else mMatrix[x + (y * mWidth)] = colour;
}

void GameBoard::setPixelColumns(uint16_t x, uint16_t y, uint32_t colour){
    if((x>=mWidth) || (y>=mHeight)) return; //Return if out of bounds

    if(x%2) mMatrix[ ( (mHeight - y - 1) + (x*mHeight))] = colour;
    else mMatrix[y + (x*mHeight)] = colour;
}

uint32_t GameBoard::getPixelRows(int16_t x, int16_t y) {
    if (x > mWidth || y > mHeight) return 0; //return if out of bounds

    //Check for uneven rows, iterate in other direction (mWidth-x-1) due to zigzag pattern 
    if (y % 2) {
        uint8_t red = mMatrix[((mWidth - x - 1)) + (y * mWidth)].red;
        uint8_t green = mMatrix[((mWidth - x - 1)) + (y * mWidth)].green;
        uint8_t blue = mMatrix[((mWidth - x - 1)) + (y * mWidth)].blue;

        uint32_t colour = red << 16 | green << 8 | blue;
        return colour;

    }
    //no else needed due to return in if branch
    uint8_t red = mMatrix[x + (y * mWidth)].red;
    uint8_t green = mMatrix[x + (y * mWidth)].green;
    uint8_t blue = mMatrix[x + (y * mWidth)].blue;

    uint32_t colour = red << 16 | green << 8 | blue;
    return colour;
}

uint32_t GameBoard::getPixelColumns(int16_t x, int16_t y) {
    if (x > mWidth || y > mHeight) return 0; //return if out of bounds

    //Check for uneven rows, iterate in other direction (mWidth-x-1) due to zigzag pattern 
    if (x % 2) {
        uint8_t red = mMatrix[((mHeight - y - 1)) + (x * mHeight)].red;
        uint8_t green = mMatrix[((mHeight - y - 1)) + (x * mHeight)].green;
        uint8_t blue = mMatrix[((mHeight - y - 1)) + (x * mHeight)].blue;

        uint32_t colour = red << 16 | green << 8 | blue;
        return colour;

    }
    //no else needed due to return in if branch
    uint8_t red = mMatrix[y + (x*mHeight)].red;
    uint8_t green = mMatrix[y + (x*mHeight)].green;
    uint8_t blue = mMatrix[y + (x*mHeight)].blue;

    uint32_t colour = red << 16 | green << 8 | blue;
    return colour;
}

uint32_t GameBoard::getPixel(int16_t x, int16_t y){
    return (this->*getPixelFormat)(x,y);
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
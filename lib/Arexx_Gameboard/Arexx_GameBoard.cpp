#include "Arexx_GameBoard.h"

const uint8_t BoardMemoryOverhead = 5;

ArexxGameBoard::ArexxGameBoard(uint8_t pin, int width, int height, uint8_t orientation, neoPixelType ledType, uint8_t scorePin) : Adafruit_NeoMatrix(width, height, pin, orientation, ledType),
                                                                                                                                  scorePanel(scorePin, 5, 0)
{
    // Initialise position and time values as 0
    xPos = 0;
    yPos = 0;

    newTime = 0;
    oldTime = 0;

    score = 0;
    value = 0;
    error = 0;
    selector = 0;

    // Allocate 5 extra rows of board memory for "off screen storage" (used in tetris for placing blocks before they appear)
    BoardMemory[width * (height + BoardMemoryOverhead)];
}

/**
 * @brief Draw all data in BoardMemory to the display
 *
 */
void ArexxGameBoard::update()
{
    for (uint8_t yIterator = 0; yIterator < height(); yIterator++){
        for (uint8_t xIterator = 0; xIterator < width(); xIterator++){
            drawPixel(xIterator, yIterator, BoardMemory[(xIterator * yIterator) + yIterator + (BoardMemoryOverhead * yIterator)]);
        }
    }
    show();
}

void (*resetFunc)(void);
#include <Tetris/Tetris.h>

#define COLISSION_OK 0
#define COLISSION_OCCURRED 1

//Tetris shapes:
//Every shape has 4 possible rotations, every "Bitmap" of a 4x4 shape is stored in a single 16-bit number
const uint16_t Shapes[28] = { //Consideration: store these in PROGMEM as well?
  0b0000000000110110, //S-Shape
  0b0000010001100010,
  0b0000000000110110,
  0b0000010001100010,

  0b0000000011000110, //Z-Shape
  0b0000001001100100,
  0b0000000011000110,
  0b0000001001100100,

  0b0000010001000110, //L-shape
  0b0000000000010111,
  0b0000011000100010,
  0b0000000011101000,

  0b0000001000100110, //J shape
  0b0000000001110001,
  0b0000011001000100,
  0b0000000010001110,

  0b0000000001100110, //O-Shape
  0b0000000001100110,
  0b0000000001100110,
  0b0000000001100110,

  0b0000000001001110, //T-shape
  0b0000010001100100,
  0b0000000011100100,
  0b0000010011000100,

  0b0100010001000100, //I-Shape
  0b0000000011110000,
  0b0100010001000100,
  0b0000000011110000
};



const uint32_t shapeColours[7] = { CRGB::Blue, CRGB::Green, CRGB::Magenta,CRGB::Aqua, CRGB::Yellow, CRGB::Red, CRGB::Orange };

Tetris::Tetris(GameBoard &gameboard, AsyncWebServer &server) : mGameBoard(gameboard), mWebServer(server) {

}
int8_t Tetris::rotate(bool antiClockwise) {
    DEBUG_PRINT("Current shape %d, current Rotation %d\n", currentShape, currentRotation);
    removeShapeFromBuffer();
    if (antiClockwise) {
        if (currentRotation == 0) currentRotation = 4;
        currentRotation--;
    }
    else {
        currentRotation++;
        if (currentRotation >= 4) currentRotation = 0;   //4 possible rotations, if over 3, loop back to 0
    }
    if (!rotationCollisionCheck()) {
        drawShape();
        return 0;
    };
    if (currentRotation == 0) currentRotation = 3;   //Set back to last shape
    else currentRotation--;

    drawShape();//draw it back to the buffer
    return 0;
}
int8_t Tetris::moveLeft() {
    removeShapeFromBuffer();
    if (!leftCollisionCheck()) mGameBoard.xPos--;
    drawShape();
    //FastLED.show();
    DEBUG_PRINT("Current Position: x:%d y:%d\n", mGameBoard.xPos, mGameBoard.yPos);
    return 0;
}
int8_t Tetris::moveRight() {
    removeShapeFromBuffer();
    if (!rightCollisionCheck()) mGameBoard.xPos++;
    drawShape();
    //FastLED.show();
    DEBUG_PRINT("Current Position: x:%d y:%d\n", mGameBoard.xPos, mGameBoard.yPos);
    return 0;
}

int8_t Tetris::moveDown() {
    removeShapeFromBuffer();
    uint8_t collision = bottomCollisionCheck();
    if (collision == 1) {
        drawShape();
        return 0;
    }
    if (collision == 2) {
        gameOver();
    }
    mGameBoard.yPos--;
    drawShape();
    DEBUG_PRINT("Current Position: x:%d y:%d\n", mGameBoard.xPos, mGameBoard.yPos);
    return 1;
}

void Tetris::newShape() {
    removeShapeFromBuffer();
    currentShape++;
    if (currentShape >= 7) currentShape = 0;
    drawShape();
    return;
}


//Check if there is a wall or already existing block on the board to the left of the current shape
uint8_t Tetris::leftCollisionCheck() {
    uint16_t shapeMap = Shapes[(currentShape * 4) + currentRotation];
    for (uint8_t y = 0; y < 4; y++) {
        if (mGameBoard.yPos + y >= mGameBoard.height()) return 0;
        for (uint8_t x = 0; x < 4; x++) {
            // if (((shapeMap << (y * 4)) & (0x8000 >> i)) && ((mGameBoard.yPos + y + 4] << (mGameBoard.xPos)) & (0x8000 >> x))) {
            //     return 1;
            // }
            if (((shapeMap << (y * 4)) & (0x8000 >> x)) && (
                /*Wall check: */(((mGameBoard.xPos - 1) + x) < 0)
                /*Pixel check: */ || (mGameBoard.getPixel(mGameBoard.xPos + x - 1, mGameBoard.yPos + y) != 0))) {
                DEBUG_PRINT("Collision at x:%d y:%d", mGameBoard.xPos + x, mGameBoard.yPos + y);
                return 1;
            }
        }
    }
    return 0;
}

//Check if there is a wall or already existing block on the board to the right of the current shape
uint8_t Tetris::rightCollisionCheck() {
    uint16_t shapeMap = Shapes[(currentShape * 4) + currentRotation];
    for (uint8_t y = 0; y < 4; y++) {
        if (mGameBoard.yPos + y >= mGameBoard.height()) return 0;
        for (uint8_t x = 0; x < 4; x++) {
            //Check +4 pixels from the cursor. making you one pixel into the wall compared to the left-most pixel in the shape
            if (((shapeMap << (y * 4)) & (0x1000 << x)) && (
                /*wall collision:*/((mGameBoard.xPos + 4) - x) >= mGameBoard.width()
                /*pixel collision:*/ || mGameBoard.getPixel(mGameBoard.xPos + 4 - x, mGameBoard.yPos + y) != 0)) {
                DEBUG_PRINT("Collision at x:%d y:%d", mGameBoard.xPos + x, mGameBoard.yPos + y);
                return 1;
            }
        }
    }
    return 0;
}

//Check if the bottom has been reached or if there is another block directly underneath, making the block stop falling and lock in position
uint8_t Tetris::bottomCollisionCheck() {
    uint16_t shapeMap = Shapes[(currentShape * 4) + currentRotation];
    for (uint8_t y = 0; y < 4; y++) {
        if (mGameBoard.yPos + y >= mGameBoard.height()) return 0; //Ignore collisions when out of bounds
        for (uint8_t x = 0; x < 4; x++) {
            if (((shapeMap << (y * 4)) & (0x8000 >> x)) && (
                /*Wall check: */(((mGameBoard.yPos - 1) + y) < 0)
                /*Pixel check: */ || mGameBoard.getPixel(mGameBoard.xPos + x, mGameBoard.yPos + y - 1) != 0)) {
                if ((mGameBoard.yPos + y == mGameBoard.height() - 1)) {
                    DEBUG_PRINT("Bottom collision at %d\n", mGameBoard.yPos + y); return 2; //Collide on top of screen
                }
                return 1; //normal collision

            }
        }
    }
    return 0;
}

//Check if a rotation is valid:
uint8_t Tetris::rotationCollisionCheck() {
    uint16_t shapeMape = Shapes[(currentShape * 4) + currentRotation];
    for (uint8_t y = 0; y < 4; y++) {
        if (mGameBoard.yPos + y >= mGameBoard.height()) return 0;
        for (uint8_t x = 0; x < 4; x++) {
            if ((shapeMape << (y * 4)) & (0x8000 >> x) && (
                /*Wall check:*/(((mGameBoard.xPos + x) >= mGameBoard.width()) || ((mGameBoard.xPos + x) < 0))
                /*Bottom check:*/ || (mGameBoard.yPos + y < 0)
                /*Pixel check:*/ || mGameBoard.getPixel(mGameBoard.xPos + x, mGameBoard.yPos + y))) {
                DEBUG_PRINT("Rotationcollision at x:%d y:%d GBx: %d GBy: %d\n", x, y, mGameBoard.xPos, mGameBoard.yPos);
                return 1;
            }
        }
    }
    return 0;
}

//Draw the current shape, in its proper rotation, on the board:
void Tetris::drawShape() {
    int selection = currentRotation + currentShape * 4; //Every fourth argument in Shapes[] is a new block shape, and the ones inbetween are the same shape in different rotations
    uint16_t shapeMap = Shapes[selection]; //Let variable "Shapemap" equal the currently selected shape and rotation...would it be more efficient to just place the "selection" calculation inside the square brackets here?
    for (int8_t iy = 0; iy < 4; iy++) {
        for (int8_t ix = 0; ix < 4; ix++) {
            //Use bitshifts to draw the entire shape on the board, one pixel at a time. (scanning horizontally from left the right, then top to bottom). Function only draws a pixel when there is a 1 in the bitmap. allowing transparency
            if (shapeMap & (0x8000 >> (ix + (iy * 4)))) {
                mGameBoard.setPixel(mGameBoard.xPos + ix, mGameBoard.yPos + iy, shapeColours[currentShape]); //Draw the selected pixel of the current shape on the board, if the current position in the shapemap copntains a 1.
            }
        }
    }
    FastLED.show();
}

void Tetris::removeShapeFromBuffer() { //Drawing pixels in "black" to essentially remove them from the display.
    int selection = currentRotation + (currentShape * 4);
    uint16_t shapeMap = Shapes[selection];
    for (int8_t iy = 0; iy < 4; iy++) {
        for (int8_t ix = 0; ix < 4; ix++) {
            if (shapeMap & (0x8000 >> (ix + (iy * 4)))) {
                mGameBoard.setPixel(mGameBoard.xPos + ix, mGameBoard.yPos + iy, 0);
            }
        }
    }
}

uint16_t Tetris::lineDetect() {
    for (int8_t y = mGameBoard.yPos; y < mGameBoard.yPos + 4; y++) {
        if (y >= mGameBoard.height()) break;
        DEBUG_PRINT("Checking line %d\n", y);
        for (uint8_t x = 0; x < mGameBoard.width(); x++) {
            if (y < 0) break;
            if (mGameBoard.getPixel(x, y) == 0) {
                DEBUG_PRINT("Missing pixel at x:%d y:%d\n", x, y);
                break;
            }
            if (x == mGameBoard.width() - 1) {
                DEBUG_PRINT("FULL LINE DETECTED AT ROW %d!\n", y);
                clearLine(y);
                lineCount++;
                DEBUG_PRINT("SCORE: %d lines", lineCount);
                y--;
            }
        }
    }
    return 0;
}

void Tetris::clearLine(uint8_t lineNumber) {
    for (uint8_t y = lineNumber; y < (mGameBoard.height() - 1); y++) {
        for (uint8_t x = 0; x < mGameBoard.width(); x++) {
            mGameBoard.setPixel(x, y, mGameBoard.getPixel(x, y + 1));
        }
    }
    for (uint8_t x = 0; x < mGameBoard.width(); x++) {
        mGameBoard.setPixel(x, mGameBoard.height() - 1, 0);
    }
}

void Tetris::saveShape() {
    lineDetect();
    mGameBoard.xPos = (mGameBoard.width() / 2) - 2; mGameBoard.yPos = mGameBoard.height() - 1;
    currentShape = random(7); currentRotation = 0;
    drawShape();
}

void Tetris::run() {
    bool running = true;
    currentShape = random(7);
    currentRotation = 0;
    mGameBoard.xPos = (mGameBoard.width() / 2) - 2; mGameBoard.yPos = mGameBoard.height() - 1;
    while (running) {
        for (uint8_t dropCounter = 0; dropCounter < totalDropCount; dropCounter++) {
            delay(downDelay);
        }
        if (!moveDown()) {
            saveShape();
            downDelay = 100;
        }

    }
}

void Tetris::speedDrop() {
    downDelay = 2;
}

void Tetris::gameOver() {
    DEBUG_PRINT("GAME OVER");
    while (1) { delay(1000); }
}
#include <Tetris/Tetris.h>


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

uint8_t shapeSelection = 0;

const uint32_t shapeColours[7] = { CRGB::Blue, CRGB::Green, CRGB::Magenta,CRGB::Aqua, CRGB::Yellow, CRGB::Red, CRGB::Orange };

Tetris::Tetris(GameBoard &gameboard, AsyncWebServer &server) : mGameBoard(gameboard), mWebServer(server) {

}
int8_t Tetris::rotate() {
    return 0;
}
int8_t Tetris::moveLeft() {
    removeShapeFromBuffer(shapeSelection, 0);
    if (!leftCollisionCheck(shapeSelection, 0)) mGameBoard.xPos--;
    drawShape(shapeSelection, 0);
    FastLED.show();
    return 0;
}
int8_t Tetris::moveRight() {
    removeShapeFromBuffer(shapeSelection, 0);
    if (!rightCollisionCheck(shapeSelection, 0)) mGameBoard.xPos++;
    drawShape(shapeSelection, 0);
    FastLED.show();
    return 0;
}


//Check if there is a wall or already existing block on the board to the left of the current shape
uint8_t Tetris::leftCollisionCheck(uint8_t shape, uint8_t rotation) {
    uint16_t shapeMap = Shapes[(shape * 4) + rotation];
    for (uint8_t y = 0; y < 4; y++) {
        for (uint8_t x = 0; x < 4; x++) {
            // if (((shapeMap << (y * 4)) & (0x8000 >> i)) && ((mGameBoard.yPos + y + 4] << (mGameBoard.xPos)) & (0x8000 >> x))) {
            //     return 1;
            // }
            if (((shapeMap << (y * 4)) & (0x8000 >> x)) && ((mGameBoard.xPos - 1) + x) < 0) {
                return 1;
            }
        }
    }
    return 0;
}

//Check if there is a wall or already existing block on the board to the right of the current shape
uint8_t Tetris::rightCollisionCheck(uint8_t shape, uint8_t rotation) {
    uint16_t shapeMap = Shapes[(shape * 4) + rotation];
    for (uint8_t y = 0; y < 4; y++) {
        for (uint8_t x = 0; x < 4; x++) {
            //Check +5 pixels from the cursor. making you one pixel into the wall compared to the left-most pixel in the shape
            if (((shapeMap << (y * 4)) & (0x1000 << x)) && ((mGameBoard.xPos + 5) - x) > mGameBoard.width()) {
                return 1;
            }
        }
    }
    return 0;
}

//Check if the bottom has been reached or if there is another block directly underneath, making the block stop falling and lock in position
uint8_t Tetris::bottomCollisionCheck(uint8_t shape, uint8_t rotation) {
    uint16_t shapeMap = Shapes[(shape * 4) + rotation];
    for (uint8_t y = 0; y < 4; y++) {
        for (uint8_t x = 0; x < 4; x++) {
            if (((shapeMap << (y * 4)) & (0x8000 >> x)) && ((mGameBoard.yPos - 1) + y) < 0) {
                return 1;
            }
        }
    }
    return 0;
}


//Draw the current shape, in its proper rotation, on the board:
void Tetris::drawShape(uint8_t shape, uint8_t rotation) {
    int selection = rotation + shape * 4; //Every fourth argument in Shapes[] is a new block shape, and the ones inbetween are the same shape in different rotations
    uint16_t shapeMap = Shapes[selection]; //Let variable "Shapemap" equal the currently selected shape and rotation...would it be more efficient to just place the "selection" calculation inside the square brackets here?
    for (int8_t iy = 0; iy < 4; iy++) {
        for (int8_t ix = 0; ix < 4; ix++) {
            //Use bitshifts to draw the entire shape on the board, one pixel at a time. (scanning horizontally from left the right, then top to bottom). Function only draws a pixel when there is a 1 in the bitmap. allowing transparency
            if (shapeMap & (0x8000 >> (ix + (iy * 4)))) {
                mGameBoard.setPixel(mGameBoard.xPos + ix, mGameBoard.yPos + iy, shapeColours[shape]); //Draw the selected pixel of the current shape on the board, if the current position in the shapemap copntains a 1.
            }
        }
    }
}
void Tetris::removeShapeFromBuffer(uint8_t shape, uint8_t rotation) { //Drawing pixels in "black" to essentially remove them from the display.
    int selection = rotation + (shape * 4);
    uint16_t shapeMap = Shapes[selection];
    for (int8_t iy = 0; iy < 4; iy++) {
        for (int8_t ix = 0; ix < 4; ix++) {
            if (shapeMap & (0x8000 >> (ix + (iy * 4)))) {
                mGameBoard.setPixel(mGameBoard.xPos + ix, mGameBoard.yPos + iy, 0);
            }
        }
    }
}

/*
   Multiple games for on Arexx Game Board
   Author: Sjors Smit
   Date: June 2nd 2020
   AREXX ENGINEERING
*/

//used libraries:
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
int8_t Xpos = 3;
int8_t Ypos = - 4; //start of the board is at y=0, shape will start above the board and scroll inside

//Variables for a shape and rotation: in the shape map every four arguments is a shape and every one of its rotations.
int8_t shape = 0;
int8_t rotation = 0;
//A global variable in which to store the currently selected shape in Tetris:
uint16_t shapeMap;

//The amount of time before the block moves down 1 space. gets shorter proporionally to the score of the current round. (game gets faster).
uint16_t downDelay = 200;

//variables for the millis() function.
unsigned long newTime;
unsigned long oldTime;

uint32_t Score = 0;
unsigned int value = 0;
int error = 0;
uint8_t pendingSelect = 0;

//definitions for the main LED-matrix/display:
#define LEDPIN 6
#define TILEWIDTH 10      //the amount of horizontal pixels in a single panel
#define TILEHEIGHT 20     //the amount of vertical pixels in a single panel
#define PANELTYPE  (NEO_MATRIX_BOTTOM + NEO_MATRIX_LEFT + NEO_MATRIX_COLUMNS + NEO_MATRIX_ZIGZAG)
#define XTILES 1          //the amount of "tiles" (matrices) in the horizontal direction
#define YTILES 1          //the amount of tiles in the vertical direction
#define TILEORIENTATION (NEO_TILE_TOP   + NEO_TILE_LEFT   + NEO_TILE_COLUMNS   + NEO_TILE_PROGRESSIVE)
Adafruit_NeoMatrix matrix = Adafruit_NeoMatrix(TILEWIDTH, TILEHEIGHT, XTILES, YTILES, LEDPIN, PANELTYPE + TILEORIENTATION, NEO_GRB + NEO_KHZ800);

//definitions for the scoreboard:
#define SCOREPIN 4
#define SCOREPANELAMOUNT 8
Max72xxPanel scorePanel = Max72xxPanel(SCOREPIN, SCOREPANELAMOUNT, 1);

//Creating the controller object:
PS2X ps2x;

//creating the RTC object:
DS1339 RTC = DS1339(INT_PIN, INT_NUMBER);

//shape definitions:
//main image for the analog clock:
const uint8_t clockBckgrnd[18] PROGMEM = {
  0x1F, 0x00,
  0x20, 0x80,
  0x40, 0x40,
  0x40, 0x40,
  0x44, 0x40,
  0x40, 0x40,
  0x40, 0x40,
  0x20, 0x80,
  0x1f, 0x00
};
//7-segment numbers for "digital" clock display:
const uint32_t digitalNumbers[5] PROGMEM = { //To save space, all numbers are stored together in a single 5x32-bits array. these are later decoded into the individual numbers
  0b11100111111110111110011111111100,
  0b10100100100110110010000110110100,
  0b10100111111111111111100111111100,
  0b10100110000100100110100110100100,
  0b11100111111100111111100111100100
};

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

//Give every shape a different colour
const uint16_t ShapeColours[7] = {BLUE, GREEN, MAGENTA, DARKGREEN, YELLOW, RED, ORANGE};

//an array to store all the already placed blocks in
uint16_t Board[TILEHEIGHT + 5];

//Allows call to "resetFunc()" to reset the arduino via software
void(* resetFunc) (void) = 0;//declare reset function at address 0

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



//Functions:

/***************************************************************************************/
/*************************@@@@**@******@@@@***@@@@**@**@********************************/
/************************@******@*****@****@*@******@*@*********************************/
/************************@******@*****@****@*@******@@**********************************/
/************************@******@*****@****@*@******@*@*********************************/
/*************************@@@@**@@@@***@@@@***@@@@**@**@********************************/
/***************************************************************************************/
void displayClock() {
  bool mode = 0; //Mode for either setting or displaying the clock
  matrix.clear();
  float minutes = 0;
  float Hours = 0;
  float setMinutes = 0;
  float setHours = 0;
  int8_t minuteX;
  int8_t minuteY;
  int8_t hourX;
  int8_t hourY;
  for (;;) { //loop forever (until reset)
    delay(5);
    ps2x.read_gamepad();
    delay(5);
    if (ps2x.ButtonPressed(PSB_SELECT)) {
      resetFunc(); //reset on Select Press
    }
    switch (mode) {
      case 0: //Display Time Mode
        if (ps2x.ButtonPressed(PSB_CROSS)) {
          mode = 1;
        }
        matrix.drawBitmap(0, 0, clockBckgrnd,  10, 9, RED);
        RTC.readTime(); //Update the local registers from the module
        minutes = RTC.getMinutes(); //Read the minutes from the local register
        minuteX = round((4 * sin((minutes / 30) * M_PI)));
        minuteY = round((4 * cos((minutes / 30) * M_PI)));
        Hours = RTC.getHours(); //Read the Hours from the local register
        drawDigit(((int)Hours / 10), 0, 10, GREEN);
        drawDigit(((int)Hours % 10), 4, 10, GREEN);
        drawDigit(((int)minutes / 10), 3, 15, BLUE);
        drawDigit(((int)minutes % 10), 7, 15, BLUE);
        if (Hours > 12) Hours -= 12; //set to 12 hour value for analog clock
        hourX = round((2 * sin((Hours / 6) * M_PI)));
        hourY = round((2 * cos((Hours / 6) * M_PI)));
        matrix.drawLine(5, 4, 5 + minuteX, 4 - minuteY, BLUE);
        matrix.drawLine(5, 4, 5 + hourX, 4 - hourY, GREEN);
        matrix.drawPixel(5, 4, RED);
        matrix.show();
        delay(100);
        matrix.clear();
        break;
      case 1: //Set Time Mode
        if (ps2x.ButtonPressed(PSB_TRIANGLE)) { //press triangle to increment the minute by 1
          setMinutes += 1;
          if (setMinutes >= 60) setMinutes = 0;
        }
        else if (ps2x.ButtonPressed(PSB_R1)) { //Press R1 to increment the minute by 5
          setMinutes += 5;
          if (setMinutes >= 60) setMinutes = 0;
        }
        if (ps2x.ButtonPressed(PSB_SQUARE)) { //Press square to increment the hour by 1
          setHours += 1;
          if (setHours >= 24) setHours = 0;
        }
        if (ps2x.ButtonPressed(PSB_START)) { //pressing start will save the changes to the RTC
          RTC.setMinutes(setMinutes);
          RTC.setHours(setHours);
          RTC.writeTime();
          mode = 0;
        }
        else if (ps2x.ButtonPressed(PSB_CIRCLE)) mode = 0; //Press circle to cancel the set-time operation

        float hours12 = setHours;
        if (hours12 >= 12) {
          hours12 -= 12;
        }
        //Display the current time-set variables on the clock
        matrix.drawBitmap(0, 0, clockBckgrnd,  10, 9, YELLOW);
        minuteX = round((4 * sin((setMinutes / 30) * M_PI)));
        minuteY = round((4 * cos((setMinutes / 30) * M_PI)));
        hourX = round((2 * sin((hours12 / 6) * M_PI)));
        hourY = round((2 * cos((hours12 / 6) * M_PI)));
        drawDigit(((int)setHours / 10), 0, 10, GREEN);
        drawDigit(((int)setHours % 10), 4, 10, GREEN);
        drawDigit(((int)setMinutes / 10), 3, 15, BLUE);
        drawDigit(((int)setMinutes % 10), 7, 15, BLUE);
        matrix.drawLine(5, 4, 5 + minuteX, 4 - minuteY, BLUE);
        matrix.drawLine(5, 4, 5 + hourX, 4 - hourY, GREEN);
        matrix.drawPixel(5, 4, RED);
        matrix.show();
        delay(100);
        matrix.clear();
        break;
    }
  }
}

void drawDigit(uint8_t digit, uint8_t x, uint8_t y, uint16_t colour) {
  uint8_t digitMap[5];
  for (uint8_t i = 0; i < 5; i++) {
    digitMap[i] = ((pgm_read_dword_near(digitalNumbers + i) << (digit * 3)) >> 24);
  }
  matrix.drawBitmap(x, y, digitMap, 3, 5, colour);
}

/***************************************************************************************/
/********************@@@@@@@**@@@@@@*@@@@@@@*@@@@***@@@***@@@@***************************/
/***********************@*****@*********@****@***@***@***@*******************************/
/***********************@*****@@@@@*****@****@@@@****@****@@@****************************/
/***********************@*****@*********@****@**@****@*******@***************************/
/***********************@*****@@@@@@****@****@***@**@@@***@@@****************************/
/***************************************************************************************/
//(taken from te main loop of the Tetris-only version)
void mainTetris() {
  for (;;) { //Loop forever. processor will reboot to return to main menu.
    Ypos = -4; //Setting X and Y position to the top center of the screen
    Xpos = 3;
    shape = random(0, 7); //select a random shape
    shapeMap = Shapes[shape * 4];
    int startNewCycle = 0;
    delay(100);
    oldTime = newTime;
    while (!startNewCycle) {
      delay(5);
      ps2x.read_gamepad(); //Read which buttons are pressed
      delay(5);
      if (ps2x.ButtonPressed(PSB_PAD_DOWN)) { //Pressing the down button will make the block fall down a lot faster.
        downDelay = 10;
      }
      if (ps2x.ButtonPressed(PSB_PAD_LEFT)) { //Pressing the left pad will attempt to move the block to the left, but fails if there is something obstructing the movement
        if ((!leftCollisionCheck() && (Xpos >= 0))) { //When Xpos=-1 the program can't compute this because of a negative bit-shift. but when x=-1, you are always at the left edge anyway so just disabled the shift there.
          removeShapeFromBuffer();
          Xpos--;
        }
      }
      if (ps2x.ButtonPressed(PSB_PAD_RIGHT)) {//Pressing the right pad will attempt to move the block to the right, but fails if there is something obstructing the movement
        if (!rightCollisionCheck()) { //See if there is space to move before changing the position
          removeShapeFromBuffer();
          Xpos++;
        }
      }
      if (ps2x.ButtonPressed(PSB_CROSS)) { //When cross is pressed, the block will attempt to rotate clockwise, if there is a block or wall obstructing the rotated shape, no rotation will be executed
        removeShapeFromBuffer();
        int8_t newRotation = rotation - 1; //Setting the new rotation in a test variable to determine whether a rotation is possible.
        if (newRotation < 0) {
          newRotation = 3;
        }
        if (!rotationCheck(newRotation)) {
          rotation = newRotation; //If rotation is possible, change the "real" rotation value.
        }
      }
      if (ps2x.ButtonPressed(PSB_CIRCLE)) { //When circle is pressed, the block will attempt to rotate counterclockwise, if there is a block or wall obstructing the rotated shape, no rotation will be executed
        removeShapeFromBuffer();
        int8_t newRotation = rotation + 1;
        if (newRotation > 3) {
          newRotation = 0;
        }
        if (!rotationCheck(newRotation)) {
          rotation = newRotation;
        }
      }
      if (ps2x.ButtonPressed(PSB_SELECT)) { //When the select button is pressed, a call to the resetFunction is made, resetting the Arduino and thus closing the game
        resetFunc();
      }
      drawShape();  //Draws the shape in its current position and rotation on the board
      matrix.show(); //Call to show to display the changes
      newTime = millis();
      if (newTime > (oldTime + downDelay)) { //Checks to see if the amount of time has passed for the block to move down one square
        uint8_t bottomCollision = bottomCollisionCheck();
        if (bottomCollision) {  //Stop the cycle if there is a occupied square below the current position of the block while moving down
          saveShape();  //Save the current position of the shape to the "Board" array
          /*
            Serial.println(F("new Board shape: "));
            for (int i = 0; i < TILEHEIGHT + 1; i++) { //Print out the board value to the serial monitor
            Serial.println(Board[i], BIN);
            }*/
          drawBoard();  //Clear the display and draw everything saved on the board array to the display again.
          lineDetect(); //Check to see if there are any full lines, if so, remove them, add to score and redraw the board.
          if (deathCheck()) { //Check if the blocks have reached the top of the board. if so, commence game-over sequence
            matrix.fill(0xff0000);
            matrix.show();
            delay(3000);
            resetFunc();
          }

          //If bottom has been reached but no game-over: reset the positions and delay and start a new cycle:
          uint8_t delaySubtract;
          if (Score < 8000) {
            delaySubtract = (Score / 1000) * 15; //For every 1000 points scored up to 8000, increase the speed by 15 milliseconds
          }
          else delaySubtract = 8 * 15; //If score is 8000 or higher, speed will always be at maximum
          downDelay = 200 - delaySubtract;
          rotation = 0;
          startNewCycle = 1;
        }
        else { //If no collision below the block (free space underneath to move into) Move the block down one and reset the timer:
          removeShapeFromBuffer();
          Ypos++;
          oldTime = newTime;
        }
      }
    }
  }
}

//Draw the current shape, in its proper rotation, on the board:
void drawShape() {
  int selection = rotation + shape * 4; //Every fourth argument in Shapes[] is a new block shape, and the ones inbetween are the same shape in different rotations
  shapeMap = Shapes[selection]; //Let variable "Shapemap" equal the currently selected shape and rotation...would it be more efficient to just place the "selection" calculation inside the square brackets here?
  for (int8_t YMAP = 0; YMAP < 4; YMAP++) {
    for (int8_t XMAP = 0; XMAP < 4; XMAP++) {
      //Use bitshifts to draw the entire shape on the board, one pixel at a time. (scanning horizontally from left the right, then top to bottom). Function only draws a pixel when there is a 1 in the bitmap. allowing transparency
      if (shapeMap & (0x8000 >> (XMAP + (YMAP * 4)))) {
        matrix.drawPixel(Xpos + XMAP, Ypos + YMAP, ShapeColours[shape]); //Draw the selected pixel of the current shape on the board, if the current position in the shapemap copntains a 1.
      }
    }
  }
}
//Follows the same functionality as "drawShape" but clears them instead. (Could this be compresesd into the same function with a variable flipping it from colour to black?)
void removeShapeFromBuffer() { //Drawing pixels in "black" to essentially remove them from the display.
  int selection = rotation + (shape * 4);
  shapeMap = Shapes[selection];
  for (int8_t YMAP = 0; YMAP < 4; YMAP++) {
    for (int8_t XMAP = 0; XMAP < 4; XMAP++) {
      if (shapeMap & (0x8000 >> (XMAP + (YMAP * 4)))) {
        matrix.drawPixel(Xpos + XMAP, Ypos + YMAP, 0);
      }
    }
  }
}


//Check if there is a wall or already existing block on the board to the left of the current shape
uint8_t leftCollisionCheck() {
  for (uint8_t y = 0; y < 4; y++) {
    for (uint8_t i = 0; i < 4; i++) {
      if (((shapeMap << (y * 4)) & (0x8000 >> i))  &&   ((Board[Ypos + y + 4] << (Xpos)) & (0x8000 >> i))) {
        return 1;
      }
    }
  }
  return 0;
}
//Check if there is a wall or already existing block on the board to the right of the current shape
uint8_t rightCollisionCheck() {
  for (uint8_t y = 0; y < 4; y++) {
    for (uint8_t i = 0; i < 4; i++) {
      if (((shapeMap << (y * 4)) & (0x1000 << i))  &&   ((Board[Ypos + y + 4] << (Xpos)) & (0x0400 << i))) {
        return 1;
      }
    }
  }
  return 0;
}

//Check if the bottom has been reached or if there is another block directly underneath, making the block stop falling and lock in position
uint8_t bottomCollisionCheck() {
  for (uint8_t y = 0; y < 4; y++) {
    for (uint8_t i = 0; i < 4; i++) {
      if (((shapeMap << (y * 4)) & (0x8000 >> i))  &&   (((Board[Ypos + y + 5]) << (Xpos + 1)) & (0x8000 >> i))) {
        return 1;
      }
    }
  }
  return 0;
}

//Check to see if there is enough rooom to rotate
uint8_t rotationCheck(int8_t testRotation) {
  int8_t selection = testRotation + (shape * 4);
  uint16_t testShapeMap = Shapes[selection];
  for (uint8_t y = 0; y < 4; y++) {
    for (uint8_t i = 0; i < 4; i++) {
      if (((testShapeMap << (y * 4)) & (0x8000 >> i))  &&   (((Board[Ypos + y + 4] << (Xpos + 1))) & (0x8000 >> i))) {
        return 1;
      }
    }
  }
  return 0;
}

//Chcek to see if a block got saved somewhere in the top 4 rows of the Board[]. if so, a collision will have happened on the top row of the board, initialising a game-over
uint8_t deathCheck() {
  if (Board[3] & 0x7FE0) { //Board should be 1000000000010000, if not then a block got stuck on top, meaning a game over.
    return 1;
  }
  else {
    return 0;
  }
}

//Draw everything currently saved in Board[] to the display:
void drawBoard() {
  matrix.clear();
  for (uint8_t y = 0; y < TILEHEIGHT; y++) {
    for (uint8_t x = 0; x < TILEWIDTH; x++) {
      if (Board[y + 4] & 0x4000 >> x) {
        matrix.drawPixel(x, y, BOARDCOLOUR); //When there is a "1" in the current position of board. set the pixel
      }
    }
  }
  matrix.show();
}

//Detect if full lines have been made. if more lines at the same time are made, more points are granted.
void lineDetect() {
  uint8_t Amount = 0; //Checking for multiple simultaneous lines. more lines = higher score
  for (uint8_t Position = 4; Position < TILEHEIGHT + 4; Position++) {
    if (Board[Position] == 0xFFF0) {
      Amount++;
      for (uint8_t i = 0; i < (Position - 4); i++) { //If a line has been detected, move everything down one block
        Board[Position - i] = Board[Position - i - 1];
      }
      drawBoard(); //redraw the board after shifting everything down

    }
  }
  switch (Amount) { //add the score and write it to the panel
    case 1:
      Score += 100;
      break;
    case 2 :
      Score += 250;
      break;
    case 3 :
      Score += 400;
      break;
    case 4:
      Score += 600;
      break;
  }
  scorePanel.fillScreen(0);
  scorePanel.setCursor(0, 0);
  scorePanel.print(Score);
  scorePanel.write();
  Serial.println(Score);
}

//Save the current shape, position, and rotation to the Board variable
void saveShape() {
  for (int y = 0; y < 4; y++) {
    Board[Ypos + y + 4] |= ((shapeMap << (y * 4)) & (0xf000)) >> (Xpos + 1);
  }
}

/**************************************************************************************************/
/***********************@@@@@@****@@@@@***@@*****@***@@@@@*****************************************/
/***********************@*****@**@*****@**@*@****@**@*****@****************************************/
/***********************@*****@**@*****@**@**@***@**@**********************************************/
/***********************@@@@@@***@*****@**@***@**@**@***@@@****************************************/
/***********************@********@*****@**@****@*@**@*****@****************************************/
/***********************@*********@@@@@***@*****@@***@@@@@*****************************************/
/**************************************************************************************************/

void mainPong() {
  //Setup:
  uint8_t xP1 = 3;
  uint8_t xP2 = 3;
  uint8_t scoreP1 = 0;
  uint8_t scoreP2 = 0;
  uint8_t ballX = 4;
  uint8_t ballY = TILEHEIGHT/2;
  uint8_t ballDir = 0; //bit one sets X direction, bit 2 sets Y direction
  uint8_t ballDelay = 90;
  printScore(scoreP1, scoreP2);
  oldTime = millis();
  newTime = oldTime;
  for (;;) {
    delay(5);
    ps2x.read_gamepad();
    if (ps2x.Button(PSB_PAD_DOWN) and xP1 < TILEWIDTH - BARWIDTH) {
      xP1++;
    }
    else if (ps2x.Button(PSB_PAD_UP) and xP1 > 0) {
      xP1--;
    }
    if (ps2x.Button(PSB_TRIANGLE) and xP2 < TILEWIDTH - BARWIDTH) {
      xP2++;
    }
    else if (ps2x.Button(PSB_CROSS) and xP2 > 0) {
      xP2--;
    }
    if (ps2x.ButtonPressed(PSB_SELECT)) { //press select to exit game:
      resetFunc();
    }
    matrix.clear();
    matrix.drawLine(xP1, TILEHEIGHT - 1, xP1 + (BARWIDTH - 1), TILEHEIGHT - 1, BLUE);
    matrix.drawLine(xP2, 0, xP2 + (BARWIDTH - 1), 0, BLUE);
    matrix.drawPixel(ballX, ballY, GREEN);
    matrix.show();
    delay(30);
    newTime = millis();
    if (newTime > (oldTime + ballDelay)) {
      //Check to see if the ball has reached the side of the screen:
      if (ballX >= TILEWIDTH - 1) {
        ballDir &= 0x02; //set bit 1 to 0 without affecting 2nd bit
      }
      else if (ballX == 0) {
        ballDir |= 0x01; //Set bit 1 to 1 without affecting 2nd bit
      }

      //Check if the ball has reached the bottom and if the bar is in the correct place to bounce it:
      if (ballY > TILEHEIGHT - 3) {
        if (ballX < xP1 || ballX > (xP1 + BARWIDTH - 1)) {
          ballX = 4;
          ballY = (TILEHEIGHT / 2);
          ballDir = 0;
          ballDelay = 60;
          matrix.fillScreen(RED);
          matrix.show();
          scoreP2++;
          printScore(scoreP1, scoreP2);
          delay(500);
        }
        else {
          ballDir &= 0x01; //set bit 2 to 0 without affecting 1st bit
          ballDelay--;
        }
      }

      //Check if the ball has reached the top of the screen and if the paddle is in the correct place to bounce it:
      if (ballY == 1) {
        if (ballX < xP2 || ballX > (xP2 + BARWIDTH - 1)) {
          ballX = 4;
          ballY = (TILEHEIGHT / 2 + 1);
          ballDir = 2;
          ballDelay = 60;
          matrix.fillScreen(RED);
          matrix.show();
          scoreP1++;
          printScore(scoreP1, scoreP2);
          delay(500);

        }
        else {
          ballDir |= 0x02; //set bit 2 to 1 without affecting the 1st bit
          ballDelay--;
        }
      }
      if (ballDir & 0x02) { //Make the ball either go up or down depending on the value of the 2nd bit of BallDir
        ballY++;
      }
      else ballY--;
      uint8_t moves;
      if (ballX < (TILEWIDTH - 3) && ballX > 2) moves = random(1, 3); //select a random number (1 or 2, since max value is exclusive) to move on the Xbar
      else moves = 1; //Set moves to 1 if there is no room on the board to move 2 tiles
      if (ballDir & 0x01) {
        ballX += moves;
      }
      else ballX -= moves;
      pongOver(scoreP1, scoreP2);
      oldTime = newTime;
    }
  }
}
void printScore(uint8_t p1, uint8_t p2) {
  scorePanel.fillScreen(0);
  scorePanel.setCursor(0, 0);
  scorePanel.print(p1);
  scorePanel.print(":");
  scorePanel.print(p2);
  scorePanel.write();
}
void pongOver(uint8_t p1, uint8_t p2) {
  if (p1 > PONGSCORE-1) {
    scorePanel.fillScreen(0);
    scorePanel.setCursor(0, 0);
    scorePanel.print("P1WIN");
    scorePanel.write();
    while (1) {
      delay(10);
      ps2x.read_gamepad();
      if (ps2x.ButtonPressed(PSB_SELECT)) resetFunc();
    }
  }
  else if (p2 > PONGSCORE-1) {
    scorePanel.fillScreen(0);
    scorePanel.setCursor(0, 0);
    scorePanel.print("P2WIN");
    scorePanel.write();
    while (1) {
      delay(10);
      ps2x.read_gamepad();
      if (ps2x.ButtonPressed(PSB_SELECT)) resetFunc();
    }
  }
}

/*
   Backgrounds. These are shown during the menu
*/
void tetrisBackground() {
  rotation = 0;
  shape = 2;
  Xpos = 3;
  Ypos = 14;
  drawShape();
  shape = 6;
  rotation = 1;
  Xpos = 3;
  Ypos = 16;
  drawShape();
  Xpos = 4;
  Ypos = 10;
  shape = 0;
  drawShape();
}

void pongBackground() {
  matrix.drawLine(3, TILEHEIGHT - 3, 5, TILEHEIGHT - 3, BLUE);
  matrix.drawPixel(5, TILEHEIGHT - 6, GREEN);
}

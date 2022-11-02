// #include "Tetris.h"

// //Tetris shapes:
// //Every shape has 4 possible rotations, every "Bitmap" of a 4x4 shape is stored in a single 16-bit number
// const uint16_t Shapes[28] = { //Consideration: store these in PROGMEM as well?
//   0b0000000000110110, //S-Shape
//   0b0000010001100010,
//   0b0000000000110110,
//   0b0000010001100010,

//   0b0000000011000110, //Z-Shape
//   0b0000001001100100,
//   0b0000000011000110,
//   0b0000001001100100,

//   0b0000010001000110, //L-shape
//   0b0000000000010111,
//   0b0000011000100010,
//   0b0000000011101000,

//   0b0000001000100110, //J shape
//   0b0000000001110001,
//   0b0000011001000100,
//   0b0000000010001110,

//   0b0000000001100110, //O-Shape
//   0b0000000001100110,
//   0b0000000001100110,
//   0b0000000001100110,

//   0b0000000001001110, //T-shape
//   0b0000010001100100,
//   0b0000000011100100,
//   0b0000010011000100,

//   0b0100010001000100, //I-Shape
//   0b0000000011110000,
//   0b0100010001000100,
//   0b0000000011110000
// };

// int8_t shape;
// int8_t rotation;
// uint16_t downDelay = 200;
// uint16_t shapeMap;

// //Give every shape a different colour
// const uint16_t ShapeColours[7] = {BLUE, GREEN, MAGENTA, DARKGREEN, YELLOW, RED, ORANGE};

// //(taken from te main loop of the Tetris-only version)
// void mainTetris() {
//   for (;;) { //Loop forever. processor will reboot to return to main menu.
//     Ypos = -4; //Setting X and Y position to the top center of the screen
//     Xpos = 3;
//     shape = random(0, 7); //select a random shape
//     shapeMap = Shapes[shape * 4];
//     int startNewCycle = 0;
//     delay(100);
//     oldTime = newTime;
//     while (!startNewCycle) {
//       delay(5);
//       ps2x.read_gamepad(); //Read which buttons are pressed
//       delay(5);
//       if (ps2x.ButtonPressed(PSB_PAD_DOWN)) { //Pressing the down button will make the block fall down a lot faster.
//         downDelay = 10;
//       }
//       if (ps2x.ButtonPressed(PSB_PAD_LEFT)) { //Pressing the left pad will attempt to move the block to the left, but fails if there is something obstructing the movement
//         if ((!leftCollisionCheck() && (Xpos >= 0))) { //When Xpos=-1 the program can't compute this because of a negative bit-shift. but when x=-1, you are always at the left edge anyway so just disabled the shift there.
//           removeShapeFromBuffer();
//           Xpos--;
//         }
//       }
//       if (ps2x.ButtonPressed(PSB_PAD_RIGHT)) {//Pressing the right pad will attempt to move the block to the right, but fails if there is something obstructing the movement
//         if (!rightCollisionCheck()) { //See if there is space to move before changing the position
//           removeShapeFromBuffer();
//           Xpos++;
//         }
//       }
//       if (ps2x.ButtonPressed(PSB_CROSS)) { //When cross is pressed, the block will attempt to rotate clockwise, if there is a block or wall obstructing the rotated shape, no rotation will be executed
//         removeShapeFromBuffer();
//         int8_t newRotation = rotation - 1; //Setting the new rotation in a test variable to determine whether a rotation is possible.
//         if (newRotation < 0) {
//           newRotation = 3;
//         }
//         if (!rotationCheck(newRotation)) {
//           rotation = newRotation; //If rotation is possible, change the "real" rotation value.
//         }
//       }
//       if (ps2x.ButtonPressed(PSB_CIRCLE)) { //When circle is pressed, the block will attempt to rotate counterclockwise, if there is a block or wall obstructing the rotated shape, no rotation will be executed
//         removeShapeFromBuffer();
//         int8_t newRotation = rotation + 1;
//         if (newRotation > 3) {
//           newRotation = 0;
//         }
//         if (!rotationCheck(newRotation)) {
//           rotation = newRotation;
//         }
//       }
//       if (ps2x.ButtonPressed(PSB_SELECT)) { //When the select button is pressed, a call to the resetFunction is made, resetting the Arduino and thus closing the game
//         resetFunc();
//       }
//       drawShape();  //Draws the shape in its current position and rotation on the board
//       matrix.show(); //Call to show to display the changes
//       newTime = millis();
//       if (newTime > (oldTime + downDelay)) { //Checks to see if the amount of time has passed for the block to move down one square
//         uint8_t bottomCollision = bottomCollisionCheck();
//         if (bottomCollision) {  //Stop the cycle if there is a occupied square below the current position of the block while moving down
//           saveShape();  //Save the current position of the shape to the "Board" array
//           /*
//             Serial.println(F("new Board shape: "));
//             for (int i = 0; i < TILEHEIGHT + 1; i++) { //Print out the board value to the serial monitor
//             Serial.println(Board[i], BIN);
//             }*/
//           drawBoard();  //Clear the display and draw everything saved on the board array to the display again.
//           lineDetect(); //Check to see if there are any full lines, if so, remove them, add to score and redraw the board.
//           if (deathCheck()) { //Check if the blocks have reached the top of the board. if so, commence game-over sequence
//             matrix.fill(0xff0000);
//             matrix.show();
//             delay(3000);
//             resetFunc();
//           }

//           //If bottom has been reached but no game-over: reset the positions and delay and start a new cycle:
//           uint8_t delaySubtract;
//           if (Score < 8000) {
//             delaySubtract = (Score / 1000) * 15; //For every 1000 points scored up to 8000, increase the speed by 15 milliseconds
//           }
//           else delaySubtract = 8 * 15; //If score is 8000 or higher, speed will always be at maximum
//           downDelay = 200 - delaySubtract;
//           rotation = 0;
//           startNewCycle = 1;
//         }
//         else { //If no collision below the block (free space underneath to move into) Move the block down one and reset the timer:
//           removeShapeFromBuffer();
//           Ypos++;
//           oldTime = newTime;
//         }
//       }
//     }
//   }
// }

// //Draw the current shape, in its proper rotation, on the board:
// void drawShape() {
//   int selection = rotation + shape * 4; //Every fourth argument in Shapes[] is a new block shape, and the ones inbetween are the same shape in different rotations
//   shapeMap = Shapes[selection]; //Let variable "Shapemap" equal the currently selected shape and rotation...would it be more efficient to just place the "selection" calculation inside the square brackets here?
//   for (int8_t YMAP = 0; YMAP < 4; YMAP++) {
//     for (int8_t XMAP = 0; XMAP < 4; XMAP++) {
//       //Use bitshifts to draw the entire shape on the board, one pixel at a time. (scanning horizontally from left the right, then top to bottom). Function only draws a pixel when there is a 1 in the bitmap. allowing transparency
//       if (shapeMap & (0x8000 >> (XMAP + (YMAP * 4)))) {
//         matrix.drawPixel(Xpos + XMAP, Ypos + YMAP, ShapeColours[shape]); //Draw the selected pixel of the current shape on the board, if the current position in the shapemap copntains a 1.
//       }
//     }
//   }
// }
// //Follows the same functionality as "drawShape" but clears them instead. (Could this be compresesd into the same function with a variable flipping it from colour to black?)
// void removeShapeFromBuffer() { //Drawing pixels in "black" to essentially remove them from the display.
//   int selection = rotation + (shape * 4);
//   shapeMap = Shapes[selection];
//   for (int8_t YMAP = 0; YMAP < 4; YMAP++) {
//     for (int8_t XMAP = 0; XMAP < 4; XMAP++) {
//       if (shapeMap & (0x8000 >> (XMAP + (YMAP * 4)))) {
//         matrix.drawPixel(Xpos + XMAP, Ypos + YMAP, 0);
//       }
//     }
//   }
// }

// //Check if there is a wall or already existing block on the board to the left of the current shape
// uint8_t leftCollisionCheck() {
//   for (uint8_t y = 0; y < 4; y++) {
//     for (uint8_t i = 0; i < 4; i++) {
//       if (((shapeMap << (y * 4)) & (0x8000 >> i))  &&   ((Board[Ypos + y + 4] << (Xpos)) & (0x8000 >> i))) {
//         return 1;
//       }
//     }
//   }
//   return 0;
// }

// //Check if there is a wall or already existing block on the board to the right of the current shape
// uint8_t rightCollisionCheck() {
//   for (uint8_t y = 0; y < 4; y++) {
//     for (uint8_t i = 0; i < 4; i++) {
//       if (((shapeMap << (y * 4)) & (0x1000 << i))  &&   ((Board[Ypos + y + 4] << (Xpos)) & (0x0400 << i))) {
//         return 1;
//       }
//     }
//   }
//   return 0;
// }

// //Check if the bottom has been reached or if there is another block directly underneath, making the block stop falling and lock in position
// uint8_t bottomCollisionCheck() {
//   for (uint8_t y = 0; y < 4; y++) {
//     for (uint8_t i = 0; i < 4; i++) {
//       if (((shapeMap << (y * 4)) & (0x8000 >> i))  &&   (((Board[Ypos + y + 5]) << (Xpos + 1)) & (0x8000 >> i))) {
//         return 1;
//       }
//     }
//   }
//   return 0;
// }

// //Check to see if there is enough rooom to rotate
// uint8_t rotationCheck(int8_t testRotation) {
//   int8_t selection = testRotation + (shape * 4);
//   uint16_t testShapeMap = Shapes[selection];
//   for (uint8_t y = 0; y < 4; y++) {
//     for (uint8_t i = 0; i < 4; i++) {
//       if (((testShapeMap << (y * 4)) & (0x8000 >> i))  &&   (((Board[Ypos + y + 4] << (Xpos + 1))) & (0x8000 >> i))) {
//         return 1;
//       }
//     }
//   }
//   return 0;
// }

// //Chcek to see if a block got saved somewhere in the top 4 rows of the Board[]. if so, a collision will have happened on the top row of the board, initialising a game-over
// uint8_t deathCheck() {
//   if (Board[3] & 0x7FE0) { //Board should be 1000000000010000, if not then a block got stuck on top, meaning a game over.
//     return 1;
//   }
//   else {
//     return 0;
//   }
// }

// //Draw everything currently saved in Board[] to the display:
// void drawBoard() {
//   matrix.clear();
//   for (uint8_t y = 0; y < TILEHEIGHT; y++) {
//     for (uint8_t x = 0; x < TILEWIDTH; x++) {
//       if (Board[y + 4] & 0x4000 >> x) {
//         matrix.drawPixel(x, y, BOARDCOLOUR); //When there is a "1" in the current position of board. set the pixel
//       }
//     }
//   }
//   matrix.show();
// }

// //Detect if full lines have been made. if more lines at the same time are made, more points are granted.
// void lineDetect() {
//   uint8_t Amount = 0; //Checking for multiple simultaneous lines. more lines = higher score
//   for (uint8_t Position = 4; Position < TILEHEIGHT + 4; Position++) {
//     if (Board[Position] == 0xFFF0) {
//       Amount++;
//       for (uint8_t i = 0; i < (Position - 4); i++) { //If a line has been detected, move everything down one block
//         Board[Position - i] = Board[Position - i - 1];
//       }
//       drawBoard(); //redraw the board after shifting everything down

//     }
//   }
//   switch (Amount) { //add the score and write it to the panel
//     case 1:
//       Score += 100;
//       break;
//     case 2 :
//       Score += 250;
//       break;
//     case 3 :
//       Score += 400;
//       break;
//     case 4:
//       Score += 600;
//       break;
//   }
//   scorePanel.fillScreen(0);
//   scorePanel.setCursor(0, 0);
//   scorePanel.print(Score);
//   scorePanel.write();
//   Serial.println(Score);
// }

// //Save the current shape, position, and rotation to the Board variable
// void saveShape() {
//   for (int y = 0; y < 4; y++) {
//     Board[Ypos + y + 4] |= ((shapeMap << (y * 4)) & (0xf000)) >> (Xpos + 1);
//   }
// }

// void tetrisBackground() {
//   rotation = 0;
//   shape = 2;
//   Xpos = 3;
//   Ypos = 14;
//   drawShape();
//   shape = 6;
//   rotation = 1;
//   Xpos = 3;
//   Ypos = 16;
//   drawShape();
//   Xpos = 4;
//   Ypos = 10;
//   shape = 0;
//   drawShape();
// }
#include "Arexx_Pong.h"

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

void pongBackground() {
  matrix.drawLine(3, TILEHEIGHT - 3, 5, TILEHEIGHT - 3, BLUE);
  matrix.drawPixel(5, TILEHEIGHT - 6, GREEN);
}
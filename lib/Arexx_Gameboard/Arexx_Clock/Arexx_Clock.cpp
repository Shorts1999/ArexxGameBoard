
#include "Arexx_Clock/Arexx_Clock.h"

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

/***************************************************************************************/
/*************************@@@@**@******@@@@***@@@@**@**@********************************/
/************************@******@*****@****@*@******@*@*********************************/
/************************@******@*****@****@*@******@@**********************************/
/************************@******@*****@****@*@******@*@*********************************/
/*************************@@@@**@@@@***@@@@***@@@@**@**@********************************/
/***************************************************************************************/
void drawDigit(uint8_t digit, uint8_t x, uint8_t y, uint16_t colour) {
  uint8_t digitMap[5];
  for (uint8_t i = 0; i < 5; i++) {
    digitMap[i] = ((pgm_read_dword_near(digitalNumbers + i) << (digit * 3)) >> 24);
  }
  matrix.drawBitmap(x, y, digitMap, 3, 5, colour);
}

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

void clockBackground(){
  matrix.drawCircle(5, TILEHEIGHT-5, 4, RED);
  matrix.drawLine(5, TILEHEIGHT-5, 5, TILEHEIGHT - 9, BLUE);
  matrix.drawLine(5, TILEHEIGHT-5, 8, TILEHEIGHT-5, GREEN);
  matrix.drawPixel(5,TILEHEIGHT-5, RED);
  matrix.show();
}
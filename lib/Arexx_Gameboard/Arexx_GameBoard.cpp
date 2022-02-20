#include "Arexx_GameBoard.h"

Adafruit_NeoMatrix matrix = Adafruit_NeoMatrix(TILEWIDTH, TILEHEIGHT, XTILES, YTILES, LEDPIN, PANELTYPE + TILEORIENTATION, NEO_GRB + NEO_KHZ800);
Max72xxPanel scorePanel = Max72xxPanel(SCOREPIN, SCOREPANELAMOUNT, 1);
PS2X ps2x;
DS1339 RTC = DS1339(INT_PIN, INT_NUMBER);

int8_t Xpos = 3;
int8_t Ypos = -4; // start of the board is at y=0, shape will start above the board and scroll inside
uint32_t Score = 0;
uint16_t value = 0;
int16_t error = 0;
uint8_t pendingSelect = 0;
uint16_t shapeMap;

unsigned long newTime;
unsigned long oldTime;

uint16_t downDelay = 200;

uint16_t Board[TILEHEIGHT + 5];

void(* resetFunc) (void);
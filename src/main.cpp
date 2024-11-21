#include <Arduino.h>
#include <WiFi.h>
#include <GameBoard.h>
#include <Tetris/Tetris.h>
#include <SPIFFS.h>
#define DEBUG 1
#include <simpleDebug.h>

#include <ESP32Wiimote.h>

#define MATRIXWIDTH 10
#define MATRIXHEIGHT 20

#define SCOREPIN 6

#define BUTTON_UP 10
#define BUTTON_DOWN 11
#define BUTTON_LEFT 12
#define BUTTON_RIGHT 13
#define BUTTON_A 20
#define BUTTON_B 21
#define BUTTON_START 30
#define BUTTON_SELECT 31

uint32_t colour;
GameBoard gameboard(MATRIXPIN, MATRIXWIDTH, MATRIXHEIGHT, GameBoard::Orientation::Columns);

ESP32Wiimote wiimote;

Tetris tetris(gameboard, wiimote);
bool gameHasBegun = false;
//Function prototypes
void startingAnimation();

#include <BluetoothSerial.h>
BluetoothSerial bt;

void setup() {
    Serial.begin(115200);
    DEBUG_PRINT("RUNNING IN DEBUG MODE\n\r");   //let us know if we are in debug mode
    //Initialize SPIFFS
    if (!SPIFFS.begin(false)) {
        DEBUG_PRINT("An Error has occurred while mounting SPIFFS");
        return;
    }

    //Setup wiiremote
    
    Serial.println("Init done");
    // wiimote.addFilter(ACTION_IGNORE, FILTER_ACCEL); // Ignore accelerometer
    Serial.println("Added filter");
    // delay(5000);
    //initialise random number generation
    randomSeed(esp_random());

    // webServer.on("/update", HTTP_GET, [](AsyncWebServerRequest *request) {
    //     uint16_t score = tetris.getScore();
    //     bool isGameOver = tetris.isGameOver();
    //     DynamicJsonDocument jsonDoc(256);
    //     jsonDoc["score"] = score;
    //     jsonDoc["state"] = isGameOver;
    //     String jsonString;
    //     serializeJson(jsonDoc, jsonString);
    //     DEBUG_PRINT(jsonString.c_str());
    //     request->send(200, "text/json", jsonString);
    // });
    // webServer.on("/button", HTTP_POST, [](AsyncWebServerRequest *request) {
    //     int paramCnt = request->params();
    //     DEBUG_PRINT("\n\rReceived button post, containing %i params\n\r", paramCnt);
    //     request->send(200, "text/plain", "OK");
    //     if (paramCnt != 1) return;
    //     AsyncWebParameter *parameter = request->getParam(0);
    //     DEBUG_PRINT("Parameter: %s Value: %s\n\r", parameter->name(), parameter->value());
    //     //Convert parameter string to integer
    //     int paramValue = atoi(parameter->value().c_str());
    //     switch (paramValue) {
    //         case BUTTON_UP:
    //             DEBUG_PRINT("Button up was pressed");
    //             break;
    //         case BUTTON_DOWN:
    //             DEBUG_PRINT("Button down was pressed");
    //             tetris.speedDrop();
    //             break;
    //         case BUTTON_LEFT:
    //             DEBUG_PRINT("Button left was pressed");
    //             tetris.moveLeft();
    //             break;
    //         case BUTTON_RIGHT:
    //             DEBUG_PRINT("Button right was pressed");
    //             tetris.moveRight();
    //             break;
    //         case BUTTON_A:
    //             DEBUG_PRINT("Button A was pressed");
    //             tetris.rotate(ROTATE_CLOCKWISE);
    //             break;
    //         case BUTTON_B:
    //             DEBUG_PRINT("Button B was pressed");
    //             tetris.rotate(ROTATE_ANTICLOCKWISE);
    //             break;
    //         case BUTTON_START:
    //             DEBUG_PRINT("Button Start was pressed");
    //             gameHasBegun = true; //Start the game on start press
    //             break;
    //         case BUTTON_SELECT:
    //             DEBUG_PRINT("Button Select was pressed");
    //             break;
    //         default:
    //             DEBUG_PRINT("Unknown button ID received");
    //             break;
    //     }

    // });


    // webServer.begin();
    // DEBUG_PRINT("Server started\n");
    FastLED.setBrightness(250);
    FastLED.clear();

    FastLED.show();
    // while (!gameHasBegun) { delay(100); }
    Serial.println("STARTING GAME");
    tetris.run();
}

void loop() {

}

// #include <Arduino.h>
// #include <GameBoard.h>
// #include <Tetris/Tetris.h>
// #include "ESP32Wiimote.h"

// ESP32Wiimote wiimote;

// static bool logging = true;
// static long last_ms = 0;
// static int num_run = 0, num_updates = 0;

// void setup()
// {
//     Serial.begin(115200);
//     Serial.println("ESP32Wiimote");
    
//     wiimote.init();
//     if (! logging)
//         wiimote.addFilter(ACTION_IGNORE, FILTER_ACCEL); // optional
    
//     Serial.println("Started");
//     last_ms = millis();
// }

// void loop()
// {
//     wiimote.task();
//     num_run++;

//     if (wiimote.available() > 0) 
//     {
//         ButtonState  button  = wiimote.getButtonState();
//         AccelState   accel   = wiimote.getAccelState();
//         NunchukState nunchuk = wiimote.getNunchukState();

//         num_updates++;
//         if (logging)
//         {
//             char ca     = (button & BUTTON_A)     ? 'A' : '.';
//             char cb     = (button & BUTTON_B)     ? 'B' : '.';
//             char cc     = (button & BUTTON_C)     ? 'C' : '.';
//             char cz     = (button & BUTTON_Z)     ? 'Z' : '.';
//             char c1     = (button & BUTTON_ONE)   ? '1' : '.';
//             char c2     = (button & BUTTON_TWO)   ? '2' : '.';
//             char cminus = (button & BUTTON_MINUS) ? '-' : '.';
//             char cplus  = (button & BUTTON_PLUS)  ? '+' : '.';
//             char chome  = (button & BUTTON_HOME)  ? 'H' : '.';
//             char cleft  = (button & BUTTON_LEFT)  ? '<' : '.';
//             char cright = (button & BUTTON_RIGHT) ? '>' : '.';
//             char cup    = (button & BUTTON_UP)    ? '^' : '.';
//             char cdown  = (button & BUTTON_DOWN)  ? 'v' : '.';
      
//             Serial.printf("button: %05x = ", (int)button);
//             Serial.print(ca);
//             Serial.print(cb);
//             Serial.print(cc);
//             Serial.print(cz);
//             Serial.print(c1);
//             Serial.print(c2);
//             Serial.print(cminus);
//             Serial.print(chome);
//             Serial.print(cplus);
//             Serial.print(cleft);
//             Serial.print(cright);
//             Serial.print(cup);
//             Serial.print(cdown);
//             Serial.printf(", wiimote.axis: %3d/%3d/%3d", accel.xAxis, accel.yAxis, accel.zAxis);
//             Serial.printf(", nunchuk.axis: %3d/%3d/%3d", nunchuk.xAxis, nunchuk.yAxis, nunchuk.zAxis);
//             Serial.printf(", nunchuk.stick: %3d/%3d\r", nunchuk.xStick, nunchuk.yStick);
//         }
//     }

//     if (! logging)
//     {
//         long ms = millis();
//         if (ms - last_ms >= 1000)
//         {
//             Serial.printf("Run %d times per second with %d updates\n", num_run, num_updates);
//             num_run = num_updates = 0;
//             last_ms += 1000;
//         }
//     }

//     delay(10);
// }

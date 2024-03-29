#include <Arduino.h>
#include <WiFi.h>
#include <WiFiManager.h>
#include <GameBoard.h>
#include <Tetris/Tetris.h>
#include <SPIFFS.h>
#include <ArduinoJson.h>
#define DEBUG 1
#include <simpleDebug.h>

#include <AsyncTCP.h>
#include <ESPAsyncWebServer.h>

// #include <webHelper.h>

#define MATRIXPIN 13
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

AsyncWebServer webServer(80);
Tetris tetris(gameboard, webServer);
bool gameHasBegun = false;
//Function prototypes
void startingAnimation();

void setup() {
    Serial.begin(115200);
    DEBUG_PRINT("RUNNING IN DEBUG MODE\n\r");   //let us know if we are in debug mode
    //Initialize SPIFFS
    if (!SPIFFS.begin(false)) {
        DEBUG_PRINT("An Error has occurred while mounting SPIFFS");
        return;
    }
    Serial.print("Connecting");
    WiFi.mode(WIFI_MODE_STA);
    //Set up Wifimanager for provisioning of Wi-Fi credentials
    WiFiManager wm;
    wm.autoConnect("ESP32-GamePanel");
    delay(1000);

    //initialise random number generation
    randomSeed(esp_random());

    // webServer.onNotFound(defaultHandler);

    webServer.on("/", HTTP_GET, [](AsyncWebServerRequest *request) {
        request->send(SPIFFS, "/index.html", String(), false);
    });
    webServer.on("/control.js", HTTP_GET, [](AsyncWebServerRequest *request) {
        request->send(SPIFFS, "/control.js", "text/javascript", false);
    });
    webServer.on("/style.css", HTTP_GET, [](AsyncWebServerRequest *request) {
        request->send(SPIFFS, "/style.css", "text/css", false);
    });
    webServer.on("/update", HTTP_GET, [](AsyncWebServerRequest *request){
        	uint16_t score = tetris.getScore();
            bool isGameOver = tetris.isGameOver();
            DynamicJsonDocument jsonDoc(256);
            jsonDoc["score"] = score;
            jsonDoc["state"] = isGameOver;
            String jsonString;
            serializeJson(jsonDoc, jsonString);
            DEBUG_PRINT(jsonString.c_str());
            request->send(200, "text/json", jsonString);
    });
    webServer.on("/button", HTTP_POST, [](AsyncWebServerRequest *request) {
        int paramCnt = request->params();
        DEBUG_PRINT("\n\rReceived button post, containing %i params\n\r", paramCnt);
        request->send(200, "text/plain", "OK");
        if (paramCnt != 1) return;
        AsyncWebParameter *parameter = request->getParam(0);
        DEBUG_PRINT("Parameter: %s Value: %s\n\r", parameter->name(), parameter->value());
        //Convert parameter string to integer
        int paramValue = atoi(parameter->value().c_str());
        switch (paramValue) {
            case BUTTON_UP:
                DEBUG_PRINT("Button up was pressed");
                break;
            case BUTTON_DOWN:
                DEBUG_PRINT("Button down was pressed");
                tetris.speedDrop();
                break;
            case BUTTON_LEFT:
                DEBUG_PRINT("Button left was pressed");
                tetris.moveLeft();
                break;
            case BUTTON_RIGHT:
                DEBUG_PRINT("Button right was pressed");
                tetris.moveRight();
                break;
            case BUTTON_A:
                DEBUG_PRINT("Button A was pressed");
                tetris.rotate(ROTATE_CLOCKWISE);
                break;
            case BUTTON_B:
                DEBUG_PRINT("Button B was pressed");
                tetris.rotate(ROTATE_ANTICLOCKWISE);
                break;
            case BUTTON_START:
                DEBUG_PRINT("Button Start was pressed");
                gameHasBegun=true; //Start the game on start press
                break;
            case BUTTON_SELECT:
                DEBUG_PRINT("Button Select was pressed");
                break;
            default:
                DEBUG_PRINT("Unknown button ID received");
                break;
        }

    });


    webServer.begin();
    
    FastLED.setBrightness(20);
    FastLED.clear();

    FastLED.show();
    while(!gameHasBegun){ delay(100);}
    DEBUG_PRINT("STARTING GAME");
    tetris.run();
}

void loop() {

}

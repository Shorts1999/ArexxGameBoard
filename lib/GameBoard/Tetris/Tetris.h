#include <Arduino.h>
#include <WiFi.h>
#include <SPIFFS.h>
#include <GameBoard.h>

#include <AsyncTCP.h>
#include <ESPAsyncWebServer.h>

// #define DEBUG
#include <simpleDebug.h>

#define ROTATE_CLOCKWISE false
#define ROTATE_ANTICLOCKWISE true

class Tetris {
public:
    //Constructor:
    Tetris(GameBoard &gameboard, AsyncWebServer &server);

    //MEMBERS:


    //METHODS:
    int8_t moveLeft();
    int8_t moveRight();
    int8_t moveDown();
    int8_t rotate(bool anitclockwise);
    void newShape();
    void saveShape();
    void run();
    void speedDrop();

private:
    //MEMBERS:
    GameBoard &mGameBoard;
    AsyncWebServer &mWebServer;

    uint8_t currentShape;
    uint8_t currentRotation;

    uint16_t downDelay = 100;
    uint16_t totalDropCount = 10;
    uint16_t lineCount = 0;

    //METHODS:

    uint8_t leftCollisionCheck();
    uint8_t rightCollisionCheck();
    uint8_t bottomCollisionCheck();
    uint8_t rotationCollisionCheck();
    uint16_t lineDetect();
    void clearLine(uint8_t);
    void drawShape();
    void removeShapeFromBuffer();
    void gameOver();

    //DESTRUCTOR:
};
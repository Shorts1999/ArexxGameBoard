#include <Arduino.h>
#include <WiFi.h>
#include <SPIFFS.h>
#include <GameBoard.h>

#include <ESP32Wiimote.h>

// #define DEBUG
#include <simpleDebug.h>

#define ROTATE_CLOCKWISE false
#define ROTATE_ANTICLOCKWISE true

class Tetris {
public:
    //Constructor:
    Tetris(GameBoard &gameboard, ESP32Wiimote &wiimote);

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
    uint16_t getScore();
    bool isGameOver();

private:
    //MEMBERS:
    GameBoard &mGameBoard;
    ESP32Wiimote &mWiimote;

    uint8_t currentShape;
    uint8_t currentRotation;

    uint16_t downDelay = 10;
    uint16_t totalDropCount = 100;
    uint16_t lineCount = 0;
    bool mGameOver = false;

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
    void resetGame();

    //DESTRUCTOR:
};
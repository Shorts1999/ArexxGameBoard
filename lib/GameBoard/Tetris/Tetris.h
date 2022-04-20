#include <Arduino.h>
#include <WiFi.h>
#include <SPIFFS.h>
#include <GameBoard.h>

#include <AsyncTCP.h>
#include <ESPAsyncWebServer.h>

class Tetris {
public:
    Tetris(GameBoard &gameboard, AsyncWebServer &server);

    int8_t moveLeft();
    int8_t moveRight();
    int8_t rotate();

private:
    int8_t moveDown();

    uint8_t leftCollisionCheck(uint8_t shape, uint8_t rotation);
    uint8_t rightCollisionCheck(uint8_t shape, uint8_t rotation);
    uint8_t bottomCollisionCheck(uint8_t shape, uint8_t rotation);
    void drawShape(uint8_t shape, uint8_t rotation);
    void removeShapeFromBuffer(uint8_t shape, uint8_t rotation);


    GameBoard &mGameBoard;
    AsyncWebServer &mWebServer;
};
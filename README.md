# ESP32 WS2812 based game board

This repository contains firmware for an ESP32 microcontroller, controlling an array of 10x20 addressable LEDs. The firmware contains the option of multiple ways to control the display:

* Control through a web-interface, on a smartphone
* Control through a wireless Bluetooth controller
* Control through a wired PS2 Controller
* Control through a wired NES or SNES Controller

## Prerequisites

* Visual studio code
* PlatformIO Extension for visual studio code

## How to install

1. Clone this repository in visual studio code
2. Connect the ESP32 microcontroller via USB
3. Open a platformIO terminal in visual studio code
4. Clear the memory: ```pio run -t erase```
5. Upload the webpage to the filesystem (required when using the web-based controller option, otherwise optional): ```pio run -t uploadfs```
6. Upload the ESP32 code: ```pio run -t upload```
7. Optionally, open the serial monitor to verify operation: ```pio device monitor```

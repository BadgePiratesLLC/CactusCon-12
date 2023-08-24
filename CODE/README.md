# Project-HomeBase
## Version: 2023 ESP32-S3

This project serves as a boilerplate for badgepirates code, it is designed to facilitate rapid developement and production of badges.

When you are creating a new badge, clone this repo, delete the .git folder and copy it into a badge project. 

Configure `prototype.cpp/hpp` to add code to drive hardware necessary to perform a functional test of components, and set up OTA information in `config.hpp`.

Create a binary and save it to be used later during initial flashing. Flash it with esptool.

Start development on the project and remove the test code, change the version in `config.hpp` to `1` and code as you wish. 

When you are ready to make production binaries, made sure debug mode is off and compile, compile and save.

For an OTA server, run an HTTP server with two files, `version` and `binary` (can change in config), the version file will be a number represending `VERSION` in `config.hpp` and binary will be the .bin without the extension. 

## Built-in tests

* WS2812 All color test
* Button testing
* OLED tests (if equipped)
* Accessory testing (Defaults to running OLED code on I2C bus 2)

## Capabilities

* In-production automatic hardware testing and WiFi flashing
* OTA

## Wants

When there's spare time, try implementing these features.

* HTTPS updates (Have had cert errors on GH pages)
* Background updates
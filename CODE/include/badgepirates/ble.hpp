#pragma once

#include <Arduino.h>
#include <BLEDevice.h>
#include <BLEServer.h>

#define SERVICE_UUID "4fafc201-1fb5-459e-8fcc-c5c9c331914b"
#define CHARACTERISTIC_UUID "beb5483e-36e1-4688-b7f5-ea07361b26a8"

extern BLECharacteristic *pCharacteristic;
extern bool deviceConnected;
extern bool oldDeviceConnected;
extern uint8_t value;

class MyBLEServer
{
public:
    MyBLEServer();
    void init();
    void loop();

private:
    class MyServerCallbacks : public BLEServerCallbacks
    {
    public:
        void onConnect(BLEServer *pServer);
        void onDisconnect(BLEServer *pServer);
    };

    BLEServer *pServer;
};
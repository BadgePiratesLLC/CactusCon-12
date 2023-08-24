#include "badgepirates/ble.hpp"
#include "badgepirates/factory.hpp"

Factory factoryObject;

BLECharacteristic *pCharacteristic;
bool deviceConnected = false;
bool oldDeviceConnected = false;
uint8_t value = 0;

MyBLEServer::MyBLEServer()
{
  pServer = NULL;
}

void MyBLEServer::init()
{
  Serial.begin(115200);

  // Create the BLE Device
  BLEDevice::init("Badge Pirates Badge #123456");

  // Create the BLE Server
  pServer = BLEDevice::createServer();
  pServer->setCallbacks(new MyServerCallbacks());

  // Create the BLE Service
  BLEService *pService = pServer->createService(SERVICE_UUID);

  // Create a BLE Characteristic
  pCharacteristic = pService->createCharacteristic(
      CHARACTERISTIC_UUID,
      BLECharacteristic::PROPERTY_READ |
          BLECharacteristic::PROPERTY_WRITE |
          BLECharacteristic::PROPERTY_NOTIFY |
          BLECharacteristic::PROPERTY_INDICATE);

  // Start the service
  pService->start();

  // Starting Advertising
  BLEAdvertising *pAdvertising = pServer->getAdvertising();
  pAdvertising->addServiceUUID(SERVICE_UUID);
  pAdvertising->start();

  Serial.println("Waiting a client connection to notify...");
}

void MyBLEServer::loop()
{
  if (deviceConnected)
  {
    pCharacteristic->setValue(&value, 1);
    pCharacteristic->notify();
    value++;
    delay(10); // Consider adjusting this delay to suit your specific needs
  }

  // Debounce logic for handling connection state changes
  if (!deviceConnected && oldDeviceConnected)
  {
    // Give the client a chance to reconnect on its own if it's still in range
    delay(500); // This delay might need to be adjusted based on your requirements
    if (!deviceConnected)
    {
      // If the client hasn't reconnected on its own, start advertising
      pServer->startAdvertising();
      Serial.println("start advertising");
      oldDeviceConnected = deviceConnected;
    }
  }
  else if (deviceConnected && !oldDeviceConnected)
  {
    oldDeviceConnected = deviceConnected;
  }
}

void MyBLEServer::MyServerCallbacks::onConnect(BLEServer *pServer)
{
  deviceConnected = true;
  Serial.println("Device connected!");
  factoryObject.test_ws2812();
}

void MyBLEServer::MyServerCallbacks::onDisconnect(BLEServer *pServer)
{
  deviceConnected = false;
  factoryObject.test_ws2812();
  Serial.println("Device disconnected!");
}
// BLEHandler.cpp
#include "BLEHandler.h"
#include <Arduino.h>
#include <BLEDevice.h>
#include <BLEUtils.h>
#include <BLEServer.h>
#include <BLE2902.h>
#include <vector> // Para usar std::vector si es necesario

using namespace std;

// Definición de los UUIDs
const char* SERVICE_UUID = "4fafc201-1fb5-459e-8fcc-c5c9c331914b";
const char* CHARACTERISTIC_UUID = "beb5483e-36e1-4688-b7f5-ea07361b26a8";

// Variables globales para el servidor y la característica BLE
BLEServer* pServer = NULL;
BLECharacteristic* pCharacteristic = NULL;
bool deviceConnected = false;
bool oldDeviceConnected = false;

// Clase para gestionar los eventos del servidor BLE
class MyServerCallbacks : public BLEServerCallbacks {
    void onConnect(BLEServer* pServer) {
        deviceConnected = true;
        Serial.println("Dispositiu connectat");
    };

    void onDisconnect(BLEServer* pServer) {
        deviceConnected = false;
        Serial.println("Dispositiu desconnectat");
    }
};

// Función para iniciar el BLE
void iniciarBLE() {
    Serial.println("Iniciant BLE...");
    BLEDevice::init("MiESP32_ECG"); // Puedes cambiar el nombre si quieres
    pServer = BLEDevice::createServer();
    pServer->setCallbacks(new MyServerCallbacks());
    BLEService *pService = pServer->createService(SERVICE_UUID);

    pCharacteristic = pService->createCharacteristic(
                                         CHARACTERISTIC_UUID,
                                         BLECharacteristic::PROPERTY_READ |
                                         BLECharacteristic::PROPERTY_NOTIFY
                                       );
    pCharacteristic->addDescriptor(new BLE2902());
    pService->start();

    BLEAdvertising *pAdvertising = BLEDevice::getAdvertising();
    pAdvertising->addServiceUUID(SERVICE_UUID);
    pAdvertising->setScanResponse(true);
    BLEDevice::startAdvertising();
    Serial.println("BLE iniciat i en publicació");
}

// Función para enviar datos por BLE
void enviarBLE(const float* data, size_t length) {
    if (deviceConnected) {
        pCharacteristic->setValue((uint8_t*)data, length * sizeof(float)); // Asumimos que 'data' es un array de floats
        pCharacteristic->notify();
        Serial.println("Dades enviades per BLE");
    }
    if (!deviceConnected && oldDeviceConnected) {
        delay(500); // Un pequeño delay antes de reiniciar la publicidad
        pServer->startAdvertising();
        Serial.println("Buscant dispositiu BLE...");
        oldDeviceConnected = deviceConnected;
    }
    if (deviceConnected && !oldDeviceConnected) {
        oldDeviceConnected = deviceConnected;
    }
}
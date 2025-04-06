// BLEHandler.cpp
#include "BLEHandler.h"
#include <Arduino.h>
#include <BLEDevice.h>
#include <BLEUtils.h>
#include <BLEServer.h>
#include <BLE2902.h>

using namespace std;

const char* SERVICE_UUID = "4fafc201-1fb5-459e-8fcc-c5c9c331914b";
const char* CHARACTERISTIC_UUID = "beb5483e-36e1-4688-b7f5-ea07361b26a8";

BLEServer* pServer = NULL;
BLECharacteristic* pCharacteristic = NULL;
bool deviceConnected = false;
bool oldDeviceConnected = false;

// Clase x gestionar esdeveniments del servidor BLE
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

// Funció x iniciar el BLE
void iniciarBLE() {
    //Serial.println("Iniciant BLE...");
    BLEDevice::init("DEPRESP-32");
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
    //Serial.println("BLE iniciat i en publicació");
}

// Funció per enviar dades per BLE
void enviarFloatBLE(const float* data, size_t byteLength) {
// Pre: punter al primer element d'un array de float i la mida en bytes de l'array
    if (deviceConnected) {
        pCharacteristic->setValue((uint8_t*)data, byteLength);
        pCharacteristic->notify();
    }
    if (!deviceConnected && oldDeviceConnected) {
        delay(500);
        pServer->startAdvertising();
        oldDeviceConnected = deviceConnected;
    }
    if (deviceConnected && !oldDeviceConnected) {
        oldDeviceConnected = deviceConnected;
    }
}

void iniciarPaquetBLE(PaquetBLE* pq) {
// Pre: paquet
// Post: inicialitza
    pq->ecg.last = 0;
}

int afegirECGPaquet(PaquetBLE* pq, float valor) {
// Pre: paquet inicat
// Post: afegeix valor al paquet si hi ha espai i si no n'hi ha, envia el paquet, reinicia i afegeix el valor
    int returnval = 0;

    // Si hem emplenat el paquet BLE, enviar-lo i reiniciar el buffer
    if (pq->ecg.last == MIDA_BUF_ECG_BLE) {
        enviarFloatBLE(pq->ecg.bufferECG, MIDA_BUF_ECG_BLE * sizeof(pq->ecg.bufferECG[0]));   // Enviam el paquet x BLE
        pq->ecg.last = 0;  // reiniciem el buffer
        returnval = 1;
    }

    pq->ecg.bufferECG[pq->ecg.last] = valor;
    pq->ecg.last++;

    return returnval;   // 0 si no s'ha enviat res, 1 si sí s'ha enviat
}
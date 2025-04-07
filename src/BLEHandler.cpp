// BLEHandler.cpp
#include "BLEHandler.h"

using namespace std;

BLEServer* pServer = NULL;
BLECharacteristic* pCharacteristic = NULL;
bool deviceConnected = false;
bool oldDeviceConnected = false;

// Clase x gestionar esdeveniments del servidor BLE
class MyServerCallbacks : public BLEServerCallbacks {
    void onConnect(BLEServer* pServer) {
        deviceConnected = true;
        debugln("Dispositiu connectat");
    };

    void onDisconnect(BLEServer* pServer) {
        deviceConnected = false;
        debugln("Dispositiu desconnectat");
    }
};

// Funció x iniciar el BLE
void iniciarBLE() {
    debugln("Iniciant BLE...");
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
    debugln("BLE iniciat i en publicació");
}

void iniciarPaqBLE(PaquetBLE* pq) {
// Pre: paquet
// Post: inicialitza
    pq->ecg.last = 0;
    debugln("pBLE iniciat");
}

// Funció per enviar dades per BLE
void enviarFloatBLE(const float* data, size_t byteLength) {
// Pre: punter al primer element d'un array de float i la mida en bytes de l'array
    if (deviceConnected) {
        pCharacteristic->setValue((uint8_t*)data, byteLength);
        pCharacteristic->notify();
        debug("paquet enviat")
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

int afegirECGPaquet(PaquetBLE* pq, float valor) {
// Pre: paquet inicat
// Post: afegeix valor al paquet si hi ha espai i si no n'hi ha, envia el paquet, reinicia i afegeix el valor
    int returnval = 0;

    // Si hem emplenat el paquet BLE, enviar-lo i reiniciar el buffer
    if (pq->ecg.last == BLE_MAX_BUF_ECG) {
        enviarFloatBLE(pq->ecg.bufferECG, BLE_MAX_BUF_ECG * sizeof(pq->ecg.bufferECG[0]));   // Enviam el paquet x BLE
        pq->ecg.last = 0;  // reiniciem el buffer
        returnval = 1;
    }

    pq->ecg.bufferECG[pq->ecg.last] = valor;
    pq->ecg.last++;

    return returnval;   // 0 si no s'ha enviat res, 1 si sí s'ha enviat
}
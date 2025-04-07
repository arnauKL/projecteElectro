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

PaquetBLE crearPaquetBLE() {
// Pre: paquet
// Post: inicialitza el contingut
    PaquetBLE pq;
    pq.nEl = 0;                // iniciem a 0 elements
    pq.PNS = 0.0;             // aquests 3 valors seran 0 fins que es calculin
    pq.SNS = 0.0;
    pq.estres = 0.0;
    debugln("paquet BLE creat");
    return pq;
}

// Funció per enviar dades per BLE
void enviarFloatBLE(const float* buf, size_t byteLength) {
// Pre: punter al primer element d'un array de float i la mida en bytes de l'array
    if (deviceConnected) {
        pCharacteristic->setValue((uint8_t*)buf, byteLength);
        pCharacteristic->notify();
        debug("paquet enviat");
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

int afegirDadesPaquet(PaquetBLE* pq, float valorECG, float valorRES) {
// Pre: paquet inicat
// Post: afegeix valor al paquet si hi ha espai i si no n'hi ha, envia el paquet, reinicia i afegeix el valor
    int returnval = 0;

    // Si hem emplenat el paquet BLE, enviar-lo i reiniciar el buffer
    if (pq->nEl == BLE_MAX_BUF_ECG or pq->nEl == BLE_MAX_BUF_RES) {
        // Això és terrible però crec que funciona: assumint que la memòria del paquet està contínua, podem enviar-ho tot calculant la mida de tot junt
        enviarFloatBLE(pq->bufferECG, MIDA_BLE_PCKT);   // Enviam el paquet x BLE
        pq->nEl = 0;  // reiniciem el buffer
        returnval = 1;
    }

    pq->bufferECG[pq->nEl] = valorECG;
    pq->bufferRES[pq->nEl] = valorRES;
    pq->nEl++;

    debugln("dades afegides al paq");

    return returnval;   // 0 si no s'ha enviat res, 1 si sí s'ha enviat
}
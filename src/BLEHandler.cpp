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


PaquetBLE_U crearPaquetBLE_U() {
    PaquetBLE_U paquet;
    paquet.p.nEl = 0;
    paquet.p.nEl = 0;       // iniciem a 0 elements
    paquet.p.PNS = 0.0;     // aquests 3 valors seran 0 fins que es calculin
    paquet.p.SNS = 0.0;
    paquet.p.estres = 0.0;
    return paquet;
}

// Funció per enviar dades per BLE
void enviarBytesBLE(uint8_t* buf, size_t byteLength) {
// Pre: punter al primer element d'un array de float i la mida en bytes de l'array
    if (deviceConnected) {
        pCharacteristic->setValue(buf, byteLength);
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

int afegirDadesPaquet(PaquetBLE_U* paq_u, float valorECG, float valorRES) {
// Pre: paquet inicat
// Post: afegeix valor al paquet si hi ha espai i si no n'hi ha, envia el paquet, reinicia i afegeix el valor
    int returnval = 0;

    // Si hem emplenat el paquet BLE, enviar-lo i reiniciar el buffer
    if (paq_u->p.nEl == BLE_MAX_BUF_ECG or paq_u->p.nEl == BLE_MAX_BUF_RES) {
        enviarBytesBLE(paq_u->bytes, MIDA_TOTAL_PAQUET_BYTES);  // Enviam el paquet x BLE
        paq_u->p.nEl = 0;  // reiniciem el buffer
        returnval = 1;
    }

    paq_u->p.bufferECG[paq_u->p.nEl] = valorECG;
    paq_u->p.bufferRES[paq_u->p.nEl] = valorRES;
    paq_u->p.nEl++;

    debugln("dades afegides al paq");

    return returnval;   // 0 si no s'ha enviat res, 1 si sí s'ha enviat
}
#ifndef BLE_HANDLER_H_
#define BLE_HANDLER_H_

#include <BLEServer.h>
#include <BLECharacteristic.h>
#include "config.h"


typedef struct {
// Estructura per guardar els paquets de BLE. Per ara només conté l'ECG
    struct {
        float bufferECG[MIDA_BUF_ECG_BLE];  // Crec q això canviarà a ser `uint` quan tinguem les dades de l'ADS (Seria més eficient tb)
        int last;                           // primer espai lliure
    } ecg;
} PaquetBLE;

// UUIDs pel nostre servei i característica BLE
extern const char* SERVICE_UUID;
extern const char* CHARACTERISTIC_UUID;
extern bool deviceConnected;

// Funcions
void iniciarBLE();
void enviarFloatBLE(const float* data, size_t length);
int afegirECGPaquet(PaquetBLE* pq, float valor);

#endif
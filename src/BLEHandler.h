#ifndef BLE_HANDLER_H_
#define BLE_HANDLER_H_

#include <BLEServer.h>
#include <BLECharacteristic.h>
#include <Arduino.h>
#include <BLEDevice.h>
#include <BLEUtils.h>
#include <BLEServer.h>
#include <BLE2902.h>
#include "config.h"

typedef struct {
// Estructura per guardar els paquets de BLE. Per ara només conté l'ECG
    float bufferECG[BLE_MAX_BUF_ECG];  // Crec q això canviarà a ser `uint` quan tinguem les dades de l'ADS (Seria més eficient tb)
    float bufferRES[BLE_MAX_BUF_RES];
    float SNS;      // Activació del simpàtic
    float PNS;      // Activació del parasimpàtic
    float estres;   // Estrés calculat
    int nEl;        // nombre d'elements als buffers
} PaquetBLE;

// mida en bytes x poder fer la union
#define MIDA_TOTAL_PAQUET_BYTES (sizeof(PaquetBLE)-sizeof(int)) // l'int que restem és el del nombre d'elements, que no interessa enviar

// union per fer el trukasso per no haver de copiar dades B)
typedef union {
    PaquetBLE p;
    float bytes[MIDA_TOTAL_PAQUET_BYTES]; // Dona accés a tot el paquet en forma de bytes per enviar x BLE
} PaquetBLE_U;

// Funcions
PaquetBLE_U crearPaquetBLE_U();
void iniciarBLE();
void enviarBytesBLE(const float* data, size_t length);
void afegirDadesPaquet(PaquetBLE_U* pq, float valorECG, float valorRES);



#endif
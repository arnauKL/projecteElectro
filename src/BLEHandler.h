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
    int nEl;        // nombre d'elements als buffers
    float SNS;      // Activació del simpàtic
    float PNS;      // Activació del parasimpàtic
    float estres;   // Estrés calculat
} PaquetBLE;

// Funcions
PaquetBLE crearPaquetBLE();
void iniciarBLE();
void enviarFloatBLE(const float* data, size_t length);
int afegirDadesPaquet(PaquetBLE* pq, float valorECG, float valorRES);

#endif
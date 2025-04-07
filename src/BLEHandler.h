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
    struct {
        float bufferECG[BLE_MAX_BUF_ECG];  // Crec q això canviarà a ser `uint` quan tinguem les dades de l'ADS (Seria més eficient tb)
        int last;                           // primer espai lliure
    } ecg;
} PaquetBLE;

/*
typedef struct {
// Estructura per guardar els paquets de BLE. Per ara només conté l'ECG
    struct {
        float bufferECG[BLE_MAX_BUF_ECG];  // Crec q això canviarà a ser `uint` quan tinguem les dades de l'ADS (Seria més eficient tb)
        int last;                           // primer espai lliure
    } ecg;
    struct {
        float bufferRES[BLE_MAX_BUF_RES];
        int last;
    } res;
    float SNS;
    float PNS;
    float estres;
} PaquetBLE;*/

// Funcions
void iniciarBLE();
void iniciarPaqBLE(PaquetBLE* pq);
void enviarFloatBLE(const float* data, size_t length);
int afegirECGPaquet(PaquetBLE* pq, float valor);

#endif
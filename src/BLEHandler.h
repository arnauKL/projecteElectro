#ifndef BLE_HANDLER_H_
#define BLE_HANDLER_H_

#include <BLEServer.h>
#include <BLECharacteristic.h>

// UUIDs para nuestro servicio y característica BLE
extern const char* SERVICE_UUID;
extern const char* CHARACTERISTIC_UUID;

// Variable externa para indicar si un dispositivo está conectado
extern bool deviceConnected;

// Declaración de las funciones que definiremos en BLEHandler.cpp
void iniciarBLE();
void enviarBLE(const float* data, size_t length);

#endif
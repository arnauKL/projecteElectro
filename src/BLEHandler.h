#ifndef BLE_HANDLER_H_
#define BLE_HANDLER_H_

#include <BLEServer.h>
#include <BLECharacteristic.h>

// UUIDs pel nostre servei i característica BLE
extern const char* SERVICE_UUID;
extern const char* CHARACTERISTIC_UUID;
extern bool deviceConnected;

// Funcions
void iniciarBLE();
void enviarFloatBLE(const float* data, size_t length);

#endif
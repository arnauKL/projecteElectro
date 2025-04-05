#include <Arduino.h>
#include "ECG.h"
#include "BLEHandler.h"
#include "config.h"

ECGgen ecg;

void setup() {
    Serial.begin(BAUD_RATIO);
    delay(1000);
    iniciarBLE(); // Iniciem la comunicació x BLE
}

void loop() {
    float ecg_value = ecg.generarSenyalECG();

    float dataToSend[1] = {ecg_value};
    enviarBLE(dataToSend, 1);   // Enviam el valor x BLE

    Serial.println(ecg_value);  // Mostrar el senyal ECG tb x serial (debug)

    ecg.temps += ecg.dt; // Incrementar el temps per al següent mostreig de l'ECG
    delay(1000 / ecg.fs); // Retard per ajustar la freqüència de mostreig de l'ECG
}
/// Codi per simular un senyal ECG amb comunicació BLE

#include <Arduino.h>
#include "ECG.h"
#include "BLEHandler.h"
#include "config.h"

// Constants per al bucle principal
//const float STEP_SIZE = 0.1; // Increment de la variable temps
//const float AMPLITUD = 100.0; // Amplitud del senyal (potser ho vols moure a ECG.h/cpp si és específic de l'ECG)

float temps_main = 0.0; // Variable temps per al bucle principal (separada de la de l'ECG)

void setup() {
    Serial.begin(BAUD_RATIO);
    delay(1000);
    iniciarBLE(); // Iniciem la comunicació x BLE
    initECG();
}

void loop() {
    float ecg_value = generarSenyalECG();

    float dataToSend[1] = {ecg_value};
    enviarBLE(dataToSend, 1);   // Enviam el valor x BLE

    Serial.println(ecg_value);  // Mostrar el senyal ECG tb x serial (debug)

    temps += dt; // Incrementar el temps per al següent mostreig de l'ECG
    delay(1000 / fs_ECG); // Retard per ajustar la freqüència de mostreig de l'ECG
}
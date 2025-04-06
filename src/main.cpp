#include <Arduino.h>
#include "ECG.h"
#include "BLEHandler.h"
#include "config.h"
//#include "CoaCircular.h"

//----------------------------------------------------------------------

ECGgen ecgGen;      // Generador d'ECG x tests
PaquetBLE pBLE;

float umbral = 0.6;          // Llindar simple (s'hauria de fer automàtic)
unsigned long tempsUltimPic = 0;

float anterior = 0;
float actual = 0;
float seguent = 0;
float llindar = 0.7;

bool detectarPicR(float mostraNova, float mostraAnt, float mostraSeg, float llindar) {
// Pre: "buffer" cutre; Post: retorna cert si hi ha un pic
    return (mostraAnt < mostraNova &&
            mostraNova > mostraSeg &&
            mostraNova > llindar);
}

void setup() {
    Serial.begin(BAUD_RATIO);
    delay(1000);
    iniciarBLE(); // Iniciem la comunicació x BLE
}

//----------------------------------------------------------------------

void loop() {    
    float ecg_value = ecgGen.generarSenyalECG();        // 'rebem' dades d'ECG

    anterior = actual;
    actual = seguent;
    seguent = ecg_value;

    // Detectar pic R
    if (detectarPicR(actual, anterior, seguent, llindar)) {
        unsigned long rr = millis() - tempsUltimPic;
        tempsUltimPic = millis();

        Serial.print("interval RR (ms): ");
        Serial.println(rr);
    }

    // afegir el valor al paquet BLE
    afegirECGPaquet(&pBLE, ecg_value);
    
    //Serial.println(ecg_value);  // Mostrar el senyal ECG tb x serial (debug)

    ecgGen.temps += ecgGen.dt; // Incrementar el temps per al següent mostreig de l'ECG
    delay(1000 / ecgGen.fs); // Retard per ajustar la freqüència de mostreig de l'ECG
}
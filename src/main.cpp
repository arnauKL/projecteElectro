#include <Arduino.h>
#include "ECG.h"
#include "BLEHandler.h"
#include "config.h"

// TODO: afegir específics de BLE al fitxer BLEHandler.h/.c

//----------------------------------------------------------------------

typedef struct {
    float buffer[MAX_BUFFER_ECG];
    int first;
    int last;   // primer espai lliure
    int capacitat;
} CoaCircular;

typedef struct {
// Estructura per guardar els paquets de BLE. Per ara només conté l'ECG
    struct {
        float bufferECG[MIDA_BUF_ECG_BLE];  // Crec q això canviarà a ser `uint` quan tinguem les dades de l'ADS (Seria més eficient tb)
        int last;                           // primer espai lliure
    } ecg;
} PaquetBLE;

ECGgen ecgGen;      // Generador d'ECG x tests
CoaCircular coaEcg; // Estructura de coa circular per guardar dades que arriben d'ECG
PaquetBLE pBLE;

//----------------------------------------------------------------------

void iniciarCoa(CoaCircular* coa) {
    coa->first = 0;
    coa->last = 0;
    coa->capacitat = MAX_BUFFER_ECG;    // No m'agrada, s'hauria de canviar
}

void iniciarPaquetBLE(PaquetBLE* pq) {
    pq->ecg.last = 0;
}

void afegirValor(CoaCircular* coa, float valor) {
// Pre: coa i valor
// Post: afegeix l'element al final de la coa

    // afegim el valor
    coa->buffer[coa->last] = valor;

    // Incrementem
    coa->last++;

    // Si ens passem, fem la volta
    if (coa->last > coa->capacitat) { coa->last = 0; }
}

void setup() {
    Serial.begin(BAUD_RATIO);
    delay(1000);
    iniciarBLE(); // Iniciem la comunicació x BLE
}

void loop() {

    // Si hem emplenat el paquet BLE, enviar-lo i reiniciar el buffer
    if (pBLE.ecg.last == MIDA_BUF_ECG_BLE) {
        enviarFloatBLE(pBLE.ecg.bufferECG, MIDA_BUF_ECG_BLE * sizeof(pBLE.ecg.bufferECG[0]));   // Enviam el paquet x BLE
        pBLE.ecg.last = 0;  // reiniciem el buffer
    }
    
    float ecg_value = ecgGen.generarSenyalECG();        // 'rebem' dades d'ECG
    afegirValor(&coaEcg, ecg_value);                    // guardem al buffef de dades ECG

    // afegir el valor al paquet BLE
    // TODO: ficar això en una funció x evitar errors d'accés
    pBLE.ecg.bufferECG[pBLE.ecg.last] = ecg_value;
    pBLE.ecg.last++;
    
    //Serial.println(ecg_value);  // Mostrar el senyal ECG tb x serial (debug)

    ecgGen.temps += ecgGen.dt; // Incrementar el temps per al següent mostreig de l'ECG
    delay(1000 / ecgGen.fs); // Retard per ajustar la freqüència de mostreig de l'ECG
}
#include <Arduino.h>
#include "driver/timer.h"
#include "BLEHandler.h"
#include "FFThandler.h"
#include "SPIhandler.h"
#include "RR.h"
#include "config.h"

//-------------------------- Variables globals --------------------------

BufRR bufferRR = crearBufRR();              // Buffer x guardar els intervals RR
BufRR bufferTimeRR = crearBufRR();          // Buffer x guardar els temps dels intervals RR
BufRR bufferInterRR = crearBufRR();         // Buffer x guardar les interpolacions dels intervals RR
BufRR bufferInterTimeRR = crearBufRR();     // Buffer x guardar els temps de les interpolacions fetes

PaquetBLE_U pBLE_U = crearPaquetBLE_U();    // Paquet per guardar les dades i enviar-les per BLE

// Variable per rebre mostres (volatile xq pot ser canviada en un interrupt)
volatile bool newSampleAvailable = false;

// Variables per detectar els pics RR:
unsigned long tempsUltimPic = 0;
float anterior = 0;     // Variables que fan de 'buffer' per trobar pics
float actual = 0;   
float seguent = 0;
float llindar = 0.7;    // Llindar automàtic ( anirà variant segons els darrers pics detectats --> detectarPicRdinamic() )

// Variable per controlar quan hem interpolat i FFT
bool interpolationDone = false;
bool FFTdone = false;

//-------------------------- Setup de l'interrupt --------------------------
void IRAM_ATTR onDRDY() {
// Quan DRDY s'activi, l'interrupt posa "new_sample_available" a cert x després llegir al loop()
    newSampleAvailable = true;
}

//-------------------------- Funcions per parsejar --------------------------
float convertirAmV(uint32_t sampleRaw) {
// Passa els valors donats de l'ADS a mV
    return (sampleRaw * (2.0 * V_REF / GAIN)) / (2.0 * TWO_POW_23) * 1000.0;
}
// TODO: comprovar aquesta fòrmula

//-------------------------- Programa principal --------------------------
void setup() {

    // -------------- Setup del Serial per debugar --------------
    debugbegin(BAUD_RATIO);
    debugln("Serial iniciat");
    delay(1000);

    // -------------- Setup del BLE --------------
    iniciarBLE(); // Iniciem la comunicació x BLE
    debugln("BLE iniciat");

    // -------------- Setup del SPI --------------
    SPIsetup();
    debugln("SPI iniciat");

    // -------------- Setup de l'interrupt per llegir dades --------------
    // Configurar l'interrupt: quan dataReady estigui HIGH

    pinMode(ADS1292_DRDY_PIN, INPUT);
    attachInterrupt(digitalPinToInterrupt(ADS1292_DRDY_PIN), onDRDY, FALLING); // quan DRDY baixa s'activa l'interrupt 'onDRDY'
}


void loop() {

    if(newSampleAvailable) {

        // -------------- Arribada de dades --------------
        // Anar a cercar les dades de l'ADS:
        noInterrupts(); // Apagar interrupts per copiar dades en memòria compartida
        newSampleAvailable = false;
        interrupts(); // Re-encendre interrupts

        uint8_t spiData[9];
        ADS1292R_Data sensorData;
      
        readADS1292RData(spiData);
        sensorData = parseADS1292RData(spiData);

        // Convertir a floats (mV)
        float ecgValue = convertirAmV(sensorData.ecgSample);
        float resValue = convertirAmV(sensorData.resSample);

        debug("dades rebudes: ");
        debug(ecgValue);
        debug(", ");
        debugln(resValue);
    
        // -------------- Detecció de pics R --------------
        // sliding buffer per trobar pics de l'ECG
        anterior = actual;
        actual = seguent;
        seguent = ecgValue;
    
        if (detectarPicRdinamic(actual, anterior, seguent, llindar)) {
            unsigned long rr = millis() - tempsUltimPic;
            tempsUltimPic = millis();


            if (rr >= MIN_INTERVAL_PICS) {  // Marge per no detectar el mateix pic dos cops (soroll)
                debug("interval RR (ms): ");
                debugln(rr);
        
                // afegim al vector d'intervals RR
                afegirRR(&bufferRR, rr);    // Aquesta funció ignora les dades si ja està ple el buffer (TODO: s'hauria de canviar)
                afegirRR(&bufferTimeRR, tempsUltimPic); // Afegim el temps al seu vector
            }
        }
    
        // -------------- Intepolació i FFT --------------

        // Interpolem 
        if(millis() > 150000 && !interpolationDone) { // Comencem a fer interpolacions a partir de dos minuts d'haver pres dades

            interpolar(&bufferInterRR, &bufferRR, &bufferInterTimeRR, &bufferTimeRR);
            interpolationDone = true;
        
        }
    
        // FFT
        if(interpolationDone && !FFTdone){
    
            FFTbuffer bufferFFT = crearFFTbuffer();     // Creem un objecte que ens permet gestionar la fft
            setArrays(bufferInterRR.vec, &bufferFFT);   // Introduim les dades de les interpolacions
            calcularFFT(&bufferFFT);
            computeStress(&bufferFFT, pBLE_U.p.SNS, pBLE_U.p.PNS, pBLE_U.p.estres);
            FFTdone = true;
        }
    
        // -------------- BLE --------------
        afegirDadesPaquet(&pBLE_U, ecgValue, resValue);   // Afegeix al paquet BLE i envia si està ple
    }
}
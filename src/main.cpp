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
inline float convertirAmVecg(uint32_t sampleRaw) {  // inline x veure si el compilador l'optimitza
// Passa els valors d'ECG l'ADS a mV
    return 1000.0 * (sampleRaw * (V_REF / (GAIN_ECG * TWO_POW_23)));    // *1000 per passar a mV
}

inline float convertirAmVres(uint32_t sampleRaw) {
// Passa els valors de respiració de l'ADS a mV
    return 1000.0 * (sampleRaw * (V_REF / (GAIN_RES * TWO_POW_23)));
}

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
        debugln("DRDY LOW");

        uint8_t spiData[9];
        ADS1292R_Data sensorData;
      

        readADS1292RData(spiData);
        debugln("dades llegides de SPI");
        sensorData = parseADS1292RData(spiData);

        debugln("dades parsejades");        

        // Convertir a floats (mV)
        float ecgValue = convertirAmVecg(sensorData.ecgSample);
        float resValue = convertirAmVres(sensorData.resSample);
        debugln("dades convertides a mV:")

        //debug(sensorData.ecgSample);
        debug(ecgValue);
        debug(", ");
        //debugln(sensorData.resSample);
        debugln(resValue);

    
        // -------------- Detecció de pics R --------------
        // sliding buffer per trobar pics de l'ECG
        anterior = actual;
        actual = seguent;
        seguent = ecgValue;
        //seguent = sensorData.ecgSample;

        if (detectarPicRdinamic(actual, anterior, seguent, llindar)) {
            unsigned long rr = millis() - tempsUltimPic;
            tempsUltimPic = millis();

            debugln("pic trobat");

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
        if (millis() > 150000 && !interpolationDone) { // Comencem a fer interpolacions a partir de dos minuts d'haver pres dades

            interpolar(&bufferInterRR, &bufferRR, &bufferInterTimeRR, &bufferTimeRR);
            interpolationDone = true;
        
        }
    
        // FFT
        if (interpolationDone && !FFTdone){
    
            FFTbuffer bufferFFT = crearFFTbuffer();     // Creem un objecte que ens permet gestionar la fft
            setArrays(bufferInterRR.vec, &bufferFFT);   // Introduim les dades de les interpolacions
            calcularFFT(&bufferFFT);
            computeStress(&bufferFFT, pBLE_U.p.SNS, pBLE_U.p.PNS, pBLE_U.p.estres);
            FFTdone = true;
        }
    
        // -------------- BLE --------------
        afegirDadesPaquet(&pBLE_U, ecgValue, resValue);   // Afegeix al paquet BLE i envia si està ple
        //afegirDadesPaquet(&pBLE_U, sensorData.ecgSample, sensorData.resSample);   // Afegeix al paquet BLE i envia si està ple
        debugln("dades afegides al paq BLE");
    }
}
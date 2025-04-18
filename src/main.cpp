#include <Arduino.h>
#include "Sim.h"
#include "BLEHandler.h"
#include "config.h"
#include "driver/timer.h"
#include "RR.h"
#include "FFThandler.h"

// Comentari per provar si m funcionen els git push

//-------------------------- Variables globals --------------------------

Sim gen; // Generador d'ECG x tests
BufRR bufferRR = crearBufRR(); // Buffer x guardar els intervals RR
BufRR bufferTimeRR = crearBufRR(); // Buffer x guardar els temps dels intervals RR
BufRR bufferInterRR = crearBufRR(); // Buffer x guardar les interpolacions dels intervals RR
BufRR bufferInterTimeRR = crearBufRR(); // Buffer x guardar els temps de les interpolacions fetes

// Paquet per guardar les dades i enviar-les per BLE
PaquetBLE_U pBLE_U = crearPaquetBLE_U();

// Variables per rebre mostres (volatile xq poden ser canviades en un interrupt)
volatile bool new_sample_available = false;
volatile float ecg_sample = 0.0;
volatile float res_sample = 0.0;

// Variables per detectar els pics RR:
unsigned long tempsUltimPic = 0;
float anterior = 0;
float actual = 0;
float seguent = 0;
float llindar = 0.7;    // Llindar simple (s'hauria de fer automàtic)

// Variable per controloar quan hem interpolat i FFT
bool interpolationDone = false;
bool FFTdone = false;

//-------------------------- Setup de l'interrupt --------------------------

hw_timer_t* timer;

// Funció per l'interrupt de mostreig de dades ECG
void IRAM_ATTR onTimer() {
    new_sample_available = true;
}

//-------------------------- Programa principal --------------------------

void setup() {
    Serial.begin(BAUD_RATIO);
    delay(1000);
    iniciarBLE(); // Iniciem la comunicació x BLE

    // Config pel timer (d'exemple)
    timer = timerBegin(0, 80, true);                // 80 prescaler -> 1 tick
    debugln("timer iniciat");
    timerAttachInterrupt(timer, &onTimer, true);    // Connectem l'interrupt
    debugln("interrupt configurat");
    timerAlarmWrite(timer, 1000000 / gen.fs, true); // Configurar alarma al fs de l'ECG. Va 10 vegades més lent del que hauria xq no peti jeje
    debugln("alarma OK");
    timerAlarmEnable(timer);
    debugln("Timer i interrupt configurat");
}

void loop() {

    if(new_sample_available) {

        noInterrupts(); // Apagar interrupts per copiar dades en memòria compartida
        float ecg_value = ecg_sample;
        float res_value = res_sample;
        new_sample_available = false;
        interrupts();   // Re-encendre interrupts

        //debug("dades rebudes: ");
        //debug(ecg_value);
        //debug(", ");
        //debugln(res_value);
    
        // Buffer cutre per trobar pics de l'ECG
        anterior = actual;
        actual = seguent;
        seguent = ecg_value;
    
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
    
        // Interpolem 
        if(millis() > 150000 && !interpolationDone) { // Comencem a fer interpolacions a partir de dos minuts d'haver pres dades
            //        ^^^^^^ we don't do that here
            interpolar(&bufferInterRR, &bufferRR, &bufferInterTimeRR, &bufferTimeRR);
            interpolationDone = true;
        
        }
    
        // FFT
        if(interpolationDone && !FFTdone){
    
            FFTbuffer bufferFFT = crearFFTbuffer(); // Creem un objecte que ens permet gestionar la fft
            setArrays(bufferInterRR.vec, &bufferFFT); // Introduim les dades de les interpolacions
            calcularFFT(&bufferFFT);
            computeStress(&bufferFFT, pBLE_U.p.SNS, pBLE_U.p.PNS, pBLE_U.p.estres);
            FFTdone = true;
        }
    
        // Afegir al paquet BLE i enviar si està ple
        afegirDadesPaquet(&pBLE_U, ecg_value, res_value);

        // Generem la següent dada (això a la versió final no existirà)
        ecg_sample = gen.generarSenyalECG();  // Generar mostra (simula mostra arribada de l'ADS)
        res_sample = gen.generarSenyalRES();  // Generar mostra d'ECG (")
    }
}
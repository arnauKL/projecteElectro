#include <Arduino.h>
#include "Sim.h"
#include "BLEHandler.h"
#include "config.h"
//#include "CoaCircular.h"
#include "driver/timer.h"
#include "RR.h"
#include "FFThandler.h"

//-------------------------- Variables globals --------------------------

Sim gen; // Generador d'ECG x tests
BufRR bufferRR = crearBufRR(); // Buffer x guardar els intervals RR
BufRR bufferTimeRR = crearBufRR(); // Buffer x guardar els temps dels intervals RR
BufRR bufferInterRR = crearBufRR(); // Buffer x guardar les interpolacions dels intervals RR
BufRR bufferInterTimeRR = crearBufRR(); // Buffer x guardar els temps de les interpolacions fetes 
PaquetBLE pBLE = crearPaquetBLE(); // Paquet per enviar dades per BLE

// Variables per rebre mostres (volatile xq poden ser canviades en un interrupt)
volatile bool new_sample_available = false;
volatile float ecg_sample = 0.0;
volatile float res_sample = 0.0;

hw_timer_t* timer = NULL;

// Funció per l'interrupt de mostreig de dades ECG
void IRAM_ATTR onTimer() {
    // No hauria de fer tot això aquesta funció xq està tardant massa. Al final això igualment desapareixerà
    ecg_sample = gen.generarSenyalECG();  // Generar mostra (simula mostra arribada de l'ADS)
    res_sample = gen.generarSenyalRES();  //TODO: Crear aquesta funció a partir del codi den carles
    new_sample_available = true;
    gen.temps += gen.dt;  // Avança temps simulat
}

// Variables per detectar els pics RR:
float umbral = 0.6; // Llindar simple (s'hauria de fer automàtic)
unsigned long tempsUltimPic = 0;
float anterior = 0;
float actual = 0;
float seguent = 0;
float llindar = 0.7;

// Variable per controloar quan hem interpolat

bool interpolationDone = false;

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
    timerAlarmWrite(timer, 10000000 / gen.fs, true); // Configurar alarma al fs de l'ECG. Va 10 vegades més lent del que hauria xq no peti jeje
    debugln("alarma OK");
    timerAlarmEnable(timer);
    debugln("Timer i interrupt configurat");
}

void loop() {
    debugln("in loop");
    if (new_sample_available) {
        noInterrupts(); // Apagar interrupts per copiar dades en memòria compartida
        float ecg_value = ecg_sample;
        float res_value = res_sample;
        new_sample_available = false;
        interrupts();   // Re-encendre interrupts

        debug("dades rebudes");
        debug(ecg_value);
        debug(", ");
        debugln(res_value);

        // Buffer cutre per trobar pics de l'ECG
        anterior = actual;
        actual = seguent;
        seguent = ecg_value;

        if (detectarPicR(actual, anterior, seguent, llindar) == 1) {
            unsigned long rr = millis() - tempsUltimPic;
            tempsUltimPic = millis();

            Serial.print("interval RR (ms): ");
            Serial.println(rr);

            // afegim al vector d'intervals RR
            afegirRR(&bufferRR, rr);    // Aquesta funció ignora les dades si ja està ple el buffer (TODO: s'hauria de canviar)
            afegirRR(&bufferTimeRR, tempsUltimPic); // Afegim el temps al seu vector
        }

        // Interpolem 
        if(millis() > 120000) {// Comencem a fer interpolacions a partir de dos minuts d'haver pres dades
            
            interpolar(&bufferInterRR, &bufferRR, &bufferInterTimeRR, &bufferTimeRR);
            interpolationDone = true;
        
        }

        // FFT
        if(interpolationDone){

            FFTbuffer bufferFFT = crearFFTbuffer(); // Creem un objecte que ens permet gestionar la fft
            setArrays(getVec(&bufferInterRR), &bufferFFT); // Introduim les dades de les interpolacions
            calcularFFT(&bufferFFT);


        }

        // Afegir al paquet BLE i enviar si està ple
        afegirDadesPaquet(&pBLE, ecg_value, res_value);
        debugln("afegides al paquet");
    }
}
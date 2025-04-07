#include <Arduino.h>
#include "ECG.h"
#include "BLEHandler.h"
#include "config.h"
//#include "CoaCircular.h"
#include "driver/timer.h"


bool detectarPicR(float mostraNova, float mostraAnt, float mostraSeg, float llindar) {
// Pre: "buffer" cutre; Post: retorna cert si hi ha un pic
    return (mostraAnt < mostraNova &&
            mostraNova > mostraSeg &&
            mostraNova > llindar);
}


//-------------------------- Variables globals --------------------------

// Buffer x guardar els intervals RR
typedef struct {
    float buf[MAX_BUFFER_RR];   // Buffer
    int nEl;                    // Nombre d'elements
} VecRR;
VecRR bufferRR;

ECGgen ecgGen; // Generador d'ECG x tests
PaquetBLE pBLE; // Paquet per enviar dades per BLE

//
volatile bool new_sample_available = false;
volatile float ecg_sample = 0.0;

// Variables per calcular interval RR
float umbral = 0.6; // Llindar simple (s'hauria de fer automàtic)
unsigned long tempsUltimPic = 0;
float anterior = 0;
float actual = 0;
float seguent = 0;
float llindar = 0.7;

hw_timer_t* timer = NULL;

// Funció per l'interrupt de mostreig de dades ECG
void IRAM_ATTR onTimer() {
    ecg_sample = ecgGen.generarSenyalECG();  // Generar mostra (simula mostra arribada de l'ADS)
    new_sample_available = true;
    ecgGen.temps += ecgGen.dt;  // Avança temps simulat
}

//----------------------------------------------------------------------

void setup() {
    Serial.begin(BAUD_RATIO);
    delay(1000);
    iniciarBLE(); // Iniciem la comunicació x BLE

    iniciarPaqBLE(&pBLE);

    // Config pel timer
    timer = timerBegin(0, 80, true);                    // 80 prescaler -> 1 tick
    debugln("timer iniciat OK");
    timerAttachInterrupt(timer, &onTimer, true);    // Connectem l'interrupt
    debugln("interrupt iniciat OK");
    timerAlarmWrite(timer, 1000000 / ecgGen.fs, true); // Configurar alarma al fs de l'ECG
    debugln("alarma OK");
    timerAlarmEnable(timer);
    debugln("Timer i interrupt configurat");
}

void loop() {

    if (new_sample_available) {
        debugln("nova sample rebuda");
        noInterrupts(); // Apagar interrupts per copiar dades en memòria compartida
        float ecg_value = ecg_sample;
        new_sample_available = false;
        interrupts();   // Re-encendre interrupts

        // Buffer cutre
        anterior = actual;
        actual = seguent;
        seguent = ecg_value;

        // Detect R-peak
        if (detectarPicR(actual, anterior, seguent, llindar)) {
            unsigned long rr = millis() - tempsUltimPic;
            tempsUltimPic = millis();

            Serial.print("interval RR (ms): ");
            debugln(rr);

            // afegim al vector d'intervals RR
            if (bufferRR.nEl < MAX_BUFFER_RR) {
                bufferRR.buf[bufferRR.nEl++] = rr;
            }
            // altrament ignoram les dades (TODO: no s'hauria de fer, haurem d veure què fer)
        }

        // Afegir al paquet BLE i enviar si està ple
        afegirECGPaquet(&pBLE, ecg_value);
    }
}
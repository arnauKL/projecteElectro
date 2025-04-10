#ifndef CONFIG_H_
#define CONFIG_H_

// Fitxer que defineix característiques generals


// Serial
#define BAUD_RATIO  115200

// Dades
#define MAX_BUFFER_ECG  10   // 10 dades
#define MAX_BUFFER_RR   512  // Dades que guardem de RR per fer la FFT (hauria de ser una potència de 2)
#define BLE_MAX_BUF_ECG 30      // Nombre de dades d'ECG que guardem i enviem alhora en un paquetBLE
#define BLE_MAX_BUF_RES 30      // Nombre de dades de Resp q enviem elhora en un paquet

// Interpolació i FFT
#define TEMPS_INTERPOLACIONS 250.0 // en ms, f = 4 Hz
#define SAMPLE_FREQ 4.0 // f = 4 Hz
#define SN_LIMIT 0.15 // Freq. a la que canviem de sistema nerviòs simpàtic a parasimpàtic

// BLE
#define SERVICE_UUID            "4fafc201-1fb5-459e-8fcc-c5c9c331914b"
#define CHARACTERISTIC_UUID     "beb5483e-36e1-4688-b7f5-ea07361b26a8"

// ...

// truquillo
#define DEBUG 1     // Si posem 1, es fan els Serial.prints i si posem 0 s'eliminen

#if DEBUG == 1
#define debug(x) Serial.print(x)
#define debugln(x) Serial.println(x)
#else
#define debug(x)
#define debugln(x)
#endif

#endif
#ifndef CONFIG_H_
#define CONFIG_H_

// Fitxer que defineix característiques generals


// Serial
#define BAUD_RATIO  115200

// Dades
#define MAX_BUFFER_ECG  10   // 10 dades
#define MAX_BUFFER_RR   20  // Dades que guardem de RR per fer la FFT
#define BLE_MAX_BUF_ECG 30      // Nombre de dades d'ECG que guardem i enviem alhora en un paquetBLE
#define BLE_MAX_BUF_RES 30      // Nombre de dades de Resp q enviem elhora en un paquet

#define MIDA_BLE_PCKT ((BLE_MAX_BUF_ECG + BLE_MAX_BUF_RES + 3) * __SIZEOF_FLOAT__) // El 3 és dels 3 floats extres (SNS, PNS i estres)

// BLE
#define SERVICE_UUID            "4fafc201-1fb5-459e-8fcc-c5c9c331914b"
#define CHARACTERISTIC_UUID     "beb5483e-36e1-4688-b7f5-ea07361b26a8"

// ...

// truquillo
#define DEBUG 0     // Si posem 1, es fan els Serial.prints i si posem 0 s'eliminen

#if DEBUG == 1
#define debug(x) Serial.print(x)
#define debugln(x) Serial.println(x)
#else
#define debug(x)
#define debugln(x)
#endif

#endif
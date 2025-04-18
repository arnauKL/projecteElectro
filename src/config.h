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


// Detecció de pics R
#define THRESHOLD_BIAS      0.8f
#define SOROLL_SIM          0.1     // Soroll (percent) del senyal simulat (per provar robustesa de l'algo d detecció d pics)
#define MIN_INTERVAL_PICS   200     // ms mínims entre un pic i el següent (potser enlloc de ms és millor contar mostres?)

// Interpolació i FFT
#define TEMPS_INTERPOLACIONS 250.0 // en ms, f = 4 Hz
#define SAMPLE_FREQ 4.0 // f = 4 Hz
#define SN_LIMIT 0.15 // Freq. a la que canviem de sistema nerviòs simpàtic a parasimpàtic

// BLE
#define SERVICE_UUID            "4fafc201-1fb5-459e-8fcc-c5c9c331914b"
#define CHARACTERISTIC_UUID     "beb5483e-36e1-4688-b7f5-ea07361b26a8"

// SPI 
// Comandes de l'ADS1292R
#define CMD_READ_REG 0x20
#define CMD_STOP 0x0A
#define CMD_SDATAC 0x11
#define CMD_RESET 0x06

// Pins SPI
#define ADS1292_MISO 19
#define ADS1292_MOSI 23
#define ADS1292_CS_PIN 5
#define ADS1292_SCK 18
#define ADS1292_DRDY_PIN 4
#define ADS1292_START_PIN 16
#define ADS1292_PWDN_PIN 17

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
# Projecte d'Electrònica 2025.

Es tracta de programar una ESP32 per funcionar amb l'ADS1292R i fer:

- Configurar l'ADS1292R
- Rebre dades per SPI de
    - Respiració
    - ECG
- Calcular l'interval entre els pics R de l'ECG
- Interpolació lineal per fer fft
- Calcular la FFT per trobar HRV
- Trobar el nivell d'activitat del sistema simpàtic i parasimpàtic
- Enviar les dades d'ECG, respiració i HRV per BLE

## Estructura:

- `main.cpp` conté el bloc principal, s'inicien les variables globals i l'ESP, i entra en el `loop()`.
- `config.h` conté totes les constants i paràmetres que es fan servir a les altres classes.
- *Mòduls funcionals*: els fitxers restants contenen les funcions i estructures emprades per cada funció.

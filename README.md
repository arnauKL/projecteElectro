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
- Enviar les dades d'ECG, respiració i HRV

## Explicacions per l'informe:

### `SimulaECG.cpp` --> `ECGgen`

Hem reconvertit el fitxer `SimulaECG.cpp` proporcionat en la classe `ECGgen` per tal de fer-la servir com a un generador de dades de l'ECG a dins del nostre codi.
La idea és substituir la classe amb dades de l'ADS rebudes per SPI un cop tinguem la placa. 

D'aquesta classe només n'extreurem dades, per tant ens permetrà començar a provar els algorismes per trobar els pics RR, fer la FFT, etc sense haver d'haver fet la part d'SPI encara.


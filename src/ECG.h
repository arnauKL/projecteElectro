#ifndef ECG_H_
#define ECG_H_

// Constants per simular l'ECG
const float fs_ECG = 500.0;
const float duration = 10.0;
const float heart_rate = 60.0;

// Paràmetres de la forma de l'ECG
const float P_amp = 0.1;
const float QRS_amp = 1.0;
const float T_amp = 0.3;

const float P_duration = 0.1;
const float QRS_duration = 0.1;
const float T_duration = 0.2;

// Declaració de variables (les definirem en el .cpp)
extern float temps;
extern float dt;
extern float rr_interval;
extern float rr_variability;

// Declaració de funcions (les definirem en el .cpp)
void initECG();
float generarSenyalECG();
float gaussian_wave(float t, float amplitude, float duration);
float qrs_wave_with_negatives(float t, float amplitude, float duration);

#endif
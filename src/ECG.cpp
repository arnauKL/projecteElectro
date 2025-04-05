// ECG.cpp
#include "ECG.h"
#include <cmath> // Necesario para pow() y exp()
#include <Arduino.h> // Necesario para randomSeed() y analogRead()

// Definició de variables (sense 'extern' aquí)
float temps = 0.0;
float dt = 1.0 / fs_ECG;
float rr_interval = 60.0 / heart_rate;
float rr_variability = 0.05;

// Funció per generar una ona gaussiana per a les onades P, QRS i T
float gaussian_wave(float t, float amplitude, float duration) {
  return amplitude * exp(-0.5 * pow(t / (duration / 2.0), 2));
}

// Funció per simular el complex QRS amb una part negativa (Q i S)
float qrs_wave_with_negatives(float t, float amplitude, float duration) {
  float positive_peak = gaussian_wave(t, amplitude, duration / 2.0);
  float negative_peak = gaussian_wave(t - duration / 2.0, -amplitude / 2.0, duration / 2.0);
  return positive_peak + negative_peak;
}

float generarSenyalECG() {
  float p_wave = 0.0;
  float qrs_wave = 0.0;
  float t_wave = 0.0;

  if (temps < P_duration) {
    p_wave = gaussian_wave(temps, P_amp, P_duration);
  }

  if (temps >= P_duration && temps < P_duration + QRS_duration) {
    qrs_wave = qrs_wave_with_negatives(temps - P_duration, QRS_amp, QRS_duration);
  }

  if (temps >= P_duration + QRS_duration && temps < P_duration + QRS_duration + T_duration) {
    t_wave = gaussian_wave(temps - P_duration - QRS_duration, T_amp, T_duration);
  }

  float ecg_signal = p_wave + qrs_wave + t_wave;

  rr_interval = 60.0 / heart_rate * (1 + random(-rr_variability * 100, rr_variability * 100) / 100.0);

  if (temps >= rr_interval) {
    temps = 0.0;
  }

  return ecg_signal;
}

void initECG() {
  randomSeed(analogRead(0));
}
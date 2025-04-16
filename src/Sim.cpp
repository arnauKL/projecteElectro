#include "Sim.h"
#include "config.h"
#include <cmath>
#include <Arduino.h>

// Funció per generar una ona gaussiana per a les onades P, QRS i T
float Sim::gaussian_wave(float t, float amplitude, float duration) {
  return amplitude * exp(-0.5 * pow(t / (duration / 2.0), 2));
}

// Funció per simular el complex QRS amb una part negativa (Q i S)
float Sim::qrs_wave_with_negatives(float t, float amplitude, float duration) {
  float positive_peak = gaussian_wave(t, amplitude, duration / 2.0);
  float negative_peak = gaussian_wave(t - duration / 2.0, -amplitude / 2.0, duration / 2.0);
  return positive_peak + negative_peak;
}

// Funció per generar senyal d'ECG
const float Sim::generarSenyalECG() {
  float p_wave = 0.0;
  float qrs_wave = 0.0;
  float t_wave = 0.0;

  if (tempsECG < _P_duration) {
    p_wave = gaussian_wave(tempsECG, _P_amp, _P_duration);
  }

  if (tempsECG >= _P_duration && tempsECG < _P_duration + _QRS_duration) {
    qrs_wave = qrs_wave_with_negatives(tempsECG - _P_duration, _QRS_amp, _QRS_duration);
  }

  if (tempsECG >= _P_duration + _QRS_duration && tempsECG < _P_duration + _QRS_duration + _T_duration) {
    t_wave = gaussian_wave(tempsECG - _P_duration - _QRS_duration, _T_amp, _T_duration);
  }

  rr_interval = 60.0 / _heart_rate * (1 + random(-rr_variability * 100, rr_variability * 100) / 100.0);

  tempsECG += dt;
  if (tempsECG >= rr_interval) { tempsECG = 0.0; }
  return p_wave + qrs_wave + t_wave;
}

// Funció per generar senyal de Resp
const float Sim::generarSenyalRES() {
  tempsRES += dt;

  if (tempsRES > 4.0) {   // 4.0 és el temps que tarda en fer una ona completa
    tempsRES = 0.0;
    //debug("temps RES resetejat: ");
    //debugln(tempsRES);
  }

  return resp_offset + resp_ampl * sin(2 * PI * resp_freq * tempsRES);
}

// Ctor
Sim::Sim() {
  randomSeed(analogRead(0));  // Funció per iniciar el generador de nombres aleatoris
  fs = 250.0;
  tempsECG = 0.0;
  tempsRES = 0.0;
  dt = 1.0 / fs;
  rr_interval = 60.0 / _heart_rate;
  rr_variability = 0.05;
}
#ifndef FFThandler_H_
#define FFThandler_H_

#include "config.h"
#include "arduinoFFT.h"

typedef struct{

    float vReal[MAX_BUFFER_RR]; // Part real, on van les dades a transformar, funciona de in i out
    float vImg[MAX_BUFFER_RR]; // Part imaginària
    int nEL; // Nombre d'elements

} FFTbuffer; // Per gestionar tot el tema de la fft

FFTbuffer crearFFTbuffer();
// Inicialitza el buffer

void setArrays(float* dades, FFTbuffer* buff);
// Omple els vectors 

void calcularFFT(FFTbuffer* buff);
// Calcula la fft i opté les magnituds

#endif //FFThandler_H_
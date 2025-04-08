#include "FFThandler.h"

FFTbuffer crearFFTbuffer(){
// Inicialirza el buffer
    FFTbuffer buff;
    buff.nEL = 0;
    return buff;
}

void setArrays(float* dades, FFTbuffer* buff){
// Pre: dades omplert
// Post: buff->vReal omplert i buff->vImg omplert amb 0
    for(int i=0; i<MAX_BUFFER_RR; i++){
        buff->vReal[i] = dades[i];
        buff->vImg[i] = 0;
        buff->nEL ++;
    }
}

void calcularFFT(FFTbuffer* buff){
// Pre: vectors dins del buffer omplerts
// Post: vReal conté les magnituds de les freqüencies

    arduinoFFT FFT = arduinoFFT();
    
    FFT.Windowing(buff->vReal, MAX_BUFFER_RR, FFT_WIN_TYP_HAMMING, FFT_FORWARD); // finestra
    FFT.Compute(buff->vReal, buff->vReal, MAX_BUFFER_RR, FFT_FORWARD); // càlcul
    FFT.ComplexToMagnitude(buff->vReal, buff->vImg, MAX_BUFFER_RR); // extreu les magnituds

}





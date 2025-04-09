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

    float samplingFrequency = 1/(TEMPS_INTERPOLACIONS/1000); // freq. de mostreig, la que faig servir per les interpolacions

    ArduinoFFT<float> FFT = ArduinoFFT<float>(buff->vReal, buff->vImg, MAX_BUFFER_RR, samplingFrequency); // li passo les dades reals, imaginaries, el nombre de samples i la freq. de mostreig
    
    FFT.windowing(FFTWindow::Hamming, FFTDirection::Forward); // finestra i direcció
    FFT.compute(FFTDirection::Forward); // càlcul
    FFT.complexToMagnitude(); // extreu les magnituds

}





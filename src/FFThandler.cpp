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
        buff->nEL++;
    }
}

void calcularFFT(FFTbuffer* buff){
// Pre: vectors dins del buffer omplerts
// Post: vReal conté les magnituds de les freqüencies

    float samples = MAX_BUFFER_RR;

    ArduinoFFT<float> FFT = ArduinoFFT<float>(buff->vReal, buff->vImg, samples, (float)SAMPLE_FREQ); // li passo les dades reals, imaginaries, el nombre de samples i la freq. de mostreig
    
    FFT.windowing(FFTWindow::Hamming, FFTDirection::Forward); // finestra i direcció
    FFT.compute(FFTDirection::Forward); // càlcul
    FFT.complexToMagnitude(); // extreu les magnituds

    for (int i = 0; i < buff->nEL; i++) {
        debug(buff->vReal[i]); 
        debug(", ");
    }
    debugln(";");
}

void computeStress(FFTbuffer* buff, float& sns, float& snp, float& stress){
//Pre: FFT calculada
//Post: es calcula la potència del SNS, SNP i es calcula el nivell d'estrés

    float bin = 0;

    for(int i = 0; i < MAX_BUFFER_RR/2; i++){
        
        bin = i * (SAMPLE_FREQ/MAX_BUFFER_RR); // bin de freq. en el que ens trobem

        if(bin > 0.04 && bin < SN_LIMIT) sns += buff->vReal[i]; // LF
        else if(bin > SN_LIMIT && bin < 0.4) snp += buff->vReal[i]; // HF
    }
    
    stress = sns/snp;

}
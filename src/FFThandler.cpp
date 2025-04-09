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
        buff->vReal[i] = dades[i]/1000; // 1000 per passar a segons
        buff->vImg[i] = 0;
        buff->nEL++;
    }


}

void calcularFFT(FFTbuffer* buff){
// Pre: vectors dins del buffer omplerts
// Post: vReal conté les magnituds de les freqüencies

    ArduinoFFT<float> FFT = ArduinoFFT<float>(buff->vReal, buff->vImg, MAX_BUFFER_RR, SAMPLE_FREQ); // li passo les dades reals, imaginaries, el nombre de samples i la freq. de mostreig
    
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
    sns = 0;
    snp = 0;
    stress = 0;

    int i = 0; // comptador
    float bin = i * (SAMPLE_FREQ/MAX_BUFFER_RR); // bin de freq. en el que ens trobem
    while(bin < 0.4){ // 0.4 Hz és la freq. màxima que ens interessa

        if(bin > 0.04 && bin < SN_LIMIT) sns += buff->vReal[i]; // LF
        else if(bin > SN_LIMIT) snp += buff->vReal[i]; // HF

        i++;
        bin = i * (SAMPLE_FREQ/MAX_BUFFER_RR);
    }
    
    stress = sns/snp;

}





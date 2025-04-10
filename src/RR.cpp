#include "RR.h"
#include<Arduino.h>

BufRR crearBufRR() {
// Retorna un BufRR inicialitzat correctament
    BufRR buf;
    buf.nEl = 0;
    return buf;
}

int afegirRR(BufRR* buf, float dada) {
// Pre: float dada i adreça de BufRR
// Post: Afegeix dada si hi ha espai. Retorna 1 si s'ha afegit, 0 altrament
    if (buf->nEl < MAX_BUFFER_RR) {
        buf->vec[buf->nEl] = dada;
        buf->nEl++;
        return 1;
    }
    // altrament ignoram les dades (TODO: no s'hauria de fer, haurem d veure què fer)
    return 0;
}

int detectarPicR(float mostraNova, float mostraAnt, float mostraSeg, float llindar) {
// Pre: "buffer" cutre; Post: retorna cert si hi ha un pic
    return (mostraAnt < mostraNova &&
            mostraNova > mostraSeg &&
            mostraNova > llindar);
}

void interpolar(BufRR* interRR, BufRR* peakRR, BufRR* interTimeRR, BufRR* timeRR){
// Pre: quatre adreces de BufRR, ha passat un cert temps 
// Post: Omple el vector InterRR interpolant dades de peakRR
    
    // Les primeres interpolacions són el primer pic i el seu temps corresponent
    afegirRR(interRR, peakRR->vec[0]/1000);
    afegirRR(interTimeRR, timeRR->vec[0]);

    int i = 0; // Un comptador
    
    // Variables que necessitem per aplicar la fòrmula d'interpolació lineal
    float y0 = peakRR->vec[i]/1000;
    float y1 = peakRR->vec[i+1]/1000;
    float x0 = timeRR->vec[i];
    float x1 = timeRR->vec[i+1];
    float x = (x0 + TEMPS_INTERPOLACIONS);
    float y = 0;
    
    int a = 0;

    debugln(x);
    debugln(x0);
    debugln(TEMPS_INTERPOLACIONS/1000);
    debugln(x1);

    while(i < peakRR->nEl-1 && interRR->nEl < MAX_BUFFER_RR){ // Per tots els punts que hem recollit
        
        a ++;

        while(x < x1 && interRR->nEl < MAX_BUFFER_RR){ // Fem les interpolacions que calguin entre els dos punts
            
            afegirRR(interTimeRR, x);
            y = y0 + (y1-y0)/(x1-x0) * (x-x0); // Apliquem la fòrmula de les interpolacions
            afegirRR(interRR, y);
            x += TEMPS_INTERPOLACIONS;
        
        }

        i++;

        y0 = y1;
        y1 = peakRR->vec[i+1]/1000;
        x0 = x1;
        x1 = timeRR->vec[i+1];

    }

    debugln(a);
    debugln(interRR->nEl);

    debugln("INTERPOLACIONS");
    for (int i = 0; i < interRR->nEl; i++) {
        debug(interRR->vec[i]);
        debug(", ");
    }
    debugln(";");
}

float* getVec(BufRR* buffer){
// Pre: buffer amb un vector ompler
// Post: retorna l'adreça del vector
    return buffer->vec;
}
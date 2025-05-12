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
    return 0;
}

// Funció deprecada
bool detectarPicR(const float & mostraNova, const float & mostraAnt, const float & mostraSeg, const float & llindar) {
// Pre: buffer de 3 mostres; Post: retorna cert si hi ha un pic que supera el llindar estàtic
    return (mostraNova > llindar &&
            mostraAnt < mostraNova &&
            mostraNova > mostraSeg);
}

bool detectarPicRdinamic(const float & mostraNova, const float & mostraAnt, const float & mostraSeg, float & llindarDinamic) {
// detecta pics a partir de 3 mostres i un llindar. Retorna 1 si s'ha detectat, 0 altrament; ajusta el llindar dinàmic
    if (mostraNova > llindarDinamic && mostraNova > mostraAnt && mostraNova > mostraSeg) {
        // actualitzar llindar dinàmic
        llindarDinamic = THRESHOLD_BIAS * mostraNova;
        return true;
    }
    llindarDinamic *= 0.999;   // Una mica d decay (no sé si cal, segons el soroll que tingui el senyal final)
    return false;
}

void interpolar(BufRR* interRR, BufRR* peakRR, BufRR* interTimeRR, BufRR* timeRR){
// Pre: quatre adreces de BufRR, ha passat un cert temps 
// Post: Omple el vector InterRR interpolant dades de peakRR
    
    // Les primeres interpolacions són el primer pic i el seu temps corresponent
    afegirRR(interRR, peakRR->vec[0]/1000);
    afegirRR(interTimeRR, timeRR->vec[0]);

    int i = 0;
    
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
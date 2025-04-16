#ifndef RR_H_
#define RR_H_

#include "config.h"

typedef struct {
    float vec[MAX_BUFFER_RR];   // Buffer
    int nEl;                    // Nombre d'elements
} BufRR;
// Struct per guardar dades d'intervalRR

BufRR crearBufRR();
// Inicialitza el buffer

int afegirRR(BufRR* buf, float dada);
// afegeix dada al buffer només si hi queda espai. Retorna 0 altrament

bool detectarPicR(const float & mostraNova, const float & mostraAnt, const float & mostraSeg, const float & llindar);
// detecta pics a partir de 3 mostres i un llindar. Retorna 1 si s'ha detectat, 0 altrament.

bool detectarPicRdinamic(const float & mostraNova, const float & mostraAnt, const float & mostraSeg, float & llindarDinamic);
// detecta pics a partir de 3 mostres i un llindar dinàmic (ajusta el llindar després). Retorna 1 si s'ha detectat, 0 altrament.

void interpolar(BufRR* interRR, BufRR* peakRR, BufRR* interTimeRR, BufRR* timeRR);
// interpola dades dels pics RR per tenir mostres equiespaiades

float* getVec(BufRR* buffer);
//Retorna l'adreça del vector de l'struct 

#endif // RR_H_
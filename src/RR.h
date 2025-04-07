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

int detectarPicR(float mostraNova, float mostraAnt, float mostraSeg, float llindar);
// detecta pics a partir de 3 mostres i un llindar. Retorna 1 si s'ha detectat, 0 altrament.


#endif // RR_H_
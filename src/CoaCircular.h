#ifndef COA_CIRC_H
#define COA_CIRC_H

#include "config.h"

typedef struct {
    float buffer[MAX_BUFFER_ECG];
    int first;
    int last;   // primer espai lliure
    int capacitat;
} CoaCircular;

void iniciarCoa(CoaCircular* coa);
void afegirValor(CoaCircular* coa, float valor);


#endif
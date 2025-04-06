#include "CoaCircular.h"


void iniciarCoa(CoaCircular* coa) {
    coa->first = 0;
    coa->last = 0;
    coa->capacitat = MAX_BUFFER_ECG;    // No m'agrada, s'hauria de canviar
}

void afegirValor(CoaCircular* coa, float valor) {
// Pre: coa i valor
// Post: afegeix l'element al final de la coa

    // afegim el valor
    coa->buffer[coa->last] = valor;

    // Incrementem
    coa->last++;

    // Si ens passem, fem la volta
    if (coa->last > coa->capacitat) { coa->last = 0; }
}

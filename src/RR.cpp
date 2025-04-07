#include "RR.h"

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
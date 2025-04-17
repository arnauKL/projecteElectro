#ifndef SPIhandler_H_
#define SPIhandler_H_

#include "config.h"
#include <Arduino.h>
#include <SPI.h>

void ads1292r_writeCommand(uint8_t cmd); // Per escriure una comanda a l'ADS
void reinicialitza_ads1292r(); // reinicialitza l'ADS en cas de no connectar-se
uint8_t ads1292r_readRegister(uint8_t regAddress); // Llegeix un registre de l'ADS
void writeRegister(uint8_t reg, uint8_t value); // Escriu una comanda a l'ADS a l'adreça reg
void SPIsetup(); // Fem el setup que necessita el nostre ADS
bool SPIcomprovarID(); // Fa tres intents per connectar-se a un ID vàlid


// FALTA: LLEGIR DADES DE LES QUE TRAIEM DE LA RESP I ECG I POSAR-HO AL MAIN

#endif //SPIhandler_H_

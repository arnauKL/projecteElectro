#ifndef SPIhandler_H_
#define SPIhandler_H_

#include "config.h"
#include <Arduino.h>
#include <SPI.h>

// Estructura per guardar les dades parsejades de l'ADS
typedef struct {
    uint32_t bitsStatus;
    int32_t ecgSample;
    int32_t resSample;
} ADS1292R_Data;

void ads1292r_writeCommand(uint8_t cmd); // Per escriure una comanda a l'ADS

void reinicialitza_ads1292r(); // reinicialitza l'ADS en cas de no connectar-se

uint8_t ads1292r_readRegister(uint8_t regAddress); // Llegeix un registre de l'ADS

void writeRegister(uint8_t reg, uint8_t value); // Escriu una comanda a l'ADS a l'adreça reg

void SPIsetup(); // Fem el setup que necessita el nostre ADS

bool SPIcomprovarID(); // Fa tres intents per connectar-se a un ID vàlid


// FALTA: LLEGIR DADES DE LES QUE TRAIEM DE LA RESP I ECG I POSAR-HO AL MAIN

void readADS1292RData(uint8_t *buffer);

ADS1292R_Data parseADS1292RData(uint8_t *buffer);


/** RECORDAR
 * - DRDY goes low when new data are available. (datasheet)
 * 
 * - Farem servir el mode RDATAC (continuu) perquè és més ràpid
 * 
 * - CS must remain low for the entire duration of the serial communication.
 * After the serial communication is finished, always wait four or more tCLK
 * cycles before taking CS high.
 * 
 * - SPI sempre envia paquets de 8 bits (1 byte) de cop (pwpt Pràctica 2)
 * 
 * - Velocitat de SCK mínima = 72 * Sample_freq. Si sample_freq = 250 --> SCK = 72 * 250 = 18kHz (approx, mirar pag 41 dsheet)
 * 
 * - For the ADS1292R, the number of data outputs is (24 status bits + 24 bits × 2 channels) = 72 bits. The format of
 * the 24 status bits is: (1100 + LOFF_STAT[4:0] + GPIO[1:0] + 13 '0's). The data format for each channel data is
 * twos complement, MSB first.  (datasheet pg 42)
 * 
 * - C pot llegir nombres en complement a dos, no cal convertir-los
 * 
 * - com els nombres que arriben només tenen 24 bits, cal emplenar els
 * altres bits (fins a 32) amb 1 xq C ho pugui processar correctament
*/


#endif //SPIhandler_H_

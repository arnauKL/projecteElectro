#include "SPIhandler.h"

void ads1292r_writeCommand(uint8_t cmd) { // Per escriure una comanda a l'ADS
    
  digitalWrite(ADS1292_CS_PIN, LOW);
  delayMicroseconds(20);
  SPI.transfer(cmd);
  delayMicroseconds(20);
  digitalWrite(ADS1292_CS_PIN, HIGH);
  delay(20);
}

void reinicialitza_ads1292r() { // Reinicia l'ADS
  
  digitalWrite(ADS1292_PWDN_PIN, LOW);
  delay(20);
  digitalWrite(ADS1292_PWDN_PIN, HIGH);
  delay(1000);
  ads1292r_writeCommand(CMD_RESET); // Fa el reset
  delay(10);
  ads1292r_writeCommand(CMD_SDATAC); // Sortim del mode de lectura contínua
  delay(10);
  digitalWrite(ADS1292_START_PIN, HIGH);
  
}

uint8_t ads1292r_readRegister(uint8_t regAddress) {  

  uint8_t regValue;
  digitalWrite(ADS1292_CS_PIN, LOW);
  delayMicroseconds(20);
  SPI.transfer(CMD_READ_REG | regAddress); // 0x20 | addreça (OPCODE 1)
  SPI.transfer(0x00); // llegeix 1 registre (recordar que s'ha de posar n-1 = 0) (OPCODE 2)
  regValue = SPI.transfer(0x00); // dummy byte to receive data
  digitalWrite(ADS1292_CS_PIN, HIGH);
  delay(1);
  return regValue;
  
}

void writeRegister(uint8_t reg, uint8_t value) { // Per escriure un registre a l'ADS
  
  digitalWrite(ADS1292_CS_PIN, LOW);
  delayMicroseconds(20);
  SPI.transfer(0x40 | reg); // 0x40→OPCODE per escriure
  SPI.transfer(0x00);
  SPI.transfer(value);
  delayMicroseconds(20);
  digitalWrite(ADS1292_CS_PIN, HIGH);
  delay(20);
  // Verificació: llegir el registre després d'escriure
  uint8_t readBack = ads1292r_readRegister(reg);
  Serial.print("Registre 0x");
  Serial.print(reg, HEX);
  Serial.print(" escrit amb 0x");
  Serial.print(value, HEX);
  Serial.print("\t| Llegit: 0x");
  Serial.println(readBack, HEX);
  if (readBack != value) {
    Serial.println(" ERROR: El valor escrit no coincideix amb el llegit!");
  }
  
}


void SPIsetup() {

  pinMode(ADS1292_PWDN_PIN, OUTPUT); // PAS 2
  pinMode(ADS1292_CS_PIN, OUTPUT);
  pinMode(ADS1292_START_PIN, OUTPUT);
  pinMode(ADS1292_DRDY_PIN, INPUT);
  delay(10);
  digitalWrite(ADS1292_CS_PIN, HIGH);
  delay(10);

  //digitalWrite(ADS1292_SCK, HIGH); // Això ho he afegit jo
  //delay(1);

  digitalWrite(ADS1292_START_PIN, LOW);
  digitalWrite(ADS1292_PWDN_PIN, LOW);
  delay(20); // mínim 2 ms
  digitalWrite(ADS1292_PWDN_PIN, HIGH);
  delay(1500);
  digitalWrite(ADS1292_START_PIN, HIGH);
  delay(20); // estabilitzar GPIOs


  // (Ho hem girat)
  Serial.println("Iniciant SPI...");

  SPI.begin(ADS1292_SCK, ADS1292_MISO, ADS1292_MOSI, ADS1292_CS_PIN);
  SPI.beginTransaction(SPISettings(FREQ_SCLK_SPI, MSBFIRST, SPI_MODE1)); // mode 1 (CPOL=0, CPHA=1)
  ads1292r_writeCommand(CMD_RESET); // Reset del xip, PAS 3
  delay(20);

  // Atura el mode de lectura contínua (important per evitar problemes), PAS 4
  ads1292r_writeCommand(CMD_SDATAC);
  delay(20);
  ads1292r_writeCommand(CMD_STOP);
  delay(20);

  // Configurem els registres 
  digitalWrite(ADS1292_CS_PIN, LOW);
  delayMicroseconds(200);
  writeRegister(0x01, 0x01); // 00000001 Config1: 250 SPS
  writeRegister(0x02, 0xA0); // 10100000 Config2: Enable RLD, test signal off
  writeRegister(0x03, 0x10); // Lead-Off control
  writeRegister(0x04, 0x50); // 01010000 congif del canal 1, RESP, PGAIN1 = 8     // Aquests dos potser els podem posar com a configuracions
  writeRegister(0x05, 0x40); // 0100000 config del canal 2, ECG, PGAIN2 = 4       //                          "
  writeRegister(0x07, 0x00); // LOFF_SENS, igual que a la presentació...
  writeRegister(0x09, 0xC2); // 11000010 Li indiquem que volem fer els circuits de modulació i desmodulació, que volem que la fase de l'ona moduladora sigui 0 i que fem servir clock intern
  writeRegister(0x0A, 0x07); // 00000011 Li indiquem que no fagi calibració, que la freq. de la RESP sigui 64 kHz i que la VREF sigui la interna
  writeRegister(0x06, 0xA0); // 10100000 Li indiquem que freq. de chop sigiui freq. mod / 2 (32 kHz) i que volem fer servir RLD
  writeRegister(0x0B, 0x00); // GPIO's, igual que a la presentació...
  delayMicroseconds(200);
  digitalWrite(ADS1292_CS_PIN, HIGH);

  ads1292r_writeCommand(CMD_RDATAC);    // Tornar a posar el mode x llegir data continuament
  ads1292r_writeCommand(CMD_START);
}

bool SPIcomprovarID() {
// Pre: SPI configurat
//Post: retorna cert si hem pogut establir la connexió, altrament fals

  // Aquesta primera part mira si és un dispositiu del qual ens podem connectar
  uint8_t id;
  int intents = 0;

  do {
    id = ads1292r_readRegister(0x00);
    Serial.print("Intent ");
    Serial.print(intents + 1);
    Serial.print(": Registre ID (0x00)= 0x");
    Serial.println(id, HEX);
    if (id == 0x73) {
      break;
    }
    intents++;
    reinicialitza_ads1292r();

    SPIsetup(); // Hem fet un reset, per tant, hem de tornar a configurar-ho tot

    delay(100);
  } while (intents < 3);

  if (id != 0x73) {
    Serial.println(" Error: No s'ha detectat ID vàlid després de 3 intents.");
    return false;
  } 
  else {
    Serial.println(" ID vàlid detectat.");
    return true;
  }
  delay(2000);
}


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

void readADS1292RData(uint8_t *buffer) {
// Funció x llegir els 9 bytes de cop
  digitalWrite(ADS1292_CS_PIN, LOW);        // Baixar CS
  SPI.transfer(buffer, BYTES_REBUTS);       // Legir els 9 bytes al buffer
  // 'after the serial communication is finished, always wait four or more tCLK
  // cycles before taking CS high.'
  delayMicroseconds(DELAY_CS_HIGH_MICROS);  // 4*tCLK
  digitalWrite(ADS1292_CS_PIN, HIGH);       // Tornar a pujar CS
}

ADS1292R_Data parseADS1292RData(uint8_t *buffer) {
// Funció x parsejar el buffer al struct
  ADS1292R_Data dadesParsed;

  // Unions x fer la conversió immediata sense còpia de memòria :D
  union { // Per parsejar els bytes de status
    uint8_t bytes[4]; // 4 bytes
    uint32_t valor;   // 32 bits -> unsigned aquí perquè el status no té sentit com a número, només és un conjunt de bits; així C no fa líos
  } statusConverter;

  union { // Per parsejar els bytes dels canals
    uint8_t bytes[4]; // 4 bytes = 24 bits x canal
    int32_t valor;    // 32 bits -> 4 bytes (el 4t bit l'emplenam fent extensió d signe)
  } canalConverter;


  // bits d'estat (bytes 0-2)
  statusConverter.bytes[3] = 0x00; // Posem un valor conegut als bits més grans (little-Endiannes)
  statusConverter.bytes[2] = buffer[0];
  statusConverter.bytes[1] = buffer[1];
  statusConverter.bytes[0] = buffer[2];
  dadesParsed.bitsStatus = statusConverter.valor & 0xFFFFFF; // No caldria fer la màscara però queda més net (i sóc un paranoic, no vull corrupció d memòria)

  // canal 1: ECG (bytes 3-5)
  canalConverter.bytes[2] = buffer[3];
  canalConverter.bytes[1] = buffer[4];
  canalConverter.bytes[0] = buffer[5];
  dadesParsed.ecgSample = (canalConverter.valor << 8) >> 8; // Extensió d signe. 
  // Una locura, com s compilador veu q és "signed", sap q està en C2 i fa sa propagació del signe si fem un shift amunt i avall

  // canal 2: RES (bytes 6-8)
  canalConverter.bytes[2] = buffer[6];
  canalConverter.bytes[1] = buffer[7];
  canalConverter.bytes[0] = buffer[8];
  dadesParsed.resSample = (canalConverter.valor << 8) >> 8; // Extensió d signe

  return dadesParsed;
}
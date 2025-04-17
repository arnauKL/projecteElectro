#include "SPIhandler.h"

void ads1292r_writeCommand(uint8_t cmd) { // Per escriure una comanda a l'ADS
    
        digitalWrite(ADS1292_CS_PIN, LOW);
        delayMicroseconds(2);
        SPI.transfer(cmd);
        delayMicroseconds(2);
        digitalWrite(ADS1292_CS_PIN, HIGH);
        delay(1);
  
}

void reinicialitza_ads1292r() { // Reinicia l'ADS
  
    digitalWrite(ADS1292_PWDN_PIN, LOW);
    delay(2);
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
    delayMicroseconds(2);
    SPI.transfer(CMD_READ_REG | regAddress); // 0x20 | addreça (OPCODE 1)
    SPI.transfer(0x00); // llegeix 1 registre (recordar que s'ha de posar n-1 = 0) (OPCODE 2)
    regValue = SPI.transfer(0x00); // dummy byte to receive data
    digitalWrite(ADS1292_CS_PIN, HIGH);
    delay(1);
    return regValue;
  
}

void writeRegister(uint8_t reg, uint8_t value) { // Per escriure un registre a l'ADS
  
    digitalWrite(ADS1292_CS_PIN, LOW);
    delayMicroseconds(2);
    SPI.transfer(0x40 | reg); // 0x40→OPCODE per escriure
    SPI.transfer(0x00);
    SPI.transfer(value);
    delayMicroseconds(2);
    digitalWrite(ADS1292_CS_PIN, HIGH);
    delay(1);
    // Verificació: llegir el registre després d'escriure
    uint8_t readBack = ads1292r_readRegister(reg);
    Serial.print("Registre 0x");
    Serial.print(reg, HEX);
    Serial.print(" escrit amb 0x");
    Serial.print(value, HEX);
    Serial.print(" | Llegit: 0x");
    Serial.println(readBack, HEX);
    if (readBack != value) {
      Serial.println(" ERROR: El valor escrit no coincideix amb el llegit!");
    }
  
}

void SPIsetup() {

    SPI.begin(); // PAS 1
    
    pinMode(ADS1292_PWDN_PIN, OUTPUT); // PAS 2
    pinMode(ADS1292_CS_PIN, OUTPUT);
    pinMode(ADS1292_START_PIN, OUTPUT);
    pinMode(ADS1292_DRDY_PIN, INPUT);
    delay(1);
    digitalWrite(ADS1292_CS_PIN, HIGH);
    delay(1);

    digitalWrite(ADS1292_SCK, HIGH); // Això ho he afegit jo
    delay(1);
  
    digitalWrite(ADS1292_START_PIN, LOW);
    digitalWrite(ADS1292_PWDN_PIN, LOW);
    delay(5); // mínim 2 ms
    digitalWrite(ADS1292_PWDN_PIN, HIGH);
    delay(1500);
    digitalWrite(ADS1292_START_PIN, HIGH);
    delay(10); // estabilitzar GPIOs

    Serial.begin(115200);
    Serial.println("Iniciant SPI...");

    SPI.end();

    SPI.begin(ADS1292_SCK, ADS1292_MISO, ADS1292_MOSI, ADS1292_CS_PIN);
    SPI.beginTransaction(SPISettings(1000000, MSBFIRST, SPI_MODE1)); // mode 1 (CPOL=0, CPHA=1)
    ads1292r_writeCommand(CMD_RESET); // Reset del xip, PAS 3
    delay(10);

    // Atura el mode de lectura contínua (important per evitar problemes), PAS 4
    ads1292r_writeCommand(CMD_SDATAC);
    delay(10);

    // Configurem els registres 
    digitalWrite(ADS1292_CS_PIN, LOW);
    delayMicroseconds(2);
    writeRegister(0x01, 0x01); // 00000001 Config1: 250 SPS
    writeRegister(0x02, 0xA0); // 10100000 Config2: Enable RLD, test signal off
    writeRegister(0x03, 0x10); // Lead-Off control
    writeRegister(0x04, 0x50); // 01010000 congif del canal 1, RESP, PGAIN1 = 8
    writeRegister(0x05, 0x40); // 0100000 config del canal 2, ECG, PGAIN2 = 4
    writeRegister(0x07, 0x00); // LOFF_SENS, igual que a la presentació...
    writeRegister(0x09, 0xC2); // 11000010 Li indiquem que volem fer els circuits de modulació i desmodulació, que volem que la fase de l'ona moduladora sigui 0 i que fem servir clock intern
    writeRegister(0x0A, 0x07); // 00000011 Li indiquem que no fagi calibració, que la freq. de la RESP sigui 64 kHz i que la VREF sigui la interna
    writeRegister(0x06, 0xA0); // 10100000 Li indiquem que freq. de chop sigiui freq. mod / 2 (32 kHz) i que volem fer servir RLD
    writeRegister(0x0B, 0x00); // GPIO's, igual que a la presentació...
    delayMicroseconds(2);
    digitalWrite(ADS1292_CS_PIN, HIGH);

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
    } 
    while (intents < 3);
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
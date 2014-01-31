#include "includes.h"

unsigned long mode, config;

unsigned long AD7193_ReadReg(unsigned char registerNumber, unsigned char bytesNumber) {
   // ClearUART();
    unsigned char registerWord[4] = {0, 0, 0, 0};
    unsigned long buffer = 0x0;
    unsigned char i = 0;
    registerWord[0] = AD7193_READ | AD7193_REG_SELECT(registerNumber);
    registerWord[1] = 0x0;
    registerWord[2] = 0x0;
    registerWord[3] = 0x0;
    SPI_Read(registerWord, bytesNumber + 1);
    for (i = 1; i < bytesNumber + 1; i++) {
        buffer = (buffer << 8) + registerWord[i];
    }
    return buffer;
}


void AD7193_Reset(void) {
    unsigned char registerWord[6] = {0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF};
    SPI_Write(registerWord, 6);
}

void AD7193_WriteReg(unsigned char registerAddress, unsigned long registerValue, unsigned char bytesNumber) {
    unsigned char writeCommand[4] = {0, 0, 0, 0};
    unsigned char* dataPointer = (unsigned char*) &registerValue;
    unsigned char bytesNr = bytesNumber;
    writeCommand[0] = AD7193_WRITE | AD7193_REG_SELECT(registerAddress);
    while (bytesNr > 0) {
        writeCommand[bytesNr] = *dataPointer;
        bytesNr--;
        dataPointer++;
    }
    SPI_Write(writeCommand, bytesNumber + 1);
}


void AD7193_SetChannel(unsigned char channel) {
    unsigned long new_config = (AD7193_GetConfig()&(~CONFIG_CHANNEL_MASK)) | CONFIG_CHANNEL_SEL(channel)|CONFIG_GAIN_64;
    AD7193_WriteReg(AD7193_CONFIG_REG, new_config, 3);
}

unsigned char AD7193_SetGain(unsigned char gain) {
    unsigned long new_config = AD7193_GetConfig()&(~CONFIG_GAIN_MASK);
    switch (gain) {
        case '1': new_config |= CONFIG_GAIN_1;
            break;
        case '2': new_config |= CONFIG_GAIN_8;
            break;
        case '3': new_config |= CONFIG_GAIN_16;
            break;
        case '4': new_config |= CONFIG_GAIN_32;
            break;
        case '5': new_config |= CONFIG_GAIN_64;
            break;
        case '6': new_config |= CONFIG_GAIN_128;
            break;
        default: new_config |= CONFIG_GAIN_64;
            break;
    }
    AD7193_WriteReg(AD7193_CONFIG_REG, new_config, 3);
    return 0;
}
unsigned char AD7193_GetChannel(void){
    return 0;
}
unsigned char AD7193_SetRate(unsigned int rate, unsigned char channel){

    AD7193_SetChannel(channel);
    if(rate >1024)
        return 1;
    return 0;
}
unsigned long AD7193_GetMode(void){
    unsigned long buff = AD7193_ReadReg(AD7193_MODE_REG,3);
    return buff;
}
unsigned long AD7193_GetConfig(void){
    unsigned long buff = AD7193_ReadReg(AD7193_CONFIG_REG,3);
    return buff;
}

unsigned long AD7193_Init(void){
    AD7193_Reset();
    long mode = MODE_CONT_CONV | MODE_NO_AVG | MODE_INT_CLK1 | MODE_FILTER_RATE(1) | MODE_DAT_STA;
    long config = CONFIG_REFSEL_1 | CONFIG_CHANNEL_SEL(0) | CONFIG_CHANNEL_SEL(2) | CONFIG_REFDET | CONFIG_BUF | CONFIG_GAIN_32;
    AD7193_WriteReg(AD7193_MODE_REG,mode,3);
    AD7193_WriteReg(AD7193_CONFIG_REG,config,3);
    long ID = AD7193_ReadReg(AD7193_ID_REG,1);
    if((ID&0x0F) == 0x02)
        return 1;
    else
        return 0;
}



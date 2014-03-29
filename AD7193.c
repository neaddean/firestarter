#include "includes.h"


unsigned long AD7193_ReadReg(unsigned char registerNumber, unsigned char bytesNumber) {
    unsigned char registerWord[4] = {0, 0, 0, 0};//The buffer to push out data
    unsigned long buffer = 0x0;//The long to store the response.
    unsigned char i = 0;
    //Set the first output byte to the command to read from the selected register.
    registerWord[0] = AD7193_READ | AD7193_REG_SELECT(registerNumber);
    //Push the read command and store the result back into registerWord
    SPI_Read(registerWord, bytesNumber + 1);
    //Pack the result into buffer
    for (i = 1; i < bytesNumber + 1; i++) {
        buffer = (buffer << 8) + registerWord[i];
    }
    return buffer;
}

/*
 * AD7193_WriteReg
 * unsigned char registerAddress - the register number
 * unsigned long registerValue - the data to write to the register
 * unsigned char bytesNumber - how many bytes of data are we writing
 *
 * This function uses SPI_Write as definied in Communication.c
 *
 * The function recieves a long for the data to write. It then casts the long into
 * a character pointer to allow easy iteration through the independent bytes, as
 * each byte must be written separately. As it iterates through the bytes, it adds
 * them to the writeCommand buffer and then uses SPI_Write to transmit the data.
 *
 * As this is a WRITE command the function does not return anything.
 *
 * The largest register on the AD7193 is 24 bits so the write buffer is 32 bits wide:
 * 8 bits for the command, 24 bits for the data.
 */
void AD7193_WriteReg(unsigned char registerAddress, unsigned long registerValue, unsigned char bytesNumber) {
    unsigned char writeCommand[4] = {0, 0, 0, 0}; //The write buffer.
    unsigned char* dataPointer = (unsigned char*) &registerValue; //The pointer to the first byte of registerValue.
    unsigned char bytesNr = bytesNumber;//A counter.
    //The write command to the specified register
    writeCommand[0] = AD7193_WRITE | AD7193_REG_SELECT(registerAddress);
    while (bytesNr > 0) {//Iterate through the bytes, adding them to the buffer.
        writeCommand[bytesNr] = *dataPointer;
        bytesNr--;
        dataPointer++;
    }
    //Transmit the data.
    SPI_Write(writeCommand, bytesNumber + 1);
}
/*
 * AD7193_Reset
 *
 * This function uses SPI_Write as defined in Communication.c
 *
 * The AD7193 is designed to reset itself if it sees 40(?) repeated ones on the
 * SPI bus. This function writes out a total of 6 bytes all equals to 0xFF to
 * reset the AD7193 upon power on.
 */
void AD7193_Reset(void) {
    unsigned char registerWord[6] = {0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF};
    SPI_Write(registerWord, 6);
    AD7193_Init();
}

/*
 * AD7193_Init
 *
 * This function writes the initial configuration data to the AD7193, setting both
 * the MODE and CONFIG registers.
 *
 * The mode register is set with the following options selected:
 * Continuous Conversion - on
 * No Averaging
 * Internal Clk with no output
 * A filter word of 5
 * and
 * Append the Status Register to all data output.
 *
 * The configuration register is set with the following options selected:
 * Use REFIN1 (+5V/0V)
 * Turn on Channels 0, 1 and 2. (Fuel, pressurant and rocket mass)
 * Enable reference detection
 * Use the Buffer
 * Gain of 32x
 *
 */
void AD7193_Init(void){
    long mode = MODE_CONT_CONV | MODE_AVG_8 | MODE_INT_CLK1 | MODE_FILTER_RATE(1) | MODE_DAT_STA;
    long config = CONFIG_REFSEL_1 |  CONFIG_CHANNEL_SEL(0) | CONFIG_CHANNEL_SEL(2) | CONFIG_CHANNEL_SEL(3) | CONFIG_REFDET | CONFIG_BUF | CONFIG_GAIN_32;
    AD7193_WriteReg(AD7193_MODE_REG,mode,3);
    AD7193_WriteReg(AD7193_CONFIG_REG,config,3);
}

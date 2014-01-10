#include "includes.h"

long data_cmds_received = 0;
long data_cmds_processed = 0;
long data_cmds_transmitted=0;

char MathABS(long one, long two){
    long temp = one-two;
    if(temp<0){
        temp^=0xFFFFFFFF;//Flip bits and add one.
        temp++;             //Faster than multiplication (most likely...)
    }
    return temp;
}
char opened=0;
void ProcessUART(void) {
    //RESET THE AD7193
//    int i=0;
//    for(i=0; i<UART_Buff_Size; i++)
//    {
//        putcUART1(UART_Buffer[i]);
//    }
    if (UART_Buffer[0] == 'S') {
        if (UART_Buffer[1] == 'N') {
            putsUART1((UINT *)(UINT*) "FS-01\n");
        } else if (UART_Buffer[1] == 'T') {
            long temp = AD7193_ReadReg(AD7193_STATUS_REG, 1);
            putcUART1((char)(temp&0xFF));
            while(U1STAbits.TRMT);
        }
    } else if (UART_Buffer[0] == 'M') {
        if(UART_Buffer[1]=='W'){
            long mode = MODE_CONT_CONV | MODE_NO_AVG | MODE_INT_CLK1 | MODE_FILTER_RATE(5);
            AD7193_WriteReg(AD7193_MODE_REG, mode, 3);
            putsUART1((UINT *)"A");
        } else {
            long result = AD7193_ReadReg(AD7193_MODE_REG, 3);
            char data[3] = {(char)((result>>16)&0xFF),(char)((result>>8)&0xFF),(char)(result&0xFF)};
            putcUART1(data[0]);
            while(U1STAbits.TRMT);
            putcUART1(data[1]);
            while(U1STAbits.TRMT);
            putcUART1(data[2]);
            while(U1STAbits.TRMT);
        }
    } else if (UART_Buffer[0] == 'R') {
        unsigned char reset = 0xFF;
        SPI_Write(&reset, 1);
        SPI_Write(&reset, 1);
        SPI_Write(&reset, 1);
        SPI_Write(&reset, 1);
        putsUART1((UINT *)"A");
    } else if (UART_Buffer[0] == 'l') {
//        unsigned long result = AD7193_ReadReg(AD7193_DATA_REG, 4)-(DC_OFFSET << 8);
//        char data2[4] = {(char) ((result & 0xFF000000) >> 24),(char) ((result & 0x00FF0000) >> 16),(char) ((result & 0x0000FF00) >> 8), (char) result & 0x000000FF};
//            putcUART1(data2[0]);
//            while(U1STAbits.TRMT);
//            putcUART1(data2[1]);
//            while(U1STAbits.TRMT);
//            putcUART1(data2[2]);
//            while(U1STAbits.TRMT);
//            putcUART1(data2[3]);
//            while(U1STAbits.TRMT);
        char result[4];
        AD7193_toBuffer4(result);
            putcUART1(result[0]);
            while(U1STAbits.TRMT);
            putcUART1(result[1]);
            while(U1STAbits.TRMT);
            putcUART1(result[2]);
            while(U1STAbits.TRMT);
            putcUART1(result[3]);
            while(U1STAbits.TRMT);
    } else if(UART_Buffer[0] =='G'){
        long config = CONFIG_REFSEL_1 | CONFIG_CHANNEL_SEL(1) | CONFIG_REFDET | CONFIG_BUF;
        switch(UART_Buffer[1]){
            case '1': config|=CONFIG_GAIN_1; break;
            case '2': config|=CONFIG_GAIN_8; break;
            case '3': config|=CONFIG_GAIN_16;break;
            case '4': config|=CONFIG_GAIN_32; break;
            case '5': config|=CONFIG_GAIN_64; break;
            case '6': config|=CONFIG_GAIN_128; break;
            default: config|=CONFIG_GAIN_64; break;
        }
        AD7193_WriteReg(AD7193_CONFIG_REG, config, 3);
        putsUART1((UINT *)"A");
    } else if (UART_Buffer[0] == 'o')
    {
        if(initFiles())
            putsUART1((UINT*)"filesystem initilzed successfuly\n");
        else
            putsUART1((UINT*)"failed to initialize filesystem\n");
    }
    else if (UART_Buffer[0] == 'c')
    {
        closeFiles();
    }
    else if (UART_Buffer[0] == 'd')
    {
        if(!MDD_MediaDetect())
            putsUART1((UINT*)"Card detected\n");
        else
            putsUART1((UINT*)"Card not detected\n");
    }else if (UART_Buffer[0] == 'f')
    {
        if (UART_Buffer[1] == '0')
            fireEmatch(0);
        else if (UART_Buffer[1] == '1')
            fireEmatch(1);
        else if (UART_Buffer[1] == '2')
            fireEmatch(2);
    }
    else if (UART_Buffer[0] == 's')
    {
        if (UART_Buffer[1] == '1')
            senseEmatch(1);
        else if (UART_Buffer[1] == '2')
            senseEmatch(2);
    }
     else if (UART_Buffer[0] == 'P')
    {
        if (UART_Buffer[1] == 'D') //pressurant
        {
            putcUART1('P');
            TX_Register(NTank, 3);
        }
        else if (UART_Buffer[1] == 'F') //oxidixer
        {
            putcUART1('O');
            TX_Register(NitroTank, 3);
        }
    }
     else if (UART_Buffer[0] == 'q')
     {
         if (UART_Buffer[1] == 's')
             startRecording();
         else if (UART_Buffer[1] == 'd')
         {
             pyroValve();
         }
     }

}
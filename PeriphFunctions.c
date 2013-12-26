#include "includes.h"

volatile int  UART_Buff_Size = 0;
volatile char UART_Process_Flag = 0;
volatile char UART_Buffer[10];
volatile long adc_res=0;
volatile long adc_status=0;
volatile long prev_adc_res=0;
volatile char TX_Buff[3];
volatile long ZERO_OFFSET=0;
volatile char CTS = 0;

volatile FSFILE * tankfile;

volatile char NTank[3];
volatile char NitroTank[3];

volatile char record_data_flag = 0;


/************************************************
 * UART FUNCTIONS
 ***********************************************/
/**
 * Configures the UART properly.
 * Enables the UART
 * Only uses RX/TX - no flow control.
 * Puts BRG into high speed mode.
 * Sets input to RP8 - 3 up on right.
 * Sets TX to RP 13 - 5 down on right.
 *
 */
void UARTinit(void) {
    AD1PCFG=0xFFFF;
    int config1 = UART_EN | UART_UEN_00 | UART_BRGH_SIXTEEN;
    int config2 = UART_TX_ENABLE;
    OpenUART1(config1, config2, UxBRG_value);
    RPINR18 = 1;
    RPOR0 = 0x0003;
    ConfigIntUART1(UART_RX_INT_EN | UART_RX_INT_PR4);
}

void IOinit(void)
{
    TRISA = 0x0003;
    TRISB = 0x014A;
    LATA = 0x0000;
    LATB = 0x0000;
}

void _ISR _U1RXInterrupt(void) {
    U1RX_Clear_Intr_Status_Bit;
    
    if (U1STAbits.OERR) {
        U1STAbits.OERR = 0;
    }
    UART_Buffer[UART_Buff_Size++] = U1RXREG;
    if (UART_Buffer[UART_Buff_Size - 1] == '\n') {
        UART_Process_Flag = 1;
    }
}

void ClearUART(void){
    int i=0;
    for(i=0; i<10; i++){
        UART_Buffer[i]=0;
    }
    UART_Buff_Size=0;
    UART_Process_Flag=0;
}

void TX_Register(char*buffer,char bytes){
    int index = 0;
    while(bytes-->0){
        putcUART1(buffer[index]);
        while(U1STAbits.TRMT);
        index++;
    }
    putcUART1('\n');
    while(U1STAbits.TRMT);
}

/************************************************
 * SPI FUNCTIONS
 ***********************************************/
/**
 * Configures the SPI bus to work with the AD7193
 * SCK and SDO on
 * 8-Bit transmission
 * SMP - OFF
 * CKE - OFF
 * Polarity - Active Low
 * Primary Prescaler - 4:1      - works with 16Mhz Fcy
 * Secondary Prescaler - 4:1    - so SPI clk is 1Mhz
 *
 * SCK is on RP3 - 7 down on left
 * SDO is on RP2 - 6 down on left
 * SDI is on RP7 - 2 up on right.
 */

void SPIinit(void) {
    int config1 = ENABLE_SCK_PIN | ENABLE_SDO_PIN | SPI_MODE8_ON | SPI_SMP_OFF;
    config1 |= SPI_CKE_OFF | CLK_POL_ACTIVE_LOW | MASTER_ENABLE_ON | SEC_PRESCAL_4_1 | PRI_PRESCAL_4_1;
    int config2 = SPI1CON2;
    int config3 = SPI_ENABLE;
    OpenSPI1(config1, config2, config3);

    RPOR1 = 0x0007;
    RPOR2 = 0x0008;
    RPINR20 = 0x03;

    RPOR3 = 0x0B0A;
    RPINR22 = 0x05;
}


void SPI_Read(unsigned char * data, unsigned char bytesNumber) {
    unsigned char byte = 0;
    unsigned char writeBuffer[4] = {0, 0, 0, 0};
    for (byte = 0; byte < bytesNumber; byte++) {
        writeBuffer[byte] = data[byte];
    }
    SPI1STATbits.SPIROV = 0;
    for (byte = 0; byte < bytesNumber; byte++) {
        SPI1BUF = data[byte];
        while (!SPI1_Rx_Buf_Full);
        data[byte] = SPI1BUF;
    }
}


void SPI_Write(unsigned char* data, unsigned char bytesNumber) {
    unsigned char byte = 0;
    unsigned char tempByte = 0;
    SPI1STATbits.SPIROV = 0;
    for (byte = 0; byte < bytesNumber; byte++) {
        SPI1BUF = data[byte];
        while (!SPI1_Rx_Buf_Full);
        tempByte = SPI1BUF;
    }
}

void AD7193_toBuffer4(char * data)
{
     unsigned long result = AD7193_ReadReg(AD7193_DATA_REG, 4)-(DC_OFFSET << 8);
     int i = 0;
     char data2[4] = {(char) ((result & 0xFF000000) >> 24),(char) ((result & 0x00FF0000) >> 16),(char) ((result & 0x0000FF00) >> 8), (char) result & 0x000000FF};
     for (i = 0; i < 4; i++)
     {
         data[i] = data2[i];
     }
}

BOOL initFiles(void)
{
   // putsUART1("attempting to open fs\n");
    if (!FSInit())
    {
       putsUART1("FAILED");
       return FALSE;
    }

    tankfile = FSfopen("t.txt", "w");

    if (tankfile == NULL)
    {
        return FALSE;
    }

  //  putsUART1("FS initialized corectly!\n");
    return TRUE;
}

void closeFiles(void)
{
    if (FSfclose(tankfile))
        putsUART1((UINT*) "\nERROR: Close tank1.txt failed.\n");
    putsUART1("File closed!\n");
}


void fireEmatch(int eMatch)
{
	 switch (eMatch)
	 {
            case 0:
		 LATBbits.LATB13 = 1;
		 break;

            case 1:
		 LATBbits.LATB12 = 1;
		 break;

            case 2:
                 LATBbits.LATB9 = 1;
                 break;
	}

	 __delay32(1000);

	 LATBbits.LATB13 = 0;
	 LATBbits.LATB12 = 0;
         LATBbits.LATB9  = 0;
}

void senseEmatch(int eMatch)
{
    switch (eMatch)
    {
        case 1:
            LATAbits.LATA2 = 1;
            break;
        case 2:
            LATAbits.LATA3 = 1;
            break;
    }

    __delay32(1000);

    if (eMatch == 1)
    {
        if (PORTBbits.RB8 == 1)
            putsUART1("1\tATTACHED\n");
        else
            putsUART1("1D\n");
    }
    else if (eMatch == 2)
    {
        if (PORTBbits.RB8 == 1)
            putsUART1("2\tATTACHED\n");
        else
            putsUART1("2D\n");
    }

    PORTAbits.RA2 = 0;
    PORTAbits.RA3 = 0;
}

void recordData(void)
{
    while(PORTBbits.RB3);
    unsigned long result = AD7193_ReadReg(AD7193_DATA_REG, 4)-(DC_OFFSET << 8);
    char data2[4] = {(char) ((result & 0xFF000000) >> 24),(char) ((result & 0x00FF0000) >> 16),(char) ((result & 0x0000FF00) >> 8), (char) result & 0x000000FF};
    FSfwrite(data2, 1, 4, tankfile);
}

void startRecording()
{
    record_data_flag = 1;
    //ConfigIntTimer1(T1_ON);           // 45 seconds
    OpenTimer23(T23_ON | T23_PS_1_256, 0x002AEA54); //to stop the conversions!
//    OpenTimer23(T23_ON | T23_PS_1_64, 0x002AEA54);
    ConfigIntTimer23(T23_INT_ON | T23_INT_PRIOR_6);
}

void copyBuffer(char* src, char* dest, int bytes)
{
    int i;
    for (i = 0; i < bytes; i++)
        dest[i] = src[i];
}

void __attribute__ ((interrupt,no_auto_psv)) _T23Interrupt (void)
{
   T23_Clear_Intr_Status_Bit;
   CloseTimer23();
   CloseTimer1();
   closeFiles();
}

void pyroValveCountdown()
{
    OpenTimer45(T45_ON | T45_PS_1_256, 0x0004C4B4);
    ConfigIntTimer4(T45_INT_ON | T45_INT_PRIOR_6);
}

void __attribute__ ((interrupt,no_auto_psv)) _T45Interrupt (void)
{
   T45_Clear_Intr_Status_Bit;
   CloseTimer45();
   fireEmatch(2);
}
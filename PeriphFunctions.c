#include "includes.h"

volatile int UART_Buff_Size = 0;
volatile char UART_Process_Flag = 0;
volatile char UART_Buffer[10];
volatile long adc_res = 0;
volatile long adc_status = 0;
volatile long prev_adc_res = 0;
volatile char TX_Buff[3];
volatile long ZERO_OFFSET = 0;
volatile char CTS = 0;

volatile long Pressurant;
volatile long Oxidizer;
volatile char fifo_data[4];
volatile char senddata[4];

volatile char armed = 0;
volatile char fired = 0;

volatile char close_file_flag = 0;

volatile int T3POST = 0;

volatile char record_data_flag = 0;

volatile FSFILE * tankfile;

#define fifo_size 256
volatile char fifo_buf[256];
volatile int head = 0;
volatile int tail = 0;

volatile char regulating = 0;
volatile int backangle = -60;
volatile char turnOffServo = 0;
volatile int TOSPC = 0;

volatile char servo_status;

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
    ClearUART();
    AD1PCFG = 0xFFFF;
    int config1 = UART_EN | UART_UEN_00 | UART_BRGH_SIXTEEN;
    int config2 = UART_TX_ENABLE;
    OpenUART1(config1, config2, UxBRG_value);
    RPINR18 = 1;
    RPOR0 = 0x0003;
    ConfigIntUART1(UART_RX_INT_EN | UART_RX_INT_PR1);
}

void IOinit(void) {
    TRISA = 0x0003;
    TRISB = 0x004A;
    LATA = 0x0000;
    LATB = 0x0000;
}

void __attribute__((interrupt, no_auto_psv)) _U1RXInterrupt(void) {
    U1RX_Clear_Intr_Status_Bit;

    if (U1STAbits.OERR) {
        U1STAbits.OERR = 0;
    }
    UART_Buffer[UART_Buff_Size++] = U1RXREG;
    if (UART_Buffer[UART_Buff_Size - 1] == '\n') {
        UART_Process_Flag = 1;
    }
}

void ClearUART(void) {
    int i = 0;
    for (i = 0; i < 10; i++) {
        UART_Buffer[i] = 0;
    }
    UART_Buff_Size = 0;
    UART_Process_Flag = 0;
}

void TX_Register(char*buffer, char bytes) {
    int index = 0;
    while (bytes-- > 0) {
        putcUART1(buffer[index]);
        while (U1STAbits.TRMT);
        index++;
    }
    putcUART1('\n');
    while (U1STAbits.TRMT);
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

void AD7193_toBuffer4(char * data) {
    unsigned long result = AD7193_ReadReg(AD7193_DATA_REG, 4); //-(DC_OFFSET << 8);
    int i = 0;
    char data2[4] = {(char) ((result & 0xFF000000) >> 24), (char) ((result & 0x00FF0000) >> 16), (char) ((result & 0x0000FF00) >> 8), (char) result & 0x000000FF};
    for (i = 0; i < 4; i++) {
        data[i] = data2[i];
    }
}

BOOL initFiles(void) {
    // putsUART1("attempting to open fs\n");
    if (!FSInit()) {
#ifdef DEBUG
        putsUART1((UINT*) "FAILED TO FSINIT\n");
#endif
        return FALSE;
    }
#ifdef DEBUG
    putsUART1((UINT*) "FS initialized corectly!\n");
#endif
    return TRUE;
}

void closeFiles(void) {
#ifdef DEBUG
    if (FSfclose(tankfile))
        putsUART1((UINT*) "\nERROR: Close tank1.txt failed.\n");
    putsUART1((UINT*) "File closed!\n");
#else
    FSfclose(tankfile);
#endif
}

void fireEmatch(int eMatch) {
    switch (eMatch) {
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

    unsigned int count;
    for (count = 0; count < 60000; count++);

    LATBbits.LATB13 = 0;
    LATBbits.LATB12 = 0;
    LATBbits.LATB9 = 0;
}

void senseEmatch(int eMatch) {
    switch (eMatch) {
        case 1:
            LATAbits.LATA2 = 1;
            break;
        case 2:
            LATAbits.LATA3 = 1;
            break;
    }

    UINT count;
    for (count = 0; count < 7500; count++);

    if (eMatch == 1) {
        if (PORTBbits.RB8 == 1)
            putsUART1((UINT*) "1\tATTACHED\n");
        else
            putsUART1((UINT*) "1D\n");
    } else if (eMatch == 2) {
        if (PORTBbits.RB8 == 1)
            putsUART1((UINT*) "2\tATTACHED\n");
        else
            putsUART1((UINT*) "2D\n");
    }
    //
    ////    LATAbits.LATA2 = 0;
    ////    LATAbits.LATA3 = 0;
}

void startRecording() {

    tankfile = FSfopen("t.txt", "w");

    if (tankfile == NULL) {
#ifdef DEBUG
        putsUART1((UINT*) "FAILED TO OPEN FILE\n");
#endif
        return FALSE;
    }

    record_data_flag = 1;
    //    OpenTimer45(T45_ON | T45_PS_1_256, 0x002AEA54); // 45s
    ////    OpenTimer23(T23_ON | T23_PS_1_256, 0x00393870); // 60s
    ////    OpenTimer23(T23_ON | T23_PS_1_64, 0x002625A0); //  ~10s
    ////    OpenTimer23(T23_ON, 0x0001FFFF);
    //    ConfigIntTimer45(T45_INT_ON | T45_INT_PRIOR_6);

    T4CON = 0xC038;
    PR4 = 0xEA54;
    PR5 = 0x002A;
    IEC1bits.T5IE = 1;
    IPC7bits.T5IP = 6;
}

void copyBuffer(char* src, char* dest, int bytes) {
    int i;
    for (i = 0; i < bytes; i++)
        dest[i] = src[i];
}

void __attribute__((interrupt, auto_psv)) _T1Interrupt(void) {
    T1_Clear_Intr_Status_Bit;
    //    while (PORTBbits.RB3);
    unsigned long result = AD7193_ReadReg(AD7193_DATA_REG, 4);
    char data2[4] = {(char) ((result & 0xFF000000) >> 24), (char) ((result & 0x00FF0000) >> 16), (char) ((result & 0x0000FF00) >> 8), (char) result & 0x000000FF};
    fifo_write(data2, 4);
    if ((result & 0x07) == 0x03)
    {
        Oxidizer = result >> 8;
    }
    else if ((result & 0x07) == 0x02)
    {
        Pressurant = result >> 8;
    }
    if (regulating) {
        long adc_error = PSI_SETPOINT - (Oxidizer - 0x800000);
        double PSI_error = adc_error * .000198616;
        long theta = PSI_error * 3;
        if (theta < 0) {
            theta = -30;
        } else if (theta > 57) {
            theta = 57;
        }
        setAngle(-57 + theta);
    } else if (backfill) {
        long adc_error = PSI_SETPOINT - (Oxidizer - 0x800000);
        double PSI_error = adc_error * .000198616;
        long theta = PSI_error * 1.5;
        if (theta < 0) {
            theta = -30;
        } else if (theta > 57) {
            theta = 57;
        }
        setAngle(-57 + theta);
    } else {
        setAngle(-90);
    }
}

void __attribute__((interrupt, no_auto_psv)) _T45Interrupt(void) {
    T45_Clear_Intr_Status_Bit;
    CloseTimer45();
    close_file_flag = 1;
    record_data_flag = 0;
    regulating = 0;
    OC3RS = 0;
    OC3CONbits.OCM = 0;
    SERVO_OFF();
}

void pyroValve() {
    T3CON = 0xC030;
    PR3 = 0xFFFF;
    //    PR5 = 0x0004;
    IEC0bits.T3IE = 1;
    IPC2bits.T3IP = 6;
    T3POST = 0;
    regulating = 1;
}

void __attribute__((interrupt, no_auto_psv)) _T3Interrupt(void) {
    T3_Clear_Intr_Status_Bit;
    if (T3POST++ == 7) {
        CloseTimer3();
        fireEmatch(1);
    }
}

//This reads nbytes bytes from the FIFO
//The number of bytes read is returned

int fifo_read(void * buf, int nbytes) {
    int i;
    char * p;
    p = buf;
    for (i = 0; i < nbytes; i++) {
        if (tail != head) { //see if any data is available
            *p++ = fifo_buf[tail]; //grab a byte from the buffer
            tail++; //increment the tail
            if (tail == fifo_size) { //check for wrap-around
                tail = 0;
            }
        } else {
            return i; //number of bytes read
        }
    }
    return nbytes;
}

//This writes up to nbytes bytes to the FIFO
//If the head runs in to the tail, not all bytes are written
//The number of bytes written is returned

int fifo_write(const void * buf, int nbytes) {
    int i;
    const char * p;
    p = buf;
    for (i = 0; i < nbytes; i++) {
        //first check to see if there is space in the buffer
        if ((head + 1 == tail) ||
                ((head + 1 == fifo_size) && (tail == 0))) {
            return i; //no more room
        } else {
            fifo_buf[head] = *p++;
            head++; //increment the head
            if (head == fifo_size) { //check for wrap-around
                head = 0;
            }
        }
    }
    return nbytes;
}

void processData() {
    if (fifo_read(fifo_data, 4) == 4) {
        if (record_data_flag) {
            FSfwrite(fifo_data, 1, 4, tankfile);
        }
    }
}

void readCard() {
    //    DisableIntT1;
    //
    //    tankfile = FSfopen("t.txt", "r");
    //
    //    if (tankfile == NULL)
    //        putsUART1((UINT*)"ERROR: COULD NOT OPEN FILE FOR READ!!!....\n");
    //
    //    while (1)
    //    {
    //        if (FSfread(senddata, 1, 4, tankfile) != 4)
    //            break;
    //        TX_Register(senddata, 4);
    //    }
    ////    char tempdata[1000];
    ////    int bytesread = FSfread(tempdata, 4, 250, tankfile);
    ////    int i;
    ////    for (i = 0; i < 1000; i++)
    ////        putcUART1(tempdata[i]);
    //
    //    putsUART1((UINT*)"DONE\n");
    //
    //    FSfclose(tankfile);
}

void PWMinit() {
    T2CONbits.TON = 1;
    T2CONbits.TCKPS = 1;
    T2CONbits.T32 = 0;
    T2CONbits.TCS = 0;

    PR3 = SERVO_PERIOD;
    OC3R = HT7990_N90;
    OC3CONbits.OCTSEL = 0;
    OC3CONbits.OCM = 6;
    RPOR7 = 0x1414;
}

void setAngle(long angle) {
    OC3RS = HT7990_0 + (int) (12.2 * angle); //Set to max 10 degree opening for fill;
}

void SERVO_ON() {
    LATBbits.LATB8 = 1;
    __delay32(900000);
    LATBbits.LATB8 = 0;
    servo_status = 1;
}

void SERVO_OFF(void) {
    LATBbits.LATB13 = 1;
    __delay32(900000);
    LATBbits.LATB13 = 0;
    servo_status = 0;
}
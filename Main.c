/* 
 * File:   Main.c
 * Author: Drew
 *
 * Created on November 19, 2013, 9:24 PM
 */


// CONFIG2
#pragma config POSCMOD = NONE           // Primary Oscillator Select (Primary oscillator disabled)
#pragma config I2C1SEL = PRI            // I2C1 Pin Location Select (Use default SCL1/SDA1 pins)
#pragma config IOL1WAY = OFF            // IOLOCK Protection (IOLOCK may be changed via unlocking seq)
#pragma config OSCIOFNC = OFF           // Primary Oscillator Output Function (OSC2/CLKO/RC15 functions as CLKO (FOSC/2))
#pragma config FCKSM = CSDCMD           // Clock Switching and Monitor (Clock switching and Fail-Safe Clock Monitor are disabled)
#pragma config FNOSC = FRCPLL           // Oscillator Select (Fast RC Oscillator with PLL module (FRCPLL))
#pragma config SOSCSEL = SOSC           // Sec Oscillator Select (Default Secondary Oscillator (SOSC))
#pragma config WUTSEL = LEG             // Wake-up timer Select (Legacy Wake-up Timer)
#pragma config IESO = ON                // Internal External Switch Over Mode (IESO mode (Two-Speed Start-up) enabled)

// CONFIG1
#pragma config WDTPS = PS32768          // Watchdog Timer Postscaler (1:32,768)
#pragma config FWPSA = PR128            // WDT Prescaler (Prescaler ratio of 1:128)
#pragma config WINDIS = OFF             // Watchdog Timer Window (Windowed Watchdog Timer enabled; FWDTEN must be 1)
#pragma config FWDTEN = OFF             // Watchdog Timer Enable (Watchdog Timer is disabled)
#pragma config ICS = PGx2               // Comm Channel Select (Emulator EMUC2/EMUD2 pins are shared with PGC2/PGD2)
#pragma config GWRP = OFF               // General Code Segment Write Protect (Writes to program memory are allowed)
#pragma config GCP = OFF                // General Code Segment Code Protect (Code protection is disabled)
#pragma config JTAGEN = OFF             // JTAG Port Enable (JTAG port is disabled)

#include "includes.h"
//volatile char UART_Buffer[10];


char primed;

//volatile FSFILE * tankfile;
    char fifo_data[4];
int main() {
    OSCinit();
    UARTinit();
    SPIinit();
    IOinit();
    AD7193_Init();
    initFiles();
    FIFOinit();
    OpenTimer1(T1_ON, 0x9C40); // for conversions
    ConfigIntTimer1(T1_INT_ON);           // 45 seconds
    //mV/V for 3kpsi PT is 6 at a gain of 128.

    while(1){

        if(UART_Process_Flag){
            ProcessUART();
            ClearUART();
        }
//        if (adc_fifo->head != adc_fifo->tail)
//        {
//            if (fifo_read(adc_fifo, fifo_data, 4) == 4)
//                if (record_data_flag)
//                    putsUART1((UINT*)"write success");
//            if (record_data_flag)
//            {
//                FSfwrite(fifo_data, 1, 4, tankfile);
//            }
//            else
//            {
//                if (fifo_data[3] == 0x01)
//                    copyBuffer(fifo_data, NTank, 3);
//                else if (fifo_data[3] == 0x02)
//                    copyBuffer(fifo_data, NitroTank, 3);
//            }
//        }
//        if (IFS0bits.T1IF)
//        {
//            T1_Clear_Intr_Status_Bit;
//            if (record_data_flag)
//            {
//                recordData();
//            }
//            else
//            {
//                while(PORTBbits.RB3);
//                unsigned long result = AD7193_ReadReg(AD7193_DATA_REG, 4);// - (DC_OFFSET << 8) - 0x80000000;
//                char data2[4] = {(char) ((result & 0xFF000000) >> 24),(char) ((result & 0x00FF0000) >> 16),(char) ((result & 0x0000FF00) >> 8), (char) result & 0x000000FF};
//                if (data2[3] == 0x01)
//                    copyBuffer(data2, NTank, 3);
//                else if (data2[3] == 0x02)
//                    copyBuffer(data2, NitroTank, 3);
//            }
//        }
    }
    return 0;
}


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

int main() {
    OSCinit();
    SERVO_OFF();
    UARTinit();
    SPIinit();
    IOinit();
    AD7193_Reset();
    initFiles();
    PWMinit();
    //    OpenTimer1(T1_ON, 0x9C40); // for conversions
    OpenTimer1(T1_ON, 0x3E80);
    //    OpenTimer1(T1_ON, 0x29B0);
    ConfigIntTimer1(T1_INT_ON | T1_INT_PRIOR_3);
    //    putsUART1((UINT*)"Started...\n");
    while (1) {
        if (UART_Process_Flag) {
            ProcessUART();
            ClearUART();
        }
        processData();
        if (close_file_flag) {
            closeFiles();
            close_file_flag = 0;
        }
        //        if (armed) {
        //            if (!PORTAbits.RA0) {
        //                startRecording();
        //                SERVO_ON();
        //                armed = 0;
        //                fired = 1;
        //                regulating = 1;
        //                putsUART1((UINT*) "PA\n");
        //                fireEmatch(2);
        //                pyroValve();
        //            }
        //        }
        if (!fired) {
            if (!PORTAbits.RA0) {
                SERVO_ON();
                regulating = 1;
                startRecording();
                fired = 1;
                fireEmatch(1);
            }
        }
    }
    return 0;
}


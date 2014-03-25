/**
 * Initializes Oscillator.
 * 32 MHz Fosc
 * 16 MHz Fcy
 *
 */

#include "includes.h"

void OSCinit(void){
     OSCCONbits.COSC = 0b001; //Enable FRC Phased Locked Loop to get necessary USB clk.
    OSCCONbits.NOSC = 0b001;
    CLKDIVbits.RCDIV = 0;
    CLKDIVbits.DOZE = 0;
}
//
//void PWMinit(void){
//    RPOR7 = 0x0012; //Set RP10 as OC1 output - 7 up on right
//    T2CONbits.TCKPS = 0x1; //Sets Timer 1 prescaler to 8:1. Gives 2MHz source
//    T2CONbits.TCS = 0; //Ensures timer is running off Fosc/2 (16MHz)
//    T2CONbits.TON = 1; //Enable timer.
//    PR1 = 0x1019;
//    OC1R = SERVO2_N90;
//    OC1RS = SERVO2_N90; //Period of 20ms at 250kHz clk SET TO 0x9C40
//    OC1R = SERVO2_N90; //On-time is 1500us which is neutral position
//    OC1CON2bits.SYNCSEL = 0x1F; //Sets sync source as self.
////      OC1CONbits.OCTSEL = 0x0; //Timer 1 is the clk source (2MHz)
//      OC1CONbits.OCM = 0x6; //Turn on PWM if not already on.
//    OC1CON2bits.OCTRIG = 0;
//}
/******************************************************************************
 *
 *                Microchip Memory Disk Drive File System
 *
 ******************************************************************************
 * FileName:        HardwareProfile.h
 * Dependencies:    None
 * Processor:       PIC18/PIC24/dsPIC30/dsPIC33/PIC32
 * Compiler:        C18/C30/C32
 * Company:         Microchip Technology, Inc.
 *
 * Software License Agreement
 *
 * The software supplied herewith by Microchip Technology Incorporated
 * (the �Company�) for its PICmicro� Microcontroller is intended and
 * supplied to you, the Company�s customer, for use solely and
 * exclusively on Microchip PICmicro Microcontroller products. The
 * software is owned by the Company and/or its supplier, and is
 * protected under applicable copyright laws. All rights are reserved.
 * Any use in violation of the foregoing restrictions may subject the
 * user to criminal sanctions under applicable laws, as well as to
 * civil liability for the breach of the terms and conditions of this
 * license.
 *
 * THIS SOFTWARE IS PROVIDED IN AN �AS IS� CONDITION. NO WARRANTIES,
 * WHETHER EXPRESS, IMPLIED OR STATUTORY, INCLUDING, BUT NOT LIMITED
 * TO, IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A
 * PARTICULAR PURPOSE APPLY TO THIS SOFTWARE. THE COMPANY SHALL NOT,
 * IN ANY CIRCUMSTANCES, BE LIABLE FOR SPECIAL, INCIDENTAL OR
 * CONSEQUENTIAL DAMAGES, FOR ANY REASON WHATSOEVER.
 *
*****************************************************************************/
/********************************************************************
 Change History:
  Rev            Description
  ----           -----------------------
  1.3.4          Added support for PIC18F8722,PIC24FJ256DA210,
                 dsPIC33E & PIC24E microcontrollers.
                 Added macro "SPI_INTERRUPT_FLAG_ASM" for PIC18F
                 microcontrollers to support SD card SPI driver.
********************************************************************/

#ifndef _HARDWAREPROFILE_H_
#define _HARDWAREPROFILE_H_

// Define your clock speed here

    #define GetSystemClock()        32000000
    #define GetPeripheralClock()    GetSystemClock()
    #define GetInstructionClock()   (GetSystemClock() / 2)

    // Clock values
    #define MILLISECONDS_PER_TICK       10                      // Definition for use with a tick timer
    #define TIMER_PRESCALER             TIMER_PRESCALER_8       // Definition for use with a tick timer
    #define TIMER_PERIOD                20000                   // Definition for use with a tick timer

// Description: Macro used to enable the SD-SPI physical layer (SD-SPI.c and .h)
#define USE_SD_INTERFACE_WITH_SPI


/*********************************************************************/
/******************* Pin and Register Definitions ********************/
/*********************************************************************/

	        // Description: SD-SPI Chip Select Output bit
	        #define SD_CS				LATAbits.LATA4
	        // Description: SD-SPI Chip Select TRIS bit
	        #define SD_CS_TRIS			TRISAbits.TRISA4
	        
	        // Description: SD-SPI Card Detect Input bit
	        #define SD_CD               PORTAbits.RA1
	        // Description: SD-SPI Card Detect TRIS bit
	        #define SD_CD_TRIS          TRISAbits.TRISA1
	        
	        // Description: SD-SPI Write Protect Check Input bit
	        #define SD_WE               0 //PORTBbits.RB1
	        // Description: SD-SPI Write Protect Check TRIS bit
	        #define SD_WE_TRIS          TRISAbits.TRISA0
	        
	        // Registers for the SPI module you want to use

	        // Description: The main SPI control register
	        #define SPICON1             SPI2CON1
	        // Description: The SPI status register
	        #define SPISTAT             SPI2STAT
	        // Description: The SPI Buffer
	        #define SPIBUF              SPI2BUF
	        // Description: The receive buffer full bit in the SPI status register
	        #define SPISTAT_RBF         SPI2STATbits.SPIRBF
	        // Description: The bitwise define for the SPI control register (i.e. _____bits)
	        #define SPICON1bits         SPI2CON1bits
	        // Description: The bitwise define for the SPI status register (i.e. _____bits)
	        #define SPISTATbits         SPI2STATbits
	        // Description: The enable bit for the SPI module
	        #define SPIENABLE           SPI2STATbits.SPIEN

	        // Tris pins for SCK/SDI/SDO lines

	        // Description: The TRIS bit for the SCK pin
	        #define SPICLOCK            TRISBbits.TRISB7
	        // Description: The TRIS bit for the SDI pin
	        #define SPIIN               TRISBbits.TRISB5
	        // Description: The TRIS bit for the SDO pin
	        #define SPIOUT              TRISBbits.TRISB6

#include "uart2.h"

#endif

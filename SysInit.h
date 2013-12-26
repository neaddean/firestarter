/* 
 * File:   SysInit.h
 * Author: Drew
 *
 * Created on November 19, 2013, 9:32 PM
 */

#ifndef SYSINIT_H
#define	SYSINIT_H

#define SERVO1_N90 0x4B0
#define SERVO1_N45 0x834
#define SERVO1_0 0xBB8 //0xBB8
#define SERVO1_P45 0xF3C
#define SERVO1_P90 0x12C0

//These are for the E-Reg server testing ONLY
#define SERVO2_N90 0x757
#define SERVO2_N45 0x987
#define SERVO2_0 0xBB8
#define SERVO2_P45 0xDE8
#define SERVO2_P90 0x1019

void OSCinit(void);
void PWMinit(void);

#endif	/* SYSINIT_H */


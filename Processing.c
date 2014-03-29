#include "includes.h"

long data_cmds_received = 0;
long data_cmds_processed = 0;
long data_cmds_transmitted = 0;

volatile char backfill = 0;

char MathABS(long one, long two) {
    long temp = one - two;
    if (temp < 0) {
        temp ^= 0xFFFFFFFF; //Flip bits and add one.
        temp++; //Faster than multiplication (most likely...)
    }
    return temp;
}
char opened = 0;

void ProcessUART(void) {
    //RESET THE AD7193
    //    int i=0;
    //    for(i=0; i<UART_Buff_Size; i++)
    //    {
    //        putcUART1(UART_Buffer[i]);
    //    }
    if (UART_Buffer[0] == 'S') {
        if (UART_Buffer[1] == 'N') {
            putsUART1((UINT *) (UINT*) "FS-01\n");
            AD7193_Reset();
        } else if (UART_Buffer[1] == 'T') {
            long temp = AD7193_ReadReg(AD7193_STATUS_REG, 1);
            putcUART1((char) (temp & 0xFF));
            while (U1STAbits.TRMT);
        }
    } else if (UART_Buffer[0] == 'M') {
        if (UART_Buffer[1] == 'W') {
            long mode = MODE_CONT_CONV | MODE_NO_AVG | MODE_INT_CLK1 | MODE_FILTER_RATE(5);
            AD7193_WriteReg(AD7193_MODE_REG, mode, 3);
            putsUART1((UINT *) "A");
        } else {
            long result = AD7193_ReadReg(AD7193_MODE_REG, 3);
            char data[3] = {(char) ((result >> 16)&0xFF), (char) ((result >> 8)&0xFF), (char) (result & 0xFF)};
            putcUART1(data[0]);
            while (U1STAbits.TRMT);
            putcUART1(data[1]);
            while (U1STAbits.TRMT);
            putcUART1(data[2]);
            while (U1STAbits.TRMT);
        }
    } else if (UART_Buffer[0] == 'R') {
        if (UART_Buffer[1] == "M")
        {
            Reset();
        }
        else if (UART_Buffer[1] == "A")
        {
            AD7193_Reset();
        }
    }
    else if (UART_Buffer[0] == 'o') {
        if (initFiles())
            putsUART1((UINT*) "filesystem initilzed successfuly\n");
        else
            putsUART1((UINT*) "failed to initialize filesystem\n");
    } else if (UART_Buffer[0] == 'c') {
        closeFiles();
    } else if (UART_Buffer[0] == 'd') {
        if (!MDD_MediaDetect())
            putsUART1((UINT*) "Card detected\n");
        else
            putsUART1((UINT*) "Card not detected\n");
    } else if (UART_Buffer[0] == 'f') {
        if (UART_Buffer[1] == '0')
            fireEmatch(0);
        else if (UART_Buffer[1] == '1')
            fireEmatch(1);
        else if (UART_Buffer[1] == '2')
            fireEmatch(2);
    } else if (UART_Buffer[0] == 's') {
        if (UART_Buffer[1] == '1')
            senseEmatch(1);
        else if (UART_Buffer[1] == '2')
            senseEmatch(2);
        if (UART_Buffer[1] == 'a') {
            SERVO_ON();
        } else if (UART_Buffer[1] == 'z') {
            SERVO_OFF();
        } else if (UART_Buffer[1] == 'p') {
            putcUART1('S');
            putcUART1(servo_status);
            putcUART1('\n');
        }
    } else if (UART_Buffer[0] == 'P') {
        if (UART_Buffer[1] == 'D') //pressurant
        {
            putcUART1('P');
            while (U1STAbits.TRMT);
            char dataP[3] = {(char) ((Pressurant >> 16)&0xFF), (char) ((Pressurant >> 8)&0xFF), (char) (Pressurant & 0xFF)};
            putcUART1(dataP[0]);
            while (U1STAbits.TRMT);
            putcUART1(dataP[1]);
            while (U1STAbits.TRMT);
            putcUART1(dataP[2]);
            while (U1STAbits.TRMT);
        } else if (UART_Buffer[1] == 'F') //oxidixer
        {
            putcUART1('O');
            while (U1STAbits.TRMT);
            char dataO[3] = {(char) ((Oxidizer >> 16)&0xFF), (char) ((Oxidizer >> 8)&0xFF), (char) (Oxidizer & 0xFF)};
            putcUART1(dataO[0]);
            while (U1STAbits.TRMT);
            putcUART1(dataO[1]);
            while (U1STAbits.TRMT);
            putcUART1(dataO[2]);
            while (U1STAbits.TRMT);
        }
    } else if (UART_Buffer[0] == 'q') {
        if (UART_Buffer[1] == 's')
            startRecording();
        else if (UART_Buffer[1] == 'd') {
            SERVO_ON();
            fired = 1;
//#ifndef DEBUG
            startRecording();
//#endif
            regulating = 1;
            putsUART1((UINT*) "PA\n");
            fireEmatch(2);
            pyroValve();
        } else if (UART_Buffer[1] == 'f') {
            if (PORTAbits.RA0) {
                armed = 1;
                putsUART1((UINT*) "ARMED\n");
            } else
                putsUART1((UINT*) "PROBLEM\n");
        } else if (UART_Buffer[1] == 'w') {
            armed = 0;
            putsUART1((UINT*) "DISARMED\n");
        } else if (UART_Buffer[1] == 'r') {
            record_data_flag = 1;
        } else if (UART_Buffer[1] == 't') {
            record_data_flag = 0;
            close_file_flag = 1;
        } else if (UART_Buffer[1] == 'g') {
            if (PORTAbits.RA0)
                putsUART1((UINT*) "SENSE CONNECTED\n");
            else
                putsUART1((UINT*) "SENSE DISCONNECTED\n");
        }
    } else if (UART_Buffer[0] == 'b') {
        if (UART_Buffer[1] == 'b') {
            SERVO_ON();
            backfill = 1;
            OC3CONbits.OCM = 6;
            backangle = -50;
            //            setAngle(backangle);
        } else if (UART_Buffer[1] == 'e') {
            setAngle(-90);
            backfill = 0;
            turnOffServo = 1;
            TOSPC = 0;
            __delay32(16000000);
            SERVO_OFF();
        }
    } 
}
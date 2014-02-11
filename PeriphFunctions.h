/* 
 * File:   PeriphInit.h
 * Author: Drew
 *
 * Created on November 19, 2013, 9:30 PM
 */

#ifndef PERIPHINIT_H
#define	PERIPHINIT_H

#define PSI_SETPOINT 0x47e005
//#define DC_OFFSET 0x14E6D

#define UxBRG_value 16
extern volatile char UART_Buffer[10];
extern volatile int UART_Buff_Size;
extern volatile char UART_Process_Flag;
extern volatile char regulation_active;
extern volatile char back_fill_procedure_active;
extern volatile char prime_active;
extern volatile long adc_res;
extern volatile long adc_status;
extern volatile long prev_adc_res;
extern volatile long error;
extern volatile long theta;
extern volatile char TX_Buff[3];
extern volatile long ZERO_OFFSET;
extern volatile char CTS;
extern volatile char data_request;
extern volatile char close_file_flag;

extern volatile char record_data_flag;
extern volatile char ematches_armed;

extern volatile char regulating;
extern volatile char backfill;
extern volatile int  backangle;
extern volatile char turnOffServo;
extern volatile int TOSPC;

void UARTinit(void);
void ProcessUART(void);
void ClearUART(void);
void TX_Register(char * buffer,char bytes);

void IOinit(void);
void PWMinit(void);

void fireEmatch(int);
void senseEmatch(int);

BOOL initFiles(void);
void closeFiles(void);
void readCard(void);

void SPIinit(void);
void SPI_Read(unsigned char * data, unsigned char bytesNumber);
void SPI_Write(unsigned char* data, unsigned char bytesNumber);

void AD7193_toBuffer4(char* data);

void pyroValve(void);
void startRecording(void);

void processData(void);
void copyBuffer(char* src, char* dest, int bytes);

extern volatile long Pressurant;
extern volatile long Oxidizer;

int fifo_read(void * buf, int nbytes);
int fifo_write(const void * buf, int nbytes);

void setAngle(long angle);

#define HT7990_N90 1919
#define HT7990_N45 2569
#define HT7990_0 3019
#define HT7990_45 3569
#define HT7990_90 4119

#define SERVO_PERIOD 4500


#endif	/* PERIPHINIT_H */


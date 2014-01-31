/* 
 * File:   PeriphInit.h
 * Author: Drew
 *
 * Created on November 19, 2013, 9:30 PM
 */

#ifndef PERIPHINIT_H
#define	PERIPHINIT_H

#define PSI_SETPOINT 0x7CF49 //500PSI passed through conversion formula to get target ADC Value
#define DC_OFFSET 0x14E6D

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

void UARTinit(void);
void ProcessUART(void);
void ClearUART(void);
void TX_Register(char * buffer,char bytes);

void  IOinit(void);

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

extern volatile char Pressurant[3];
extern volatile char Oxidizer[3];

int fifo_read(void * buf, int nbytes);
int fifo_write(const void * buf, int nbytes);


#endif	/* PERIPHINIT_H */


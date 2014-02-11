/*
 * File:   AD7193.h
 * Author: Drew
 *
 * Created on November 27, 2013, 10:11 PM
 */

#ifndef AD7193_H
#define	AD7193_H


//Register codes
#define AD7193_COMM_REG      0
#define AD7193_STATUS_REG    0
#define AD7193_MODE_REG      1
#define AD7193_CONFIG_REG    2
#define AD7193_DATA_REG      3
#define AD7193_ID_REG        4
#define AD7193_GPOCON_REG    5
#define AD7193_OFFSET_REG    6
#define AD7193_FULLSCALE_REG 7

//Read/Write flags for the Communication Register
#define AD7193_READ  1<<6
#define AD7193_WRITE 0<<6

//Mode Register Options
#define MODE_CONT_CONV          0b000l<<21
#define MODE_SINGLE_CONV        0b001l<<21
#define MODE_IDLE               0b010l<<21
#define MODE_POWER_DOWN         0b011l<<21
#define MODE_INT_ZERO_CAL       0b100l<<21
#define MODE_INT_FULL_CAL       0b101l<<21
#define MODE_SYS_ZERO_CAL       0b110l<<21
#define MODE_SYS_FULL_CAL       0b111l<<21
#define MODE_DAT_STA            1l<<20
#define MODE_EX_CLK_DIFF        0b00l<<18
#define MODE_EX_CLK_MCLK2       0b01l<<18
#define MODE_INT_CLK1           0b10l<<18
#define MODE_INT_CLK2           0b11l<<18
#define MODE_NO_AVG             0b000<<16
#define MODE_AVG_2              0b010<<16
#define MODE_AVG_4              0b100<<16
#define MODE_AVG_8              0b110<<16
#define MODE_SINC3              1l<<15
#define MODE_SINC4              0l<<15
#define MODE_ENPAR              1l<<13
#define MODE_CLK_DIV            1l<<12
#define MODE_SINGLE             1l<<11
#define MODE_REJ60              1l<<10
#define MODE_FILTER_RATE(x)     ((x&0b1111111111))
#define MODE_FILTER_MASK        0x3FF

//CONFIGURATION REGISTER OPTIONS
#define CONFIG_CHOP             1l<<23
#define CONFIG_REFSEL_1         0l<<20
#define CONFIG_REFSEL_2         1l<<20
#define CONFIG_PSUEDO           1l<<18
#define CONFIG_CHANNEL_SEL(x)   ((1<<x)<<8)
#define CONFIG_BURN             1l<<7
#define CONFIG_REFDET           1l<<6
#define CONFIG_BUF              1l<<4
#define CONFIG_U                1l<<3
#define CONFIG_GAIN_1           0b000
#define CONFIG_GAIN_8           0b011
#define CONFIG_GAIN_16          0b100
#define CONFIG_GAIN_32          0b101
#define CONFIG_GAIN_64          0b110
#define CONFIG_GAIN_128         0b111

#define CONFIG_GAIN_MASK        0b111
#define CONFIG_CHANNEL_MASK     0b1111111111l<<8
#define AD7193_REG_SELECT(x)    (x)<<3

//AD7193 Communication Functions
unsigned long AD7193_ReadReg(unsigned char registerNumber, unsigned char bytesNumber);
void AD7193_WriteReg(unsigned char registerAddress, unsigned long registerValue, unsigned char bytesNumber);
void AD7193_Init(void);
void AD7193_Reset(void);
#endif	/* AD7193_H */


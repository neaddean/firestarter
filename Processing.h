/* 
 * File:   Processing.h
 * Author: Drew
 *
 * Created on November 27, 2013, 10:01 PM
 */

#ifndef PROCESSING_H
#define	PROCESSING_H

#define VALVECOMMAND 'V'
#define CONFIGCOMMAND 'C'
#define SERIALCOMMAND 'S'
#define DATACOMMAND 'D'

void ProcessUART(void);
char StrEqual(char * buffer, char * cmd);
char MathABS(long one, long two);
#endif	/* PROCESSING_H */


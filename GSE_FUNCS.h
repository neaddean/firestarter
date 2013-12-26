/* 
 * File:   GSE_FUNCS.h
 * Author: Dean
 *
 * Created on December 13, 2013, 6:01 AM
 */

#ifndef GSE_FUNCS_H
#define	GSE_FUNCS_H

#include "includes.h"

    extern volatile char nitrotank[4];
    extern volatile char ntank[4];

    void getNitroTank(void);
    void getNTank(void);

    void EMatchCoundown(void);




#ifdef	__cplusplus
}
#endif

#endif	/* GSE_FUNCS_H */


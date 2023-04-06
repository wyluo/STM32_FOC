#ifndef __IQS7211E_H__
#define __IQS7211E_H__

#include "stm32f10x.h"


#define IQS7211E_ADDR               0x56<<1

void IQS7211E_Extix_Init(void);
void IQS7211E_Init(void);
void Process_IQS7211E_Events(void);

#endif

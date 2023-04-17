#ifndef __IQS7211E_H__
#define __IQS7211E_H__

#include "stm32f10x.h"
#include "i2c_core.h"

#define IQS7211E_ADDR               0x56<<1
#define STOP_TRUE 1

void IQS7211E_Extix_Init(void);
int8_t IQS7211E_Init(struct i2c_dev_device *i2c_bus);
void Process_IQS7211E_Events(void);

#endif

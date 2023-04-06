#ifndef APP_TIMER_H
#define APP_TIMER_H

#include "stm32f10x.h"

void TIM2_Init(u16 arr, u16 psc);
void SystemTickConfig(void);
uint32_t GetSysTickVal(void);


#endif

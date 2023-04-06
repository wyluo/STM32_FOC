#ifndef BSP_DELAY_H
#define BSP_DELAY_H

#include "stm32f10x.h"

void delay_init(void);
void delay_ms(u32 sysclk, u16 nms);
void delay_us(u32 sysclk, u32 nus);

#endif

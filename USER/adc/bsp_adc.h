#ifndef BSP_ADC_H
#define BSP_ADC_H

#include "stm32f10x.h"

#define    ADC_IRQHandler                ADC1_2_IRQHandler

void adc_init(void);
u16 Get_Adc(u8 ch);
u16 Get_Adc_Average(u8 ch, u8 times);

#endif

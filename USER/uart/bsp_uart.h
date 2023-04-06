#ifndef BSP_UART_H
#define BSP_UART_H

#include "stm32f10x.h"
#include "stdio.h"	

#define USART_REC_LEN  			200

void USART1_Configuration(u32 bound);

#endif

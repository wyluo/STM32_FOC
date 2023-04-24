#ifndef ATK_MW8266D
#define ATK_MW8266D

#include "stm32f10x.h"
#include "config.h"

#if MW8266D_ENABLE

#define MW8266D_UART_RX_BUF_SIZE            128

void usart3_init(u32 bound);

#endif

#endif


#ifndef BSP_UART_H
#define BSP_UART_H

#include "stm32f10x.h"
#include "config.h"
#include <stdio.h>

#if ENABLE_USART1
#define USART_REC_LEN   200

void usart_init(USART_TypeDef *pUSARTx, u32 bound);
void usart_send(USART_TypeDef *pUSARTx, u16 data);
void usart_send_arry(USART_TypeDef *pUSARTx, uint8_t *array, uint16_t len);
void usart_send_string(USART_TypeDef *pUSARTx, char *str);
void usart_send_btyes(USART_TypeDef *pUSARTx, uint16_t data);

#endif

#endif

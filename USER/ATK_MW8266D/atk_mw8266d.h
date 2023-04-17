#ifndef ATK_MW8266D
#define ATK_MW8266D

#include "stm32f10x.h"
#include "config.h"

#if ATK_MW8266D_ENABLE

/* UART收发缓冲大小 */
#define ATK_MW8266D_UART_RX_BUF_SIZE            128
#define ATK_MW8266D_UART_TX_BUF_SIZE            64

void atk_mw8266d_usart_config(u32 bound);
void atk_mw8266d_uart_printf(char *fmt, ...);
uint16_t atk_mw8266d_uart_rx_get_frame_len(void);
uint8_t *atk_mw8266d_uart_rx_get_frame(void);
void atk_mw8266d_uart_rx_restart(void);

#endif

#endif


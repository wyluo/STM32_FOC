/**
****************************************************************************************
 * @Author: hekies fortwater969@163.com
 * @Version: 
 * @Date: 2023-04-17 09:22:44
 * @LastEditors: hekies fortwater969@163.com
 * @LastEditTime: 2023-04-24 15:32:02
 * @FilePath: \FOC_F103C8T6\USER\uart\bsp_uart.c
 * @Description: 
 * @Copyright (c) 2023 by ${git_name_email}, All Rights Reserved. 
****************************************************************************************
*/
#include "bsp_uart.h"

#if ENABLE_USART1
u8 USART_RX_BUF[USART_REC_LEN];

static void usart1_nvic_init(void)
{
    NVIC_InitTypeDef NVIC_InitStructure = {0};
    NVIC_InitStructure.NVIC_IRQChannel = USART1_IRQn;
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1;
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 1;
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
    NVIC_Init(&NVIC_InitStructure);
}

void usart_init(USART_TypeDef *pUSARTx, u32 bound)
{
    GPIO_InitTypeDef GPIO_InitStructure = {0};
    USART_InitTypeDef USART_InitStructure = {0};

    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART1, ENABLE);
    
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_9;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
    GPIO_Init(GPIOA, &GPIO_InitStructure);

    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
    GPIO_Init(GPIOA, &GPIO_InitStructure);

    USART_InitStructure.USART_BaudRate = bound;
    USART_InitStructure.USART_WordLength = USART_WordLength_8b;
    USART_InitStructure.USART_StopBits = USART_StopBits_1;
    USART_InitStructure.USART_Parity = USART_Parity_No;
    USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
    USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;
    USART_Init(pUSARTx, &USART_InitStructure);

    usart1_nvic_init();
    USART_ITConfig(pUSARTx, USART_IT_RXNE, ENABLE);//使能接收中断
    USART_Cmd(pUSARTx, ENABLE);
}

/**
****************************************************************************************
 * @description: 
 * @return {*}
****************************************************************************************
*/
void USART1_IRQHandler(void)
{
    uint8_t tmp;
    if(USART_GetITStatus(USART1, USART_IT_RXNE) != RESET)
    {
        tmp = USART_ReceiveData(USART1);
        USART_SendData(USART1, tmp);
    }
}

#if ENABLE_USE_MICRO_LIB
#include <stdio.h>
int fputc(int ch, FILE *f)
{
    USART_SendData(USART1, (uint8_t) ch);

    while (USART_GetFlagStatus(USART1, USART_FLAG_TXE) == RESET);

    return (ch);
}

int fgetc(FILE *f)
{
    while(USART_GetFlagStatus(USART1, USART_FLAG_RXNE) == RESET);

    return (int)USART_ReceiveData(USART1);
}
#else
#pragma import(__use_no_semihosting) 
struct __FILE 
{ 
    int handle; 
}; 

FILE __stdout;
void _sys_exit(int x) 
{
    x = x; 
}

int fputc(int ch, FILE *f)
{
    while((USART1->SR & 0X40) == 0);
    USART1->DR = (u8) ch;
    return ch;
}
#endif

/**
****************************************************************************************
 * @description: 
 * @param {USART_TypeDef} *pUSARTx
 * @param {uint8_t} ch
 * @return {*}
****************************************************************************************
*/
void usart_send_byte(USART_TypeDef *pUSARTx, uint8_t ch)
{
    USART_SendData(pUSARTx,ch);
    while(USART_GetFlagStatus(pUSARTx, USART_FLAG_TXE) == RESET);
}

void usart_send_arry(USART_TypeDef *pUSARTx, uint8_t *array, uint16_t len)
{
    uint8_t i;

    for(i = 0; i < len; i++)
    {
        usart_send_byte(pUSARTx, array[i]);	
    }
    while(USART_GetFlagStatus(pUSARTx, USART_FLAG_TC) == RESET);
}


void usart_send_string(USART_TypeDef *pUSARTx, char *str)
{
    unsigned int k=0;
    do 
    {
        usart_send_byte( pUSARTx, *(str + k) );
        k++;
    }while(*(str + k) != '\0');

    while(USART_GetFlagStatus(pUSARTx, USART_FLAG_TC) == RESET);
}

void usart_send_halfword(USART_TypeDef *pUSARTx, uint16_t ch)
{
    uint8_t temp_h, temp_l;

    temp_h = (ch & 0xff00) >> 8;
    temp_l = ch & 0xff;

    USART_SendData(pUSARTx, temp_h);
    while(USART_GetFlagStatus(pUSARTx, USART_FLAG_TXE) == RESET);

    USART_SendData(pUSARTx, temp_l);
    while(USART_GetFlagStatus(pUSARTx, USART_FLAG_TXE) == RESET);
}

#endif


#include "bsp_uart.h"
#include "config.h"

u8 USART_RX_BUF[USART_REC_LEN];
//u16 USART_RX_STA = 0; 

//static void NVIC_Config(void)
//{
//    NVIC_InitTypeDef NVIC_InitStructure = {0};
//	
//    NVIC_InitStructure.NVIC_IRQChannel = USART1_IRQn;
//    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 3;
//    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 3;
//    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
//    NVIC_Init(&NVIC_InitStructure);
//}

void USART1_Configuration(u32 bound)
{
    GPIO_InitTypeDef GPIO_InitStructure = {0};
    USART_InitTypeDef USART_InitStructure = {0};

    RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART1 | RCC_APB2Periph_GPIOA, ENABLE);
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

    USART_Init(USART1, &USART_InitStructure);
//    USART_ITConfig(USART1, USART_IT_RXNE, ENABLE);
    USART_Cmd(USART1, ENABLE);

//    NVIC_Config();
}

//void USART1_IRQHandler(void)
//{
//	u8 Res;
//	if(USART_GetITStatus(USART1, USART_IT_RXNE) != RESET)
//	{
//		Res = USART_ReceiveData(USART1);

//		if((USART_RX_STA & 0x8000) == 0)
//		{
//			if(USART_RX_STA & 0x4000)
//			{
//				if(Res != 0x0a)
//					USART_RX_STA = 0;
//				else 
//					USART_RX_STA |= 0x8000;
//			}
//			else
//			{	
//				if(Res == 0x0d)
//				{
//					USART_RX_STA |= 0x4000;
//				}
//				else
//				{
//					USART_RX_BUF[USART_RX_STA & 0X3FFF] = Res;
//					USART_RX_STA++;
//					if(USART_RX_STA > (USART_REC_LEN-1))
//					{
//						USART_RX_STA = 0;
//					}
//				}		 
//			}
//		}   		 
//	} 
//}

#if USE_MICRO_LIB
#include <stdio.h>
int fputc(int ch, FILE *f)
{
	while((USART1->SR & 0x40) == RESET);
	USART1->DR = (uint8_t)ch;
	return ch;
}
#else
#pragma import(__use_no_semihosting) 
//标准库需要的支持函数 
struct __FILE 
{ 
	int handle; 
}; 

FILE __stdout;
//定义_sys_exit()以避免使用半主机模式
void _sys_exit(int x) 
{
	x = x; 
}

//重定义fputc函数 
int fputc(int ch, FILE *f)
{      
	while((USART1->SR & 0X40) == 0);//循环发送,直到发送完毕
    USART1->DR = (u8) ch;      
	return ch;
}
#endif


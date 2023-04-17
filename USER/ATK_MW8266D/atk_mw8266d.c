#include "atk_mw8266d.h"
#include "app_timer.h"
#include "smarttimer.h"
#include "elog.h"
#include <stdarg.h>
#include <stdio.h>
#include <string.h>

#if ATK_MW8266D_ENABLE

//串口接收缓存区
u8 USART3_RX_BUF[ATK_MW8266D_UART_RX_BUF_SIZE];//接收缓冲,最大USART3_MAX_RECV_LEN个字节.

vu16 usart3_rx_sta = 0;

static struct
{
    uint8_t buf[ATK_MW8266D_UART_RX_BUF_SIZE];              /* 帧接收缓冲 */
    struct
    {
        uint16_t len    : 15;                               /* 帧接收长度，sta[14:0] */
        uint16_t finsh  : 1;                                /* 帧接收完成标志，sta[15] */
    } sta;                                                  /* 帧状态信息 */
} g_uart_rx_frame = {0};                                    /* ATK-MW8266D UART接收帧缓冲信息结构体 */

static uint8_t g_uart_tx_buf[ATK_MW8266D_UART_TX_BUF_SIZE];    /* ATK-MW8266D UART发送缓冲 */

static void atk_mw8266d_hw_init(void)
{
	GPIO_InitTypeDef GPIO_InitStructure = {0};
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);
	
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOB, &GPIO_InitStructure);
	
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_11;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
	GPIO_Init(GPIOB, &GPIO_InitStructure);
}

static void usart3_nvic_config(void)
{
	NVIC_InitTypeDef NVIC_InitStructure = {0};
	
	NVIC_InitStructure.NVIC_IRQChannel = USART3_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 2;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 1;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_InitStructure);
}

void atk_mw8266d_usart_config(u32 bound)
{
	atk_mw8266d_hw_init();
	usart3_nvic_config();
	USART_InitTypeDef USART_InitStructure = {0};
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART3, ENABLE);
	
	USART_DeInit(USART3);
	
	USART_InitStructure.USART_BaudRate = bound;
	USART_InitStructure.USART_WordLength = USART_WordLength_8b;
	USART_InitStructure.USART_StopBits = USART_StopBits_1;
	USART_InitStructure.USART_Parity = USART_Parity_No;
	USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
	USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;

	USART_Init(USART3, &USART_InitStructure);
	USART_ITConfig(USART3, USART_IT_RXNE, ENABLE);
	USART_Cmd(USART3, ENABLE);
	
	TIM2_Init(999, 7199);//10ms中断
	usart3_rx_sta = 0;//清零
	TIM_Cmd(TIM3, DISABLE);
}

void atk_mw8266d_uart_printf(char *fmt, ...)
{
	va_list ap;
	u16 buf_len, i;
	
	va_start(ap, fmt);
	vsprintf((char *)g_uart_tx_buf, fmt, ap);
	va_end(ap);
	
	buf_len = strlen((const char *)g_uart_tx_buf);
	for(i = 0; i < buf_len; i++)
	{
		while(USART_GetFlagStatus(USART3, USART_FLAG_TC) == RESET);
		USART_SendData(USART3, g_uart_tx_buf[i]);
	}
}

void USART3_IRQHandler(void)
{
	u8 res;
	if(USART_GetITStatus(USART3, USART_IT_RXNE) != RESET)
	{
		res = USART_ReceiveData(USART3);
		if((usart3_rx_sta & (1 << 15)) == 0)
		{
			if(usart3_rx_sta < ATK_MW8266D_UART_RX_BUF_SIZE)
			{
				TIM_SetCounter(TIM2, 0);
				if(usart3_rx_sta == 0)
				{
					TIM_Cmd(TIM2,ENABLE);
				}
				USART3_RX_BUF[usart3_rx_sta++] = res;
			}
			else
			{
				usart3_rx_sta |= (1 << 15);
			}
		}
	}
}

/**
 * @brief       获取ATK-MW8266D UART接收到的一帧数据的长度
 * @param       无
 * @retval      0   : 未接收到一帧数据
 *              其他: 接收到的一帧数据的长度
 */
uint16_t atk_mw8266d_uart_rx_get_frame_len(void)
{
    if(g_uart_rx_frame.sta.finsh == 1)
    {
        return g_uart_rx_frame.sta.len;
    }
    else
    {
        return 0;
    }
}

/**
 * @brief       获取ATK-MW8266D UART接收到的一帧数据
 * @param       无
 * @retval      NULL: 未接收到一帧数据
 *              其他: 接收到的一帧数据
 */
uint8_t *atk_mw8266d_uart_rx_get_frame(void)
{
    if(g_uart_rx_frame.sta.finsh == 1)
    {
        g_uart_rx_frame.buf[g_uart_rx_frame.sta.len] = '\0';
        return g_uart_rx_frame.buf;
    }
    else
    {
        return NULL;
    }
}

/**
 * @brief       ATK-MW8266D UART重新开始接收数据
 * @param       无
 * @retval      无
 */
void atk_mw8266d_uart_rx_restart(void)
{
    g_uart_rx_frame.sta.len = 0;
    g_uart_rx_frame.sta.finsh = 0;
}

//ATK-ESP8266发送命令后,检测接收到的应答
//str:期待的应答结果
//返回值:0,没有得到期待的应答结果
//    其他,期待应答结果的位置(str的位置)
u8* atk_8266_check_cmd(char *str)
{
	char *strx = 0;
	if(usart3_rx_sta & 0X8000)//接收到一次数据了
	{
		USART3_RX_BUF[usart3_rx_sta & 0X7FFF] = 0;//添加结束符
		strx = strstr((const char*)USART3_RX_BUF, (const char*)str);
	}
	return (u8*)strx;
}

//向ATK-ESP8266发送命令
//cmd:发送的命令字符串
//ack:期待的应答结果,如果为空,则表示不需要等待应答
//waittime:等待时间(单位:10ms)
//返回值:0,发送成功(得到了期待的应答结果)
//       1,发送失败
u8 atk_8266_send_cmd(char *cmd, char *ack, u16 waittime)
{
	u8 res=0; 
	usart3_rx_sta = 0;
	log_d("%s\r\n", cmd);//发送命令
	if(ack && waittime)//需要等待应答
	{
		while(--waittime)//等待倒计时
		{
			stim_delay(10);
			if(usart3_rx_sta & 0X8000)//接收到期待的应答结果
			{
				if(atk_8266_check_cmd(ack))
				{
					log_d("ack:%s\r\n", (u8*)ack);
					break;//得到有效数据 
				}
				usart3_rx_sta = 0;
			} 
		}
		if(waittime == 0)
		{
			res = 1;
		}
	}
	return res;
}

//ATK-ESP8266退出透传模式
//返回值:0,退出成功;
//       1,退出失败
u8 atk_8266_quit_trans(void)
{
	while((USART3->SR & 0X40) == 0);//等待发送空
	USART3->DR = '+';
	stim_delay(15);//大于串口组帧时间(10ms)
	while((USART3->SR & 0X40) == 0);//等待发送空
	USART3->DR = '+';
	stim_delay(15);//大于串口组帧时间(10ms)
	while((USART3->SR & 0X40) == 0);//等待发送空
	USART3->DR = '+';
	stim_delay(500);//等待500ms
	return atk_8266_send_cmd("AT", "OK", 20);//退出透传判断.
}

void atk_mw8266d_test(void)
{
	while(atk_8266_send_cmd("AT", "OK", 20))//检查WIFI模块是否在线
	{
		atk_8266_quit_trans();//退出透传
		atk_8266_send_cmd("AT+CIPMODE=0", "OK", 200);//关闭透传模式
	}
}
#endif

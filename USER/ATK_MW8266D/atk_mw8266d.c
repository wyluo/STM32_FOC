#include "atk_mw8266d.h"
#include "elog.h"
#include <stdarg.h>
#include <stdio.h>
#include <string.h>

#if MW8266D_ENABLE

static struct{
    uint8_t buf[MW8266D_UART_RX_BUF_SIZE];
    struct{
        uint16_t len:15;
        uint16_t finish:1;
    }sta;
}g_uart_rx_frame = {0};

void usart3_init(u32 bound)
{
    NVIC_InitTypeDef NVIC_InitStructure = {0};
    GPIO_InitTypeDef GPIO_InitStructure = {0};
    USART_InitTypeDef USART_InitStructure = {0};

    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART3, ENABLE);

    USART_DeInit(USART3);

    //TX
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(GPIOB, &GPIO_InitStructure);

    //RX
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_11;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
    GPIO_Init(GPIOB, &GPIO_InitStructure);

    USART_InitStructure.USART_BaudRate = bound;
    USART_InitStructure.USART_WordLength = USART_WordLength_8b;
    USART_InitStructure.USART_StopBits = USART_StopBits_1;
    USART_InitStructure.USART_Parity = USART_Parity_No;
    USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
    USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;
    USART_Init(USART3, &USART_InitStructure);

    USART_Cmd(USART3, ENABLE);
    USART_ITConfig(USART3, USART_IT_RXNE, ENABLE);//enable usart receive interrupt
    USART_ITConfig(USART3, USART_IT_IDLE, ENABLE);

    NVIC_InitStructure.NVIC_IRQChannel = USART3_IRQn;
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 2;
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 3;
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
    NVIC_Init(&NVIC_InitStructure);
}

void USART3_IRQHandler(void)
{
    u8 ret;
    if(USART_GetITStatus(USART3, USART_IT_RXNE) != RESET)
    {
        ret = USART_ReceiveData(USART3);
        if(g_uart_rx_frame.sta.len < (MW8266D_UART_RX_BUF_SIZE - 1))
        {
            g_uart_rx_frame.buf[g_uart_rx_frame.sta.len] = ret;
            g_uart_rx_frame.sta.len++;
        }
        else
        {
            g_uart_rx_frame.sta.len = 0;
            g_uart_rx_frame.buf[g_uart_rx_frame.sta.len] = ret;
            g_uart_rx_frame.sta.len++;
        }
    }
    
    if(USART_GetITStatus(USART3, USART_IT_IDLE) == SET)
    {
        g_uart_rx_frame.sta.finish = 1;
        ret = USART_ReceiveData(USART3);
    }
}

/**
 * @brief       ATK-MW8266D UART重新开始接收数据
 * @param       无
 * @retval      无
 */
static void mw8266d_uart_rx_restart(void)
{
    g_uart_rx_frame.sta.len = 0;
    g_uart_rx_frame.sta.finish = 0;
}

/**
 * @brief       ATK-MW8266D发送AT指令
 * @param       cmd    : 待发送的AT指令
 *              ack    : 等待的响应
 *              timeout: 等待超时时间
 * @retval      ATK_MW8266D_EOK     : 函数执行成功
 *              ATK_MW8266D_ETIMEOUT: 等待期望应答超时，函数执行失败
 */
uint8_t atk_mw8266d_send_at_cmd(char *cmd, char *ack, uint32_t timeout)
{
    uint8_t *ret = NULL;
    mw8266d_uart_rx_restart();
}

/**
 * @brief       ATK-MW8266D AT指令测试
 * @param       无
 * @retval      ATK_MW8266D_EOK  : AT指令测试成功
 *              ATK_MW8266D_ERROR: AT指令测试失败
 */
uint8_t mw8266d_at_test(void)
{
    uint8_t ret;
    uint8_t i;

    for(i = 0; i < 10; i++)
    {
        ret = mw8266d_sent_at_cmd();
    }
}

#endif

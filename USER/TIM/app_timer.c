#include "app_timer.h"
#include "../uart/bsp_uart.h"
#include "../uart/app_log.h"
#include "elog.h"
#include "app_key_scan.h"
#include "smarttimer.h"

#if TIM_LOG_SWITCH
#define LOG_TIM(fmt, args...)   DBG_PRINTF(fmt, ##args)
#else
#define LOG_TIM(fmt, args...)
#endif

u32 uwTick;

void TIM2_Init(u16 arr, u16 psc)
{
	NVIC_InitTypeDef NVIC_InitStructure = {0};
	TIM_TimeBaseInitTypeDef TIM_TimeBaseInitStructure = {0};
	
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM2, ENABLE);
	
	TIM_TimeBaseInitStructure.TIM_Prescaler = psc; //Ԥ��Ƶֵ
	TIM_TimeBaseInitStructure.TIM_CounterMode = TIM_CounterMode_Down;
	TIM_TimeBaseInitStructure.TIM_Period = arr;//�Զ���װֵ
	TIM_TimeBaseInitStructure.TIM_ClockDivision = TIM_CKD_DIV4;
	TIM_TimeBaseInit(TIM2, &TIM_TimeBaseInitStructure);
	TIM_InternalClockConfig(TIM2);
	TIM_ITConfig(TIM2, TIM_IT_Update, ENABLE);
	TIM_Cmd(TIM2, ENABLE);
	
	NVIC_InitStructure.NVIC_IRQChannel = TIM2_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_InitStructure);
}

void TIM2_IRQHandler(void)
{
	if(TIM_GetITStatus(TIM2, TIM_IT_Update))
	{
//		log_i("this is KeyScan test");
		KeyScan();
	}
	TIM_ClearITPendingBit(TIM2, TIM_IT_Update);
}

/**
 * ��ȡ��ǰ�������е�ʱ��
 */

/*
* �������ƣ�SystemTickConfig
* ���������None
* �� �� ֵ��None
* ��    �ߣ�Barry
* ������������ʼ��ϵͳ����ʱ��
* �޸ļ�¼��None
*/
void SystemTickConfig(void)
{
	uint32_t Reload = 0;
	uint16_t TickRate = 1000u;

	SysTick_CLKSourceConfig(SysTick_CLKSource_HCLK);

	/* ����PCLK1ʱ�� */
	RCC_PCLK1Config(RCC_HCLK_Div1);
	/* ����HCLKʱ�� */
	RCC_HCLKConfig(RCC_SYSCLK_Div1);
	/* ����PCLK2ʱ�� */
	RCC_PCLK2Config(RCC_HCLK_Div1);

	Reload = SystemCoreClock / TickRate;
	/* ����SYSTICK�ж� */ 
	SysTick->CTRL |= SysTick_CTRL_TICKINT_Msk;
	/* ÿ1/TickRate Hz�ж�һ�� */
	SysTick->LOAD = Reload;
	/* ʹ��SysTick */
	SysTick->CTRL |= SysTick_CTRL_ENABLE_Msk;
}

/*
* �������ƣ�SysTick_Handler
* ���������None
* �� �� ֵ��None
* ��    �ߣ�Barry
* �����������δ�ʱ���жϷ�����
* �޸ļ�¼��None
*/
void SysTick_Handler(void)
{
    uwTick++;
	stim_tick();
}

/*
* �������ƣ�GetSysTickVal
* ���������None
* �� �� ֵ��None
* ��    �ߣ�Barry
* ������������ȡ��ǰSysTick����ֵ
* �޸ļ�¼��None
*/
uint32_t GetSysTickVal(void)
{
    return uwTick;
}



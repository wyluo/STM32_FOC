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
	
	TIM_TimeBaseInitStructure.TIM_Prescaler = psc; //预分频值
	TIM_TimeBaseInitStructure.TIM_CounterMode = TIM_CounterMode_Down;
	TIM_TimeBaseInitStructure.TIM_Period = arr;//自动重装值
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
 * 获取当前软件运行的时间
 */

/*
* 函数名称：SystemTickConfig
* 输入参数：None
* 返 回 值：None
* 作    者：Barry
* 功能描述：初始化系统工作时钟
* 修改记录：None
*/
void SystemTickConfig(void)
{
	uint32_t Reload = 0;
	uint16_t TickRate = 1000u;

	SysTick_CLKSourceConfig(SysTick_CLKSource_HCLK);

	/* 配置PCLK1时钟 */
	RCC_PCLK1Config(RCC_HCLK_Div1);
	/* 配置HCLK时钟 */
	RCC_HCLKConfig(RCC_SYSCLK_Div1);
	/* 配置PCLK2时钟 */
	RCC_PCLK2Config(RCC_HCLK_Div1);

	Reload = SystemCoreClock / TickRate;
	/* 开启SYSTICK中断 */ 
	SysTick->CTRL |= SysTick_CTRL_TICKINT_Msk;
	/* 每1/TickRate Hz中断一次 */
	SysTick->LOAD = Reload;
	/* 使能SysTick */
	SysTick->CTRL |= SysTick_CTRL_ENABLE_Msk;
}

/*
* 函数名称：SysTick_Handler
* 输入参数：None
* 返 回 值：None
* 作    者：Barry
* 功能描述：滴答定时器中断服务函数
* 修改记录：None
*/
void SysTick_Handler(void)
{
    uwTick++;
	stim_tick();
}

/*
* 函数名称：GetSysTickVal
* 输入参数：None
* 返 回 值：None
* 作    者：Barry
* 功能描述：获取当前SysTick计数值
* 修改记录：None
*/
uint32_t GetSysTickVal(void)
{
    return uwTick;
}




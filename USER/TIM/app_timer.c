#include "app_timer.h"
#include "../uart/bsp_uart.h"
#include "../uart/app_log.h"
#include "elog.h"
#include "smarttimer.h"
#include "atk_mw8266d.h"

#if TIM_LOG_SWITCH
#define LOG_TIM(fmt, args...)   DBG_PRINTF(fmt, ##args)
#else
#define LOG_TIM(fmt, args...)
#endif

u32 uwTick;

extern vu16 usart3_rx_sta;

void SystemTickConfig(void)
{
    uint32_t Reload = 0;
    uint16_t TickRate = 1000u;

    SysTick_CLKSourceConfig(SysTick_CLKSource_HCLK);

    RCC_PCLK1Config(RCC_HCLK_Div1);
    RCC_HCLKConfig(RCC_SYSCLK_Div1);
    RCC_PCLK2Config(RCC_HCLK_Div1);

    Reload = SystemCoreClock / TickRate;

    SysTick->CTRL |= SysTick_CTRL_TICKINT_Msk;

    SysTick->LOAD = Reload;

    SysTick->CTRL |= SysTick_CTRL_ENABLE_Msk;
}

void SysTick_Handler(void)
{
    uwTick++;
    stim_tick();
}

uint32_t GetSysTickVal(void)
{
    return uwTick;
}

#include "bsp_delay.h"

void delay_init(void)
{
	SysTick_CLKSourceConfig(SysTick_CLKSource_HCLK_Div8);//选择时钟源-外部时钟-HCLK/8
}

void delay_ms(u32 sysclk, u16 nms)
{	 		  	  
	 u32 tmp;
	 SysTick->LOAD = (u32)(nms * (sysclk / 8000));//设置重载值:n(us)*延时1us需要多少个SysTick时钟周期
	 SysTick->VAL = 0x00;
	 SysTick->CTRL |= SysTick_CTRL_ENABLE_Msk;//使能SysTick定时器
	 do
	 {
		tmp = SysTick->CTRL;
	 }while((tmp & 0x01) && !(tmp & (1 << 16)));//等待计数时间到达(位16)
	 SysTick->CTRL &= ~SysTick_CTRL_ENABLE_Msk;
	 SysTick->VAL = 0x00;
}

void delay_us(u32 sysclk, u32 nus)
{		
	 u32 tmp;
	 SysTick->LOAD = (u32)(nus * (sysclk / 8000000));//设置重载值:n(us)*延时1us需要多少个SysTick时钟周期
	 SysTick->VAL = 0x00;
	 SysTick->CTRL |= SysTick_CTRL_ENABLE_Msk;//使能SysTick定时器
	 do
	 {
		tmp = SysTick->CTRL;
	 }while((tmp & 0x01) && !(tmp & (1 << 16)));//等待计数时间到达(位16)
	 SysTick->CTRL &= ~SysTick_CTRL_ENABLE_Msk;
	 SysTick->VAL = 0x00;
}


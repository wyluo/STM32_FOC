#include "stm32f10x.h"
#include "bsp_uart.h"
#include "bsp_adc.h"
#include "bsp_delay.h"
#include "app_timer.h"
#include "app_log.h"
#include "config.h"
#include "bsp_iic.h"
#include "IQS7211E.h"
#include "app_log.h"
#include "elog.h"
#include "app_key.h"
#include "app_key_scan.h"
#include "smarttimer.h"
#include <stdio.h>
#include <stdlib.h>

#if MAIN_LOG_SWITCH
#define LOG_MAIN(fmt, args...)   DBG_PRINTF(fmt, ##args)
#else
#define LOG_MAIN(fmt, args...)
#endif

struct date{
	uint16_t year;
	uint8_t  month;
	uint8_t  day;
	uint8_t hour;
	uint8_t minute;
	uint8_t second;
};
struct date m_date;

const char datestr[] = "2023:04:06#17:16:00";
static int8_t sysled_id;
static int8_t syskey_id;

static void remove_sysled(void)
{
	stim_kill_event(sysled_id);
}

static void sys_ledflash(void)
{
	static uint8_t flag = 0;

	if(flag == 0)
	{
		GPIO_ResetBits(SYS_LEDPORT, SYS_LEDCTRL);
		log_i("led off\r\n");
	}
	else
	{
		GPIO_SetBits(SYS_LEDPORT, SYS_LEDCTRL);
		log_i("led on\r\n");
	}
	flag = !flag;
}

static void Key_Task(void)
{
	syskey_id = stim_loop(5, KeyScan, STIM_LOOP_FOREVER);
}

static void sys_init(void)
{
	GPIO_InitTypeDef GPIO_InitStructure = {0};

	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOD, ENABLE);

	GPIO_InitStructure.GPIO_Pin = SYS_LEDCTRL;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_Init(SYS_LEDPORT, &GPIO_InitStructure);

	sysled_id = stim_loop(500, sys_ledflash, STIM_LOOP_FOREVER);
	stim_runlater(20000, remove_sysled);	
}

static void runlater_test(void)
{
	log_i("after runlater===>[%02d:%02d:%02d]\r\n", m_date.hour, m_date.minute, m_date.second);
	log_i("before delay===>[%02d:%02d:%02d]\r\n", m_date.hour, m_date.minute, m_date.second);
	stim_delay(2000);
	log_i("after delay===>[%02d:%02d:%02d]\r\n", m_date.hour, m_date.minute, m_date.second);
}

static void increaseday(uint8_t limit)
{
	if(++m_date.day > limit)
	{
		m_date.day = 1;
		if(++m_date.month > 12)
		{
			m_date.month = 1;
			m_date.year++;
		}
	}
}

static uint8_t isLeap(uint16_t year)
{
	if(year % 4 == 0 && \
		year %100 != 0 && \
		year % 400 == 0)
	{
		return 1;
	}
	else
	{
		return 0;
	}
}

static void changeday(void)
{
	switch(m_date.month)
	{
		case 1:
		case 3:
		case 5:
		case 7:
		case 8:
		case 10:
		case 12:
			increaseday(31);
		break;
		
		case 4:
		case 6:
		case 9:
		case 11:
		  increaseday(30);
		break;
		
		case 2:
		if(isLeap(m_date.year) == 1)
		{
			increaseday(29);
		}
		else
		{
			increaseday(28);
		}
		break;
		
		default:
			break;
	}
}

static void simulation_rtc(void)
{
	if(++m_date.second == 60)
	{
		m_date.second = 0;
		if(++m_date.minute == 60)
		{
			m_date.minute = 0;
			if(++m_date.hour == 24)
			{
				m_date.hour = 0;
				changeday();
			}
		}
	}
//	log_i("time===>[%02d:%02d:%02d]\r\n", m_date.hour, m_date.minute, m_date.second);
}

static void set_timetick(void)
{
	//xxxx:xx:xx#xx:xx:xx

	m_date.year = atoi(datestr);
	m_date.month = atoi(datestr + 5);
	m_date.day = atoi(datestr + 8);
	m_date.hour = atoi(datestr + 11);
	m_date.minute = atoi(datestr + 14);
	m_date.second = atoi(datestr + 17);

	stim_loop(1000, simulation_rtc, STIM_LOOP_FOREVER);
    
}/* -----  end of static function time_tick  ----- */

int main(void)
{
	stim_init();
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);
		
	USART1_Configuration(115200);
	Easylogger_Init();

	sys_init();
	set_timetick();
	app_Key_Init();
	
//	TIM2_Init(99, 7199);//10ms
	ANA_IIC_GPIO_Init();
//	IQS7211E_Init();
//	IQS7211E_Extix_Init();
//	LOG_MAIN("this is soft_timer test\r\n");
	log_a("This is assert log output!");
	log_e("This is error log output!");
	log_w("This is warning log output!");
	log_i("This is info log output!");
	log_d("This is debug log output!");
	log_v("This is verbose log output!");
	
	log_i("before runlater===>[%02d:%02d:%02d]\r\n", m_date.hour, m_date.minute, m_date.second);
	stim_runlater(1000, runlater_test);
	Key_Task();

	while(1)
	{
		stim_mainloop();
	}
}


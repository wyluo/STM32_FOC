#include "app_key.h"
#include "app_timer.h"
#include "app_key_scan.h"
#include "elog.h"
#include "bsp_oled.h"
#include "app_oled.h"

u32 KeyVal;
stKeyFunc KeyFunc;

extern key_conrol_t key_control[KEY_NUM];
extern const key_cfg_t key_cfg[KEY_NUM];

/*
* 函数名称：KeyScan
* 输入参数：None
* 返 回 值：None
* 作    者：Barry
* 功能描述：按键扫描，检测单击、双击、长按、短按抬起事件
* 修改记录：None
*/
void KeyScan(void)
{
//	int i = 0;
	
    /* 按键按下的起始时间 */
    static uint32_t KeyStartTime = 0;
    /* 按键的触发标志 */
    static uint8_t  TrigFlag = 0;
    /* 上一次按下的键值 */
    static uint8_t  PreKeyVal = 0;
    /* 等待按键释放 */
    static uint8_t  WaitKeyUp = 0;
    /* 单击起始时间 */
    static uint32_t ClickStartTime = 0;
    /* 上次单击时间 */
    static uint32_t LastClickTime = 0;
    /* 双击标志位 */
    static uint8_t DoubleClickFlag = 0;
	
	KeyVal = (KEY4_READ << 3) | (KEY3_READ << 2) | (KEY2_READ << 1) | KEY1_READ;
	
//	for(i = 0; i < KEY_NUM; i++)
	{
//		if(check_key_realse(i))
		{
//			if(key_control[i].key_down_cnt > 4 && key_control[i].key_state != KEY_HOLD)
			{
//				log_d("KEY_TYPE_111.\r\n");
			}
			/* 按键在按下的状态--标记长按事件 */
			if(WaitKeyUp)
			{
				if(KeyVal == PreKeyVal) 
				{
					return;
				}

				WaitKeyUp = 0;
				KeyFunc.KeyType = KEY_TYPE_LONG_UP;

				//log_w("KEY_TYPE_LONG_UP.\r\n");
			}
		
			/* 有按键被按下 */
			if(KeyVal != 0x0F)
			{
				if(TrigFlag == 0)
				{
					TrigFlag = 1;   
					/* 保存键值 */
					PreKeyVal = KeyVal;
					/* 获取当前系统时钟计数值 */
					KeyStartTime = GetSysTickVal();
				}
				/* 之前已经按下该按键，并保持这个键按下 */
				else if((TrigFlag == 1) && (PreKeyVal == KeyVal))
				{
					uint32_t KeyDownTime = GetSysTickVal() - KeyStartTime;

					/* 按键按下时间小于消抖时间--消抖处理 */
					if(KeyDownTime <= DEBOUNCE_TIME) 
						return; 
					/* 大于单击的最长时间--认定为长按走这个分支 */
					else if(KeyDownTime > CLICK_MAX_TIME)
					{
						/* 清空标志位 */
						TrigFlag = 0;
						/* 置位等待释放标志位 */
						WaitKeyUp = 1;
						/* 标记按键按下状态 */
						KeyFunc.KeyType = KEY_TYPE_LONG;

						log_i("KEY_TYPE_LONG.\r\n");
					}
				}
			}
		
			/* 没有按键按下或按键被释放 */
			else
			{
				/* 单击标志位 */
				static uint8_t ClickFlag = 0;

				if(TrigFlag != 0)
				{
					/* 按键按下到松开的时间 */
					uint32_t KeyTime = GetSysTickVal() - KeyStartTime;

					/* 按键按下时间小于消抖时间--消抖处理 */
					if(KeyTime <= DEBOUNCE_TIME) 
						return; 
					/* 按键按下的时间小于长按--短按（区分双击和单击） */
					else if(KeyTime <= CLICK_MAX_TIME)
					{
						if(GetSysTickVal() - LastClickTime <= DOUBLE_CLICK_TIME)
						{
							/* 置位双击标志位 */
							DoubleClickFlag = 1;
							/* 双击 */
							KeyFunc.KeyType = KEY_TYPE_DOUBLE_CLICK;
							ssd1306_off();

							log_i("KEY_TYPE_DOUBLE_CLICK.\r\n");
						}
						/* 单击 */
						else
						{
							/* 置位单击标志位 */
							ClickFlag = 1;
							/* 清除双击标志位 */
							DoubleClickFlag = 0;
							/* 更新单击释放的时间 */
							ClickStartTime = GetSysTickVal();
						}

						LastClickTime = GetSysTickVal();
					}
				}
				/* 单击标志位 且 大于双击的时间内没有双击的标志位被触发被认为是单击事件 */
				if((ClickFlag == 1) && ((GetSysTickVal() - ClickStartTime) > DOUBLE_CLICK_TIME) && (DoubleClickFlag == 0))
				{
					/* 单击抬起 */
					KeyFunc.KeyType = KEY_TYPE_SHORT_UP;
					/* 清除单击标志位 */
					ClickFlag = 0;
					/* 清除双击标志位 */
					DoubleClickFlag = 0;
					
					OLED_Fill_All(0xff);
					OLED_Fill_All(0x00);
					OLED_ShowString(1, 4, "Balance_Car");

					log_i("KEY_TYPE_SHORT_UP.\r\n");
				}

				TrigFlag = 0;
			}
		}
	}
}

void key_progess(void)
{
	int i = 0;
	for(i = 0; i < KEY_NUM; i++)
	{
		if(check_key_realse(i))
		{
//			log_d("[lwy_key_debug]key_down\r\n");
			if((key_control[i].key_down_cnt > 4) && (key_control[i].key_state != KEY_HOLD))
			{
				key_control[i].key_click_cnt++;
			}
			key_control[i].key_down_cnt = 0;
			if((key_control[i].key_state == KEY_RELEASE) && ( key_control[i].key_click_cnt == 0 ))
			{
				continue;
			}
			key_control[i].key_up_cnt++;
			if(key_control[i].key_click_cnt)
			{
				if(key_control[i].key_up_cnt > key_cfg[i].key_intervel_cnt)
				{
					if(key_control[i].key_click_cnt == 1)
					{
						if(key_cfg[i].key_short_click_func)
							key_cfg[i].key_short_click_func();
						key_control[i].key_event = KEY_SCLICK_EVENT;
					}
					else if(key_control[i].key_click_cnt == 2)
					{
						if(key_cfg[i].key_double_click_func)
							key_cfg[i].key_double_click_func();
						key_control[i].key_event = KEY_DCLICK_EVENT;
					}
					else if(key_control[i].key_click_cnt >= 3)
					{
						if(key_cfg[i].key_tclick_func)
							key_cfg[i].key_tclick_func();
						key_control[i].key_event = KEY_TCLICK_EVENT;
					}
					key_control[i].key_click_cnt = 0;					
				}
			}
			if(key_control[i].key_up_cnt > 4 && ((key_control[i].key_state != KEY_RELEASE) || key_control[i].key_event))
			{
				if(key_control[i].key_event && key_cfg[i].key_release_func)
				{
					key_cfg[i].key_release_func(key_control[i].key_event);
					key_control[i].key_event = 0;
				}
				key_control[i].key_state = KEY_RELEASE;
			}			
		}
		else
		{
			if(key_control[i].key_state != KEY_DOWN)
			{
				key_control[i].key_down_cnt++;
				key_control[i].key_up_cnt = 0;
				if((key_cfg[i].key_hold_cnt == 0) && (key_cfg[i].key_long_cnt == 0))//hold key no func
				{
					if(key_control[i].key_down_cnt > 4)
					{
						key_control[i].key_down_cnt = 0;
						key_control[i].key_state = KEY_DOWN;
						key_control[i].key_click_cnt++;
					}
				}
				else
				{
					if(key_control[i].key_click_cnt == 0)
					{
						if((key_control[i].key_down_cnt >= key_cfg[i].key_hold_cnt) \
								&& key_control[i].key_state != KEY_HOLD)
						{
							log_i("key hold\r\n");
							key_control[i].key_click_cnt = 0;
							key_control[i].key_state = KEY_HOLD;
							key_control[i].key_event = KEY_HOLD_EVENT;
							if(key_cfg[i].key_hold_func)
								key_cfg[i].key_hold_func();
						}
						else if((key_control[i].key_down_cnt >= key_cfg[i].key_long_cnt) \
								&& (key_cfg[i].key_long_cnt != 0))
						{
							log_i("key long");
							key_control[i].key_down_cnt = 0;
							key_control[i].key_click_cnt = 0;
							key_control[i].key_state = KEY_DOWN;
							key_control[i].key_event = KEY_LONG_EVENT;
							if(key_cfg[i].key_long_func)
								key_cfg[i].key_long_func();
						}						
					}
					else
					{
						if(key_control[i].key_down_cnt > 4)
						{
							key_control[i].key_down_cnt = 0;
							key_control[i].key_state = KEY_DOWN;
							key_control[i].key_click_cnt++;
						}						
					}
				}					
			}
		}
	}
}


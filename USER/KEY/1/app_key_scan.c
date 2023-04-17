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
* �������ƣ�KeyScan
* ���������None
* �� �� ֵ��None
* ��    �ߣ�Barry
* ��������������ɨ�裬��ⵥ����˫�����������̰�̧���¼�
* �޸ļ�¼��None
*/
void KeyScan(void)
{
//	int i = 0;
	
    /* �������µ���ʼʱ�� */
    static uint32_t KeyStartTime = 0;
    /* �����Ĵ�����־ */
    static uint8_t  TrigFlag = 0;
    /* ��һ�ΰ��µļ�ֵ */
    static uint8_t  PreKeyVal = 0;
    /* �ȴ������ͷ� */
    static uint8_t  WaitKeyUp = 0;
    /* ������ʼʱ�� */
    static uint32_t ClickStartTime = 0;
    /* �ϴε���ʱ�� */
    static uint32_t LastClickTime = 0;
    /* ˫����־λ */
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
			/* �����ڰ��µ�״̬--��ǳ����¼� */
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
		
			/* �а��������� */
			if(KeyVal != 0x0F)
			{
				if(TrigFlag == 0)
				{
					TrigFlag = 1;   
					/* �����ֵ */
					PreKeyVal = KeyVal;
					/* ��ȡ��ǰϵͳʱ�Ӽ���ֵ */
					KeyStartTime = GetSysTickVal();
				}
				/* ֮ǰ�Ѿ����¸ð�������������������� */
				else if((TrigFlag == 1) && (PreKeyVal == KeyVal))
				{
					uint32_t KeyDownTime = GetSysTickVal() - KeyStartTime;

					/* ��������ʱ��С������ʱ��--�������� */
					if(KeyDownTime <= DEBOUNCE_TIME) 
						return; 
					/* ���ڵ������ʱ��--�϶�Ϊ�����������֧ */
					else if(KeyDownTime > CLICK_MAX_TIME)
					{
						/* ��ձ�־λ */
						TrigFlag = 0;
						/* ��λ�ȴ��ͷű�־λ */
						WaitKeyUp = 1;
						/* ��ǰ�������״̬ */
						KeyFunc.KeyType = KEY_TYPE_LONG;

						log_i("KEY_TYPE_LONG.\r\n");
					}
				}
			}
		
			/* û�а������»򰴼����ͷ� */
			else
			{
				/* ������־λ */
				static uint8_t ClickFlag = 0;

				if(TrigFlag != 0)
				{
					/* �������µ��ɿ���ʱ�� */
					uint32_t KeyTime = GetSysTickVal() - KeyStartTime;

					/* ��������ʱ��С������ʱ��--�������� */
					if(KeyTime <= DEBOUNCE_TIME) 
						return; 
					/* �������µ�ʱ��С�ڳ���--�̰�������˫���͵����� */
					else if(KeyTime <= CLICK_MAX_TIME)
					{
						if(GetSysTickVal() - LastClickTime <= DOUBLE_CLICK_TIME)
						{
							/* ��λ˫����־λ */
							DoubleClickFlag = 1;
							/* ˫�� */
							KeyFunc.KeyType = KEY_TYPE_DOUBLE_CLICK;
							ssd1306_off();

							log_i("KEY_TYPE_DOUBLE_CLICK.\r\n");
						}
						/* ���� */
						else
						{
							/* ��λ������־λ */
							ClickFlag = 1;
							/* ���˫����־λ */
							DoubleClickFlag = 0;
							/* ���µ����ͷŵ�ʱ�� */
							ClickStartTime = GetSysTickVal();
						}

						LastClickTime = GetSysTickVal();
					}
				}
				/* ������־λ �� ����˫����ʱ����û��˫���ı�־λ����������Ϊ�ǵ����¼� */
				if((ClickFlag == 1) && ((GetSysTickVal() - ClickStartTime) > DOUBLE_CLICK_TIME) && (DoubleClickFlag == 0))
				{
					/* ����̧�� */
					KeyFunc.KeyType = KEY_TYPE_SHORT_UP;
					/* ���������־λ */
					ClickFlag = 0;
					/* ���˫����־λ */
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


#include "app_key.h"
#include "app_timer.h"
#include "app_key_scan.h"
#include "elog.h"

u32 KeyVal;
stKeyFunc KeyFunc;

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

            log_i("KEY_TYPE_SHORT_UP.\r\n");
        }

        TrigFlag = 0;
    }
}


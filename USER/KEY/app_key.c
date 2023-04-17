#include "app_key.h"
#include "hal_key.h"
#include "app_timer.h"
#include "elog.h"
#include "app_menu.h"
#if MULTIBUTTON_ENABLE
#include "multi_button.h"
extern struct Button btn1;
extern struct Button btn2;
#endif

u32 KeyVal;
u8 func_index = 0;
stKeyFunc KeyFunc;
key_conrol_t key_control[KEY_NUM];

extern Menu_table table[30];
extern void (*current_operation_index)();

const key_cfg_t key_cfg[KEY_NUM] =
{
    {
        .port = KEY1_GPIO_PORT,
        .key_pin = KEY1_GPIO_PIN,
        .key_hold_cnt = 200,
        .key_long_cnt = 300,
        .key_intervel_cnt = 50,
        .key_default_level = KEY_DOWN,
        .key_down_level = 0,
//      .key_short_click_func = key1_sclick_func,
//      .key_double_click_func = key1_dclick_func,
//      .key_tclick_func = key1_tclick_func,
//      .key_hold_func = key1_hold_func,
//      .key_long_func = key1_long_func,
//      .key_release_func = key1_release_func,
    },
    {
        .port = KEY2_GPIO_PORT,
        .key_pin = KEY2_GPIO_PIN,
        .key_hold_cnt = 200,
        .key_long_cnt = 300,
        .key_intervel_cnt = 50,
        .key_default_level = KEY_DOWN,
        .key_down_level = 0,
//      .key_short_click_func = power_key_sclick_func,
//      .key_double_click_func = power_key_dclick_func,
//      .key_tclick_func = power_key_tclick_func,
//      .key_hold_func = power_key_hold_func,
//      .key_long_func = power_key_long_func,
//      .key_release_func = power_key_release_func,
    },
    {
        .port = KEY3_GPIO_PORT,
        .key_pin = KEY3_GPIO_PIN,
        .key_hold_cnt = 200,
        .key_long_cnt = 300,
        .key_intervel_cnt = 50,
        .key_default_level = KEY_DOWN,
        .key_down_level = 0,
//      .key_short_click_func = power_key_sclick_func,
//      .key_double_click_func = power_key_dclick_func,
//      .key_tclick_func = power_key_tclick_func,
//      .key_hold_func = power_key_hold_func,
//      .key_long_func = power_key_long_func,
//      .key_release_func = power_key_release_func,
	},
	{
		.port = KEY4_GPIO_PORT,
		.key_pin = KEY4_GPIO_PIN,
		.key_hold_cnt = 200,
		.key_long_cnt = 300,
		.key_intervel_cnt = 50,
		.key_default_level = KEY_DOWN,
		.key_down_level = 0,
//		.key_short_click_func = power_key_sclick_func,
//		.key_double_click_func = power_key_dclick_func,
//		.key_tclick_func = power_key_tclick_func,
//		.key_hold_func = power_key_hold_func,
//		.key_long_func = power_key_long_func,
//		.key_release_func = power_key_release_func,
	},
};

void app_key_init(void)
{
    static int i = 0;

    GPIO_InitTypeDef GPIO_InitStructure = {0};

    memset(key_control, 0, sizeof(key_control));

    for(i = 0; i < KEY_NUM; i++)
    {
        GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;
        GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
        GPIO_InitStructure.GPIO_Pin = key_cfg[i].key_pin;
        GPIO_Init(key_cfg[i].port, &GPIO_InitStructure);
    }
}

void key_scan(void)
{
//    int i = 0;
    
    static uint32_t KeyStartTime = 0;
    static uint8_t  TrigFlag = 0;
    static uint8_t  PreKeyVal = 0;
    static uint8_t  WaitKeyUp = 0;
    static uint32_t ClickStartTime = 0;
    static uint32_t LastClickTime = 0;
    static uint8_t DoubleClickFlag = 0;
    
    KeyVal = (KEY4_READ << 3) | (KEY3_READ << 2) | (KEY2_READ << 1) | KEY1_READ;
    
//  for(i = 0; i < KEY_NUM; i++)
    {
//      if(check_key_realse(i))
        {
//          if(key_control[i].key_down_cnt > 4 && key_control[i].key_state != KEY_HOLD)
            {
//              log_d("KEY_TYPE_111.\r\n");
            }
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
            
//          for(i = 0; i < KEY_NUM; i++)
            {
//              log_d("%d\r\n", check_key_realse(i));
                if(KeyVal != 0x0F)
                {
                    if(TrigFlag == 0)
                    {
                        TrigFlag = 1;   
                        PreKeyVal = KeyVal;
                        KeyStartTime = GetSysTickVal();
                    }
                    else if((TrigFlag == 1) && (PreKeyVal == KeyVal))
                    {
                        uint32_t KeyDownTime = GetSysTickVal() - KeyStartTime;

                        if(KeyDownTime <= DEBOUNCE_TIME) 
                            return; 
                        else if(KeyDownTime > CLICK_MAX_TIME)
                        {
                            TrigFlag = 0;
                            WaitKeyUp = 1;
                            KeyFunc.KeyType = KEY_TYPE_LONG;
//                            OLED_Fill_All(0x00);
                            switch(read_gpio_state())
                            {
                                case KEY1:
                                    log_i("KEY1_TYPE_LONG.\r\n");
                                    if(func_index != 6)
                                    {
                                        func_index = table[func_index].next;
                                        OLED_Fill_All(0x00);
                                    }
                                    break;

                                case KEY2:
                                    log_i("KEY2_TYPE_LONG.\r\n");
                                    if(func_index != 6)
                                    {
                                        func_index = table[func_index].enter;
                                        OLED_Fill_All(0x00);
                                    }
                                    break;

                                case KEY3:
                                    log_i("KEY3_TYPE_LONG.\r\n");
                                    if(func_index != 6)
                                    {
                                        func_index = table[func_index].back;
                                        OLED_Fill_All(0x00);
                                    }
                                    break;

                                case KEY4:
                                    log_i("KEY4_TYPE_LONG.\r\n");
                                    break;

                                default:
                                    break;
                            }
                            current_operation_index = table[func_index].current_operation;
                            (*current_operation_index)();
                        }
                    }
                }
                else
                {
                    static uint8_t ClickFlag = 0;

                    if(TrigFlag != 0)
                    {
                        uint32_t KeyTime = GetSysTickVal() - KeyStartTime;

                        if(KeyTime <= DEBOUNCE_TIME) 
                            return; 
                        else if(KeyTime <= CLICK_MAX_TIME)
                        {
                            if(GetSysTickVal() - LastClickTime <= DOUBLE_CLICK_TIME)
                            {
                                DoubleClickFlag = 1;
                                KeyFunc.KeyType = KEY_TYPE_DOUBLE_CLICK;
                            }
                            else
                            {
                                ClickFlag = 1;
                                DoubleClickFlag = 0;
                                ClickStartTime = GetSysTickVal();
                            }

                            LastClickTime = GetSysTickVal();
                        }
                    }
                    if((ClickFlag == 1) && ((GetSysTickVal() - ClickStartTime) > DOUBLE_CLICK_TIME) && (DoubleClickFlag == 0))
                    {
                        KeyFunc.KeyType = KEY_TYPE_SHORT_UP;
                        ClickFlag = 0;
                        DoubleClickFlag = 0;
                        log_i("KEY_TYPE_SHORT_UP.\r\n");
                    }

                    TrigFlag = 0;
                }
            }
        }
    }
}


void app_key_process(void)
{
    button_attach(&btn1, PRESS_DOWN,       BTN1_PRESS_DOWN_Handler);
    button_attach(&btn1, PRESS_UP,         BTN1_PRESS_UP_Handler);
    button_attach(&btn1, PRESS_REPEAT,     BTN1_PRESS_REPEAT_Handler);
    button_attach(&btn1, SINGLE_CLICK,     BTN1_SINGLE_Click_Handler);
    button_attach(&btn1, DOUBLE_CLICK,     BTN1_DOUBLE_Click_Handler);
    button_attach(&btn1, LONG_PRESS_START, BTN1_LONG_PRESS_START_Handler);
    button_attach(&btn1, LONG_PRESS_HOLD,  BTN1_LONG_PRESS_HOLD_Handler);

    button_attach(&btn2, PRESS_DOWN,       BTN2_PRESS_DOWN_Handler);
    button_attach(&btn2, PRESS_UP,         BTN2_PRESS_UP_Handler);
    button_attach(&btn2, PRESS_REPEAT,     BTN2_PRESS_REPEAT_Handler);
    button_attach(&btn2, SINGLE_CLICK,     BTN2_SINGLE_Click_Handler);
    button_attach(&btn2, DOUBLE_CLICK,     BTN2_DOUBLE_Click_Handler);
    button_attach(&btn2, LONG_PRESS_START, BTN2_LONG_PRESS_START_Handler);
    button_attach(&btn2, LONG_PRESS_HOLD,  BTN2_LONG_PRESS_HOLD_Handler);
}

//uint8_t read_button_GPIO(uint8_t button_id)
//{
//	// you can share the GPIO read function with multiple Buttons
//	switch(button_id)
//	{
//		case btn1_id:
//			return GPIO_ReadInputDataBit(B1_GPIO_Port, B1_Pin);
//		default:
//			return 0;
//	}	
//}

#if MULTIBUTTON_ENABLE
//void multi_button_init_attach(void)
//{
//    button_attach(&btn1, PRESS_DOWN,       BTN1_PRESS_DOWN_Handler);
//    button_attach(&btn1, PRESS_UP,         BTN1_PRESS_UP_Handler);
//    //button_attach(&btn1, PRESS_REPEAT,     BTN1_PRESS_REPEAT_Handler);
//    //button_attach(&btn1, SINGLE_CLICK,     BTN1_SINGLE_Click_Handler);
//    //button_attach(&btn1, DOUBLE_CLICK,     BTN1_DOUBLE_Click_Handler);
//    //button_attach(&btn1, LONG_PRESS_START, BTN1_LONG_PRESS_START_Handler);
//    //button_attach(&btn1, LONG_PRESS_HOLD,  BTN1_LONG_PRESS_HOLD_Handler);
//}

void BTN1_PRESS_DOWN_Handler(void *btn)
{
    //do something...
    // log_d("BTN1_PRESS_DOWN_Handler");
}

void BTN1_PRESS_UP_Handler(void *btn)
{
    //do something...
    // log_d("BTN1_PRESS_UP_Handler");
}

void BTN1_PRESS_REPEAT_Handler(void *btn)
{
    // log_d("BTN1_PRESS_REPEAT_Handler");
}

void BTN1_SINGLE_Click_Handler(void *btn)
{
    log_d("BTN1_SINGLE_Click_Handler");
}
    
void BTN1_DOUBLE_Click_Handler(void *btn)
{
    log_d("BTN1_DOUBLE_Click_Handler");
}
    
void BTN1_LONG_PRESS_START_Handler(void *btn)
{
    log_d("BTN1_LONG_PRESS_START_Handler");
}
    
void BTN1_LONG_PRESS_HOLD_Handler(void *btn)
{
    log_d("BTN1_LONG_PRESS_HOLD_Handler");
}
    
void BTN2_PRESS_DOWN_Handler(void *btn)
{
    //do something...
    log_d("BTN2_PRESS_DOWN_Handler");
}

void BTN2_PRESS_UP_Handler(void *btn)
{
    //do something...
    log_d("BTN2_PRESS_UP_Handler");
}

void BTN2_PRESS_REPEAT_Handler(void *btn)
{
    log_d("BTN2_PRESS_REPEAT_Handler");
}

void BTN2_SINGLE_Click_Handler(void *btn)
{
    log_d("BTN2_SINGLE_Click_Handler");
}
    
void BTN2_DOUBLE_Click_Handler(void *btn)
{
    log_d("BTN2_DOUBLE_Click_Handler");
}
    
void BTN2_LONG_PRESS_START_Handler(void *btn)
{
    log_d("BTN2_LONG_PRESS_START_Handler");
}
    
void BTN2_LONG_PRESS_HOLD_Handler(void *btn)
{
    log_d("BTN2_LONG_PRESS_HOLD_Handler");
}
#endif

//void get_btn_press(void)
//{
//
//}

uint8_t read_gpio_state(void)
{
    if(GPIO_ReadInputDataBit(KEY1_GPIO_PORT, KEY1_GPIO_PIN) == 0)
        return KEY1;
    else if(GPIO_ReadInputDataBit(KEY1_GPIO_PORT, KEY2_GPIO_PIN) == 0)
        return KEY2;
    else if(GPIO_ReadInputDataBit(KEY1_GPIO_PORT, KEY3_GPIO_PIN) == 0)
        return KEY3;
    else if(GPIO_ReadInputDataBit(KEY1_GPIO_PORT, KEY4_GPIO_PIN) == 0)
        return KEY4;
    else
        return KEY_NONE;
}

#if MULTIBUTTON_ENABLE
uint8_t read_button_GPIO(uint8_t button_id)
{
    // you can share the GPIO read function with multiple Buttons
    switch(button_id)
    {
        case KEY1:
            return GPIO_ReadInputDataBit(KEY1_GPIO_PORT, KEY1_GPIO_PIN);
        
        case KEY2:
            return GPIO_ReadInputDataBit(KEY1_GPIO_PORT, KEY2_GPIO_PIN);
        
        default:
            return 0;
    }
}
#endif

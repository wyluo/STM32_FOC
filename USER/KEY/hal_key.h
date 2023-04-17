#ifndef HAL_KEY_H
#define HAL_KEY_H

#include "stm32f10x.h"
#include "config.h"

typedef struct{
	unsigned int key_down_cnt;
	unsigned int key_up_cnt;
	unsigned char key_state;
	unsigned char key_click_cnt;
	unsigned char key_event;
}key_conrol_t;

typedef enum
{
    KEY_NONE = 0,
    KEY1     = 1,
    KEY2     = 2,
    KEY3     = 3,
    KEY4     = 4,
}eKeyList;

typedef enum
{
    KEY_TYPE_NULL = 0,         /*! 没有按键*/ 
    KEY_TYPE_DOUBLE_CLICK = 1, /*! 双击按键 */
    KEY_TYPE_DOWN = 2,         /*! 按键按下*/
    KEY_TYPE_LONG = 3,         /*! 按键长按*/
    KEY_TYPE_HOLD = 4,         /*! 按键保持*/
    KEY_TYPE_SHORT_UP = 5,     /*! 按键短按弹起*/
    KEY_TYPE_LONG_UP = 6,      /*! 按键长按弹起*/
    KEY_TYPE_HOLD_UP = 7,      /*! 按键保持弹起*/
    KEY_TYPE_ALL = 255,        /*! 任意按键类型，不包含双击按键 */

}eKeyType;

typedef struct 
{
    eKeyList Key;
    eKeyType KeyType;
}stKeyFunc;


#endif


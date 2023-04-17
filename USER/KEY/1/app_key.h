#ifndef APP_KEY_H
#define APP_KEY_H

#include "stm32f10x.h"
#include "key_hal.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <stdbool.h>

#define KEY_NUM 4


/* ����1 GPIO���� */
#define  KEY1_GPIO_PORT          GPIOA
#define  KEY1_GPIO_PIN           GPIO_Pin_0
#define  KEY1_GPIO_MODE          GPIO_Mode_IPU
#define  KEY1_READ               GPIO_ReadInputDataBit(KEY1_GPIO_PORT, KEY1_GPIO_PIN)

/* ����2 GPIO���� */
#define  KEY2_GPIO_PORT          GPIOA
#define  KEY2_GPIO_PIN           GPIO_Pin_1
#define  KEY2_GPIO_MODE          GPIO_Mode_IPU
#define  KEY2_READ               GPIO_ReadInputDataBit(KEY2_GPIO_PORT, KEY2_GPIO_PIN)

/* ����3 GPIO���� */
#define  KEY3_GPIO_PORT          GPIOA
#define  KEY3_GPIO_PIN           GPIO_Pin_2
#define  KEY3_GPIO_MODE          GPIO_Mode_IPU
#define  KEY3_READ               GPIO_ReadInputDataBit(KEY3_GPIO_PORT, KEY3_GPIO_PIN)

/* ����4 GPIO���� */
#define  KEY4_GPIO_PORT          GPIOA
#define  KEY4_GPIO_PIN           GPIO_Pin_3
#define  KEY4_GPIO_MODE          GPIO_Mode_IPU
#define  KEY4_READ               GPIO_ReadInputDataBit(KEY4_GPIO_PORT, KEY4_GPIO_PIN)

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
    KEY_TYPE_NULL = 0,         /*! û�а���*/ 
    KEY_TYPE_DOUBLE_CLICK = 1, /*! ˫������ */
    KEY_TYPE_DOWN = 2,         /*! ��������*/
    KEY_TYPE_LONG = 3,         /*! ��������*/
    KEY_TYPE_HOLD = 4,         /*! ��������*/
    KEY_TYPE_SHORT_UP = 5,     /*! �����̰�����*/
    KEY_TYPE_LONG_UP = 6,      /*! ������������*/
    KEY_TYPE_HOLD_UP = 7,      /*! �������ֵ���*/
    KEY_TYPE_ALL = 255,        /*! ���ⰴ�����ͣ�������˫������ */

}eKeyType;

typedef enum
{
    KEY_EVENT_NULL = 0,        /*! û�а����¼�*/
    SELECT_UP_EVENT = 1,       /*������ѡ���¼�*/
    SELECT_DOWN_EVENT = 2,     /*������ѡ��ʱ��*/
    ENTER_MENU_EVENT = 3,      /*! ����˵��¼�*/
    RETURN_MENU_EVENT = 4,     /*��������һ���˵��¼�*/
}eKeyEvent;

typedef struct 
{
    eKeyList Key;
    eKeyType KeyType;
}stKeyFunc;

typedef struct{
	GPIO_TypeDef *port;
	unsigned short int key_pin;
//	GPIO_PinState valid;
//	GPIO_PinState invalid;
	unsigned int key_hold_cnt;
	unsigned int key_long_cnt;
	unsigned int key_intervel_cnt;
	void (*key_short_click_func) (void);
	void (*key_double_click_func) (void);
	void (*key_tclick_func) (void);
	void (*key_hold_func) (void);
	void (*key_long_func) (void);
	void (*key_release_func) (unsigned char event);
	unsigned char key_down_level;
	unsigned char key_default_level;
}key_cfg_t;

enum{
	KEY_DOWN,
	KEY_HOLD,
	KEY_RELEASE,
};

typedef struct{
	unsigned int key_down_cnt;
	unsigned int key_up_cnt;
	unsigned char key_state;
	unsigned char key_click_cnt;
	unsigned char key_event;
}key_conrol_t;

typedef struct{
	T_KEY_PRESS_HANDLE_T press_handle;
	unsigned int key_evt;
}KEY_MENU_T;

#define  DEBOUNCE_TIME           50         /*! ����ʱ��50ms*/
#define  CLICK_MAX_TIME          1200       /*! �����ʱ��(ms)�� ���ڴ�ʱ�䱻��Ϊ����*/
#define  DOUBLE_CLICK_TIME       220        /*! ˫�����ʱ��*/

void hal_key_init(void);
bool check_key_realse(unsigned char index);
void key1_sclick_func(void);
void key1_dclick_func(void);
void key1_tclick_func(void);
#endif

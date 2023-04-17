#ifndef APP_KEY_H
#define APP_KEY_H

#include "stm32f10x.h"
#include "config.h"

#define  DEBOUNCE_TIME           50         /*! 消抖时间50ms*/
#define  CLICK_MAX_TIME          1000       /*! 单击最长时间(ms)， 大于此时间被认为长按*/
#define  DOUBLE_CLICK_TIME       300        /*! 双击间隔时间*/

/* 按键1 GPIO配置 */
#define  KEY1_GPIO_PORT          GPIOA
#define  KEY1_GPIO_PIN           GPIO_Pin_0
#define  KEY1_GPIO_MODE          GPIO_Mode_IPU
#define  KEY1_READ               GPIO_ReadInputDataBit(KEY1_GPIO_PORT, KEY1_GPIO_PIN)

/* 按键2 GPIO配置 */
#define  KEY2_GPIO_PORT          GPIOA
#define  KEY2_GPIO_PIN           GPIO_Pin_1
#define  KEY2_GPIO_MODE          GPIO_Mode_IPU
#define  KEY2_READ               GPIO_ReadInputDataBit(KEY2_GPIO_PORT, KEY2_GPIO_PIN)

/* 按键3 GPIO配置 */
#define  KEY3_GPIO_PORT          GPIOA
#define  KEY3_GPIO_PIN           GPIO_Pin_2
#define  KEY3_GPIO_MODE          GPIO_Mode_IPU
#define  KEY3_READ               GPIO_ReadInputDataBit(KEY3_GPIO_PORT, KEY3_GPIO_PIN)

/* 按键4 GPIO配置 */
#define  KEY4_GPIO_PORT          GPIOA
#define  KEY4_GPIO_PIN           GPIO_Pin_3
#define  KEY4_GPIO_MODE          GPIO_Mode_IPU
#define  KEY4_READ               GPIO_ReadInputDataBit(KEY4_GPIO_PORT, KEY4_GPIO_PIN)

enum{
    KEY_DOWN = 0,
    KEY_HOLD,
    KEY_RELEASE,
};

typedef struct{
	GPIO_TypeDef *port;
	unsigned short int key_pin;
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

void app_key_init(void);
void app_key_process(void);

#if MULTIBUTTON_ENABLE
void multi_button_init_attach(void);
void BTN1_PRESS_DOWN_Handler(void* btn);
void BTN1_PRESS_UP_Handler(void* btn);
#endif

void key_scan(void);
uint8_t read_gpio_state(void);
#endif

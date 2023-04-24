#ifndef APP_KEY_H
#define APP_KEY_H

#include "stm32f10x.h"
#include "hal_key.h"
#include "config.h"

#define  DEBOUNCE_TIME           50
#define  CLICK_MAX_TIME          1000
#define  DOUBLE_CLICK_TIME       300

#define  KEY1_GPIO_PORT          GPIOA
#define  KEY1_GPIO_PIN           GPIO_Pin_2
#define  KEY1_GPIO_MODE          GPIO_Mode_IPU
#define  KEY1_READ               GPIO_ReadInputDataBit(KEY1_GPIO_PORT, KEY1_GPIO_PIN)

#define  KEY2_GPIO_PORT          GPIOA
#define  KEY2_GPIO_PIN           GPIO_Pin_3
#define  KEY2_GPIO_MODE          GPIO_Mode_IPU
#define  KEY2_READ               GPIO_ReadInputDataBit(KEY2_GPIO_PORT, KEY2_GPIO_PIN)

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

void BTN1_PRESS_DOWN_Handler(void *btn);
void BTN1_PRESS_UP_Handler(void *btn);
void BTN1_PRESS_REPEAT_Handler(void *btn);
void BTN1_SINGLE_Click_Handler(void *btn);
void BTN1_DOUBLE_Click_Handler(void *btn);
void BTN1_LONG_PRESS_START_Handler(void *btn);
void BTN1_LONG_PRESS_HOLD_Handler(void *btn);

void BTN2_PRESS_DOWN_Handler(void *btn);
void BTN2_PRESS_UP_Handler(void *btn);
void BTN2_PRESS_REPEAT_Handler(void *btn);
void BTN2_SINGLE_Click_Handler(void *btn);
void BTN2_DOUBLE_Click_Handler(void *btn);
void BTN2_LONG_PRESS_START_Handler(void *btn);
void BTN2_LONG_PRESS_HOLD_Handler(void *btn);

// void BTN3_PRESS_DOWN_Handler(void *btn);
// void BTN3_PRESS_UP_Handler(void *btn);
// void BTN3_PRESS_REPEAT_Handler(void *btn);
// void BTN3_SINGLE_Click_Handler(void *btn);
// void BTN3_DOUBLE_Click_Handler(void *btn);
// void BTN3_LONG_PRESS_START_Handler(void *btn);
// void BTN3_LONG_PRESS_HOLD_Handler(void *btn);
#endif

void key_scan(void);
uint8_t read_gpio_state(void);
uint8_t read_button_GPIO(uint8_t button_id);

#endif

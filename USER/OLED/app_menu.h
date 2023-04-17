#ifndef APP_MENU_H
#define APP_MENU_H

#include "stm32f10x.h"
#include "app_oled.h"

typedef struct
{
    u8 current;//µ±Ç°×´Ì¬Ë÷ÒýºÅ
    u8 next;
    u8 enter;
    u8 back;
    void (*current_operation)(void);
}Menu_table;

void tok_home_page(void);
void tok_temperature_page(void);
void tok_palygame_page(void);
void tok_setting_page(void);
void tok_info_page(void);
void tok_dht11_page(void);
void tok_conrtolGame_page(void);
void tok_set_page(void);
void tok_information_page(void);
void tok_led_page(void);


#endif // !APP_OLED_H


#ifndef APP_OLED_H
#define APP_OLED_H

#include "stm32f10x.h"
#include <stdint.h>
#include "i2c_core.h"
#include "stm32f4_fonts.h"

#define OLED12864_SLAVE0    0x3c

/* SSD1306 width in pixels */
#define SSD1306_WIDTH            128

/* SSD1306 LCD height in pixels */
#define SSD1306_HEIGHT           64

/**
 * @brief  SSD1306 color enumeration
 */
typedef enum
{
    SSD1306_COLOR_BLACK = 0x00, /*!< Black color, no pixel */
    SSD1306_COLOR_WHITE = 0x01  /*!< Pixel is set. Color depends on LCD */
}SSD1306_COLOR_t;

int8_t OLED12864_Init(struct i2c_dev_device *i2c_bus);
void OLED_Fill_All(uint8_t fill_Data);

void ssd1306_on(void);
void ssd1306_off(void);
void ssd1306_GotoXY(uint16_t x, uint16_t y);
//char SSD1306_Puts(char* str, TM_FontDef_t* Font, SSD1306_COLOR_t color);
void oled_display(void);

void OLED_ShowString(uint8_t Line, uint8_t Column, char *String);
void x_oled1306_DrawBMP(unsigned char x0, unsigned char y0, \
                        unsigned char x1, unsigned char y1, unsigned char BMP[]);
void oled1306_set_cursor(unsigned int y, unsigned int x);
void ssd1306_write_data(u8 data);
void oled1306_show_num(uint8_t line, uint8_t column, uint32_t num, uint8_t Length);


#endif

#include "app_oled.h"
#include "i2c_hw.h"
#include "code_lib.h"
#include <string.h>

struct i2c_dev_device *oled_i2c_dev = NULL;

/* Private SSD1306 structure */
typedef struct {
    uint16_t CurrentX;
    uint16_t CurrentY;
    uint8_t Inverted;
    uint8_t Initialized;
}SSD1306_t;

/* Private variable */
static SSD1306_t SSD1306;

/* SSD1306 data buffer */
static uint8_t ssd1306_buffer[SSD1306_WIDTH * SSD1306_HEIGHT / 8];

static uint32_t oled1306_pow(uint32_t x, uint32_t y);


//write oled12864 register
static char oled12864_write_reg0(u8 reg, u8 data)
{
    struct i2c_dev_message oled_msg[1];
    u8 buff[2] = {0};
    u8 slave_addr = 0;
    char ret = 0;
      
    slave_addr = OLED12864_SLAVE0;
    buff[0] = reg;
    buff[1] = data;

    oled_msg[0].addr  = slave_addr;
    oled_msg[0].flags = I2C_BUS_WR;
    oled_msg[0].buff  = buff;
    oled_msg[0].size  = 2;
    ret = i2c_bus_xfer(oled_i2c_dev, oled_msg, 1);
      
    return ret;
}

static void ssd1306_write_cmd(u8 cmd)
{
    oled12864_write_reg0(0x00, cmd);
}

void ssd1306_write_data(u8 data)
{
    oled12864_write_reg0(0x40, data);
}

//write oled12864 register
// static char oled12864_write_reg(u8 write_addr, u8 *write_buff, u8 write_size)
// {
//     struct i2c_dev_message oled_msg[2];
//     u8  slave_addr;
//     char ret;
      
//     slave_addr = OLED12864_SLAVE0;
//     oled_msg[0].addr = slave_addr;
//     oled_msg[0].flags = I2C_BUS_WR;
//     oled_msg[0].buff  = &write_addr;
//     oled_msg[0].size  = 1;
//     oled_msg[1].addr = slave_addr;
//     oled_msg[1].flags = I2C_BUS_WR | I2C_BUS_NO_START;
//     oled_msg[1].buff  = write_buff;
//     oled_msg[1].size  = write_size;
//     ret = i2c_bus_xfer(oled_i2c_dev, oled_msg, 2);
      
//     return ret;
// }

//static void ssd1306_Write_MultiData(uint8_t *data, uint16_t size)
//{
//    uint8_t tmp_array[SSD1306_WIDTH + 1] = {0};

//    memcpy(&tmp_array[0], data, size);

//    oled12864_write_reg(0x40, tmp_array, sizeof(tmp_array));
//}

//static char oled12864_read_reg(u8 read_addr, u8 *read_buff, u8 read_size)
//{
//    struct i2c_dev_message oled_msg[2];
//    u8 buff[1];
//    u8 slave_addr;
//    char ret;

//    slave_addr = OLED12864_SLAVE0;
//    buff[0] = read_addr;
//    oled_msg[0].addr = slave_addr;
//    oled_msg[0].flags = I2C_BUS_WR;
//    oled_msg[0].buff  = buff;
//    oled_msg[0].size  = 1;
//    oled_msg[1].addr = slave_addr;
//    oled_msg[1].flags = I2C_BUS_RD;
//    oled_msg[1].buff  = read_buff;
//    oled_msg[1].size  = read_size;
//    ret = i2c_bus_xfer(oled_i2c_dev, oled_msg, 2);

//    return ret;
//}

int8_t OLED12864_Init(struct i2c_dev_device *i2c_bus)
{
    if(NULL == i2c_bus)
    {
        return -1;
    }
    oled_i2c_dev = i2c_bus;

    ssd1306_write_cmd(0xAE);
    ssd1306_write_cmd(0x20);//set menory addressing mode

    ssd1306_write_cmd(0xB0);
    ssd1306_write_cmd(0xC8);

    ssd1306_write_cmd(0x00);
    ssd1306_write_cmd(0x10);

    ssd1306_write_cmd(0x40);
    ssd1306_write_cmd(0x81);
    ssd1306_write_cmd(0xff);

    ssd1306_write_cmd(0xA1);
    ssd1306_write_cmd(0xA6);
    ssd1306_write_cmd(0xA8);
    ssd1306_write_cmd(0x3F);

    ssd1306_write_cmd(0xA4);
    ssd1306_write_cmd(0xD3);
    ssd1306_write_cmd(0x00);
    ssd1306_write_cmd(0xD5);
    ssd1306_write_cmd(0xF0);

    ssd1306_write_cmd(0xD9);
    ssd1306_write_cmd(0x22);
    ssd1306_write_cmd(0xDA);
    ssd1306_write_cmd(0x12);

    ssd1306_write_cmd(0xDB);
    ssd1306_write_cmd(0x20);

    ssd1306_write_cmd(0x8D);
    ssd1306_write_cmd(0x14);
    ssd1306_write_cmd(0xAF);

    OLED_Fill_All(0xff);
    OLED_Fill_All(0x00);
    return 0;
}

void OLED_Fill_All(uint8_t fill_Data) 
{
    uint8_t i, j;
    for(i = 0; i < 8; i++)
    {
        ssd1306_write_cmd(0xb0 + i);//page0-page7
        ssd1306_write_cmd(0x00);//low column start address
        ssd1306_write_cmd(0x10);//high column start address
        for(j = 0; j < 128; j++)
        {
            ssd1306_write_data(fill_Data);
        }
    }
}

void ssd1306_on(void)
{
    ssd1306_write_cmd(0x8d);
    ssd1306_write_cmd(0x14);
    ssd1306_write_cmd(0xaf);
}

void ssd1306_off(void)
{
    ssd1306_write_cmd(0x8d);
    ssd1306_write_cmd(0x10);
    ssd1306_write_cmd(0xae);
}

void ssd1306_GotoXY(uint16_t x, uint16_t y) 
{
    /* Set write pointers */
    SSD1306.CurrentX = x;
    SSD1306.CurrentY = y;
}

void SSD1306_DrawPixel(uint16_t x, uint16_t y, SSD1306_COLOR_t color) 
{
    if(x >= SSD1306_WIDTH || y >= SSD1306_HEIGHT) 
    {
        /* Error */
        return;
    }

    /* Check if pixels are inverted */
    if (SSD1306.Inverted) 
    {
        color = (SSD1306_COLOR_t)!color;
    }

    /* Set color */
    if (color == SSD1306_COLOR_WHITE) 
    {
        ssd1306_buffer[x + (y / 8) * SSD1306_WIDTH] |= 1 << (y % 8);
    } 
    else 
    {
        ssd1306_buffer[x + (y / 8) * SSD1306_WIDTH] &= ~(1 << (y % 8));
    }
}

/**
  * @brief  OLED设置光标位置
  * @param  y 以左上角为原点，向下方向的坐标，范围：0~7
  * @param  x 以左上角为原点，向右方向的坐标，范围：0~127
  * @retval 无
  */
void oled1306_set_cursor(unsigned int y, unsigned int x)
{
    ssd1306_write_cmd(0xb0 | y);
    ssd1306_write_cmd(((x & 0xf0) >> 4) | 0x10);
    ssd1306_write_cmd((x & 0x0f) | 0x00);
}

void OLED_ShowChar(unsigned int line, unsigned int column, char Char)
{
	uint8_t i;
	oled1306_set_cursor((line - 1) * 2, (column - 1) * 8);
	for(i = 0; i < 8; i++)
	{
		ssd1306_write_data(OLED_F8x16[Char - ' '][i]);
	}
	oled1306_set_cursor((line - 1) * 2 + 1, (column - 1) * 8);
	for(i = 0; i < 8; i++)
	{
		ssd1306_write_data(OLED_F8x16[Char - ' '][i + 8]);
	}
}

/**
  * @brief  OLED显示字符串
  * @param  Line 起始行位置，范围：1~4
  * @param  Column 起始列位置，范围：1~16
  * @param  String 要显示的字符串，范围：ASCII可见字符
  * @retval 无
  */
void OLED_ShowString(uint8_t line, uint8_t column, char *String)
{
    uint8_t i;

//    OLED_Fill_All(0xff);
//    OLED_Fill_All(0x00);

    for(i = 0; String[i] != '\0'; i++)
    {
        OLED_ShowChar(line, column + i, String[i]);
    }
}

/**
  * @brief  OLED次方函数
  * @retval 返回值等于X的Y次方
  */
static uint32_t oled1306_pow(uint32_t x, uint32_t y)
{
    uint32_t res = 1;
    while(y--)
    {
        res *= x;
    }
    return res;
}

/**
  * @brief  OLED显示数字（十进制，正数）
  * @param  line 起始行位置，范围：1~4
  * @param  column 起始列位置，范围：1~16
  * @param  num 要显示的数字，范围：0~4294967295
  * @param  len 要显示数字的长度，范围：1~10
  * @retval 无
  */
void oled1306_show_num(uint8_t line, uint8_t column, uint32_t num, uint8_t len)
{
    uint8_t i;
    for(i = 0; i < len; i++)
    {
        OLED_ShowChar(line, (column + i), num / oled1306_pow(10, len - i - 1) % 10 + '0');
    }
}

void OLED_ShowCN(unsigned char x, unsigned char y, unsigned char N)
{
    unsigned int col = 0;
    for(int row = 0; row < 2; row++)
    {
        oled1306_set_cursor(x, y + row);
        for(col = 0; col < 16; col++)
        {
            ssd1306_write_data(F16x16[2 * N + row][col]);
        }
    }
}

void OLED_ShowEN(unsigned char x, unsigned char y)
{
    int i, j;
    for(i = 0; i < 9; i++)
    {
        oled1306_set_cursor(x, y);
        for(j = 0; j < 8; j++)
            ssd1306_write_data(F8x16[i][j]);
        oled1306_set_cursor(x, y + 1);
        for(j = 0; j < 8; j++)
            ssd1306_write_data(F8x16[i][j + 8]);
        x += 8;
    }
}

void OLED_ShowCN_14(unsigned char x, unsigned char y, unsigned char N)
{
    unsigned int  col = 0;
    for(int row = 0; row < 2; row++)
    {
        oled1306_set_cursor(x, y + row);
        for(col = 0; col < 14; col++)
        {
            ssd1306_write_data(F14x16[2 * N + row][col]);
        }
    }
}

void oled_display(void)
{
//	unsigned char i = 0;
    OLED_Fill_All(0xff);
    OLED_Fill_All(0x00);
//	for(i = 0; i < 4; i++)
//	{
//		OLED_ShowCN(8 + 2 * i * 16, 2, i);
//	}
//	OLED_ShowEN(0, 4);
//	for(i = 0; i < 4; i++)
//	{
//		OLED_ShowCN_14(72 + i * 14, 4, i);
//	}
}

void x_oled1306_DrawBMP(unsigned char x0, unsigned char y0, \
                        unsigned char x1, unsigned char y1, unsigned char BMP[])
{
    unsigned int j = 0;
    unsigned char x, y;

    if(y1 % 8 == 0) 
    {
        y = y1 / 8;
    }
    else
    {
        y = y1 / 8 + 1;
    }
    for(y = y0; y < y1; y++)
    {
        oled1306_set_cursor(y, x0);
        for(x = x0; x < x1; x++)
        {
            ssd1306_write_data(BMP[j++]);
        }
    }
} 


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

static void ssd1306_write_data(u8 data)
{
	oled12864_write_reg0(0x40, data);
}

//write oled12864 register
static char oled12864_write_reg(u8 write_addr, u8 *write_buff, u8 write_size)
{
	struct i2c_dev_message oled_msg[2];
	u8  slave_addr;
	char ret;
	  
	slave_addr = OLED12864_SLAVE0;
	oled_msg[0].addr = slave_addr;
	oled_msg[0].flags = I2C_BUS_WR;
	oled_msg[0].buff  = &write_addr;
	oled_msg[0].size  = 1;
	oled_msg[1].addr = slave_addr;
	oled_msg[1].flags = I2C_BUS_WR | I2C_BUS_NO_START;	//ע��˱�ʶ
	oled_msg[1].buff  = write_buff;
	oled_msg[1].size  = write_size;
	ret = i2c_bus_xfer(oled_i2c_dev, oled_msg, 2);
	  
	return ret;
}

static void ssd1306_Write_MultiData(uint8_t *data, uint16_t size)
{
	uint8_t tmp_array[SSD1306_WIDTH + 1] = {0};

    memcpy(&tmp_array[0], data, size);
	
    oled12864_write_reg(0x40, tmp_array, sizeof(tmp_array));
}

static char oled12864_read_reg(u8 read_addr, u8 *read_buff, u8 read_size)
{
	struct i2c_dev_message oled_msg[2];
	u8	buff[1];
	u8  slave_addr;
	char ret;
	  
	slave_addr = OLED12864_SLAVE0;
	buff[0] = read_addr;
	oled_msg[0].addr = slave_addr;
	oled_msg[0].flags = I2C_BUS_WR;
	oled_msg[0].buff  = buff;
	oled_msg[0].size  = 1;
	oled_msg[1].addr = slave_addr;
	oled_msg[1].flags = I2C_BUS_RD;
	oled_msg[1].buff  = read_buff;
	oled_msg[1].size  = read_size;
	ret = i2c_bus_xfer(oled_i2c_dev, oled_msg, 2);
	  
	return ret;
}

static void ssd1306_fill(SSD1306_COLOR_t color) 
{
    /* Set memory */
    memset(ssd1306_buffer, (color == SSD1306_COLOR_BLACK) ? 0x00 : 0xFF, sizeof(ssd1306_buffer));
}

static void ssd1306_UpdateScreen(void)
{
    uint8_t page;

    for (page = 0; page < 8; page++) 
    {
        ssd1306_write_cmd(0xB0 + page);
        ssd1306_write_cmd(0x00);
        ssd1306_write_cmd(0x10);

        /* Write multi data */
        ssd1306_Write_MultiData(&ssd1306_buffer[SSD1306_WIDTH * page], SSD1306_WIDTH);
    }	
}

int8_t OLED12864_Init(struct i2c_dev_device *i2c_bus)
{
	if(NULL == i2c_bus)
	{
		return -1;
	}
	oled_i2c_dev = i2c_bus;
	
	ssd1306_write_cmd(0xAE);//�ر���ʾ,������AF
	ssd1306_write_cmd(0x20);

	ssd1306_write_cmd(0xB0);
	ssd1306_write_cmd(0xC8);
	
	/*������ʼ�е�ַ*/
	ssd1306_write_cmd(0x00);//��4λ
	ssd1306_write_cmd(0x10);//��4λ
	
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
	
//	/* Clear screen */
//	ssd1306_fill(SSD1306_COLOR_BLACK);
//	
//	/* Update screen */
//    ssd1306_UpdateScreen();
//	
//	/* Set default values */
//	SSD1306.CurrentX = 0;
//	SSD1306.CurrentY = 0;
//	
//	SSD1306.Initialized = 1;
//	
	return 0;
}

/*-------------------���Data---------------------------------*/
void OLED_Fill_All(uint8_t fill_Data) 
{
	uint8_t i, j;
	for(i = 0; i < 8; i++)
	{
		ssd1306_write_cmd(0xb0 + i);			//page0-page7
		ssd1306_write_cmd(0x00);				//low column start address
		ssd1306_write_cmd(0x10);				//high column start address
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
    if (x >= SSD1306_WIDTH || y >= SSD1306_HEIGHT) 
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

/*
 * �������� : ����Ļ����ʾһ���ַ�
 * ����  ch    : Ҫ��ʾ���ַ�
 * ����  Font  : ����
 * ����  color : ��ɫ
 * ����ֵ : ��
 */
char SSD1306_Putc(char ch, TM_FontDef_t* Font, SSD1306_COLOR_t color) 
{
    uint32_t i, b, j;

    /* Check available space in LCD */
    if (SSD1306_WIDTH <= (SSD1306.CurrentX + Font->FontWidth) || \
        SSD1306_HEIGHT <= (SSD1306.CurrentY + Font->FontHeight)) 
    {
        /* Error */
        return 0;
    }

    /* Go through font */
    for (i = 0; i < Font->FontHeight; i++) 
    {
        b = Font->data[(ch - 32) * Font->FontHeight + i];
        for (j = 0; j < Font->FontWidth; j++) 
        {
            if ((b << j) & 0x8000) 
            {
                SSD1306_DrawPixel(SSD1306.CurrentX + j, (SSD1306.CurrentY + i), (SSD1306_COLOR_t) color);
            } 
            else 
            {
                SSD1306_DrawPixel(SSD1306.CurrentX + j, (SSD1306.CurrentY + i), (SSD1306_COLOR_t)!color);
            }
        }
    }

    /* Increase pointer */
    SSD1306.CurrentX += Font->FontWidth;

    /* Return character written */
    return ch;
}

char SSD1306_Puts(char* str, TM_FontDef_t* Font, SSD1306_COLOR_t color) 
{
    /* Write characters */
    while (*str) 
    {
        /* Write character by character */
        if (SSD1306_Putc(*str, Font, color) != *str)
		{
            /* Return error */
            return *str;
        }

        /* Increase string pointer */
        str++;
    }

    /* Everything OK, zero should be returned */
    return *str;
}

void OLED_SetPos(unsigned char x, unsigned char y) //������ʼ������
{
    ssd1306_write_cmd(0xb0 + y);//y����
    ssd1306_write_cmd(((x & 0xf0) >> 4) | 0x10);//x�����4λ
    ssd1306_write_cmd((x & 0x0f) | 0x01);//x�����4λ
}

/*��ʾһ��16*16�ĺ���
 * x:��ʾ����ʼ��
 * y����ʾ����ʼҳ
 * N����ʾ�ĺ�������ģ�����еı�ţ��������еĵڼ�������
 * */
void OLED_ShowCN(unsigned char x, unsigned char y, unsigned char N)
{
    unsigned int col = 0;
    for(int row = 0; row < 2; row++)
    {
        //һ����������ҳ��һҳ8�У�
        OLED_SetPos(x, y + row);//������ʼ�к���
        for(col = 0; col < 16; col++)//ÿ��д16�ֽڣ�һ�ֽ�һ�У�
        {
            ssd1306_write_data(F16x16[2 * N + row][col]);
        }
    }
}

/*��ʾ8*16��С��Ӣ��*/
void OLED_ShowEN(unsigned char x, unsigned char y)
{
    int i, j;
    for(i = 0; i < 9; i++)//9���ֽڣ�ÿ���ֽ�ռ2ҳ���߶���16bit��һҳ8bit��
    {
        OLED_SetPos(x, y);//���ù��
        for(j = 0; j < 8; j++)
            ssd1306_write_data(F8x16[i][j]);//��ʾ�ֽڵ��ϰ벿��
        OLED_SetPos(x, y + 1);//��괹ֱ�ƶ�1ҳ
        for(j = 0; j < 8; j++)
            ssd1306_write_data(F8x16[i][j + 8]);//��ʾ�ֽڵ��°벿��
        x += 8;//ˮƽ�ƶ���
    }
}

/*��ʾһ��14*16�ĺ���*/
void OLED_ShowCN_14(unsigned char x, unsigned char y, unsigned char N)
{
    unsigned int  col = 0;
    for(int row = 0; row < 2; row++)
    {
        //һ����������ҳ��һҳ8�У�
        OLED_SetPos(x, y + row);//������ʼ�к���
        for(col = 0; col < 14; col++)//ÿ��д16�ֽڣ�һ�ֽ�һ�У�
        {
            ssd1306_write_data(F14x16[2 * N + row][col]);
        }
    }
}

/*��ʾȫ����Ϣ*/
void oled_display(void)
{
	unsigned char i = 0;
	OLED_Fill_All(0xff);
	OLED_Fill_All(0x00);
    for(i = 0 ; i < 4; i++)
    {
        OLED_ShowCN(8 + 2 * i * 16, 2, i);//��ʾ16*16���ģ���ӭʹ��
    }
    OLED_ShowEN(0, 4);//��ʾӢ����ĸ��RT-Thread
    for(i = 0; i < 4; i++)
    {
        OLED_ShowCN_14(72 + i * 14, 4, i);//��ʾ14*16���ģ�����С��
    }	
}

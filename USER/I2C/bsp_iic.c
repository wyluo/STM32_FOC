#include "bsp_iic.h"
#include "bsp_delay.h"

void ANA_IIC_Delay(void)
{
	delay_us(SystemCoreClock, 2);
}

void ANA_IIC_GPIO_Init(void)
{
	GPIO_InitTypeDef GPIO_InitStructure = {0};
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);
	
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_OD;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_8;
	GPIO_Init(GPIOB, &GPIO_InitStructure);
	
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_9;
	GPIO_Init(GPIOB, &GPIO_InitStructure);
	
	GPIO_SetBits(GPIOB, GPIO_Pin_8 | GPIO_Pin_9);
}

/**
  * @brief  I2C start
  * @param  None
  * @retval None
  */
void ANA_I2C_Start(void)
{
	ANA_SDA_OUT();
	ANA_IIC_SCL = 1;
	ANA_IIC_SDA = 1;
	ANA_IIC_Delay();
	ANA_IIC_SDA = 0;
	ANA_IIC_Delay();
	ANA_IIC_SCL = 0;
}

/**
  * @brief  I2C stop
  * @param  None
  * @retval None
  */
void ANA_I2C_Stop(void)
{
	ANA_SDA_OUT();
	ANA_IIC_SCL = 0;
	ANA_IIC_SDA = 0;
	ANA_IIC_Delay();
	ANA_IIC_SCL = 1;
	ANA_IIC_SDA = 1;
}

u8 ANA_IIC_Wait_Ack(void)
{
	u8 ucErrTime=0;
	ANA_SDA_IN();      //SDA设置为输入
	ANA_IIC_SDA = 1;
	ANA_IIC_Delay();	   
	ANA_IIC_SCL = 1;
	ANA_IIC_Delay();	 
	while(ANA_READ_SDA)
	{
		ucErrTime++;
		if(ucErrTime > 250)
		{
			ANA_I2C_Stop();
			return 1;
		}
	}
	ANA_IIC_SCL = 0;//时钟输出0
	return 0;  
}

//产生ACK应答
void ANA_IIC_Ack(void)
{
	ANA_IIC_SCL = 0;
	ANA_SDA_OUT();
	ANA_IIC_SDA = 0;
	ANA_IIC_Delay();
	ANA_IIC_SCL = 1;
	ANA_IIC_Delay();
	ANA_IIC_SCL = 0;
}

//不产生ACK应答
void ANA_IIC_NAck(void)
{
	ANA_IIC_SCL = 0;
	ANA_SDA_OUT();
	ANA_IIC_SDA = 1;
	ANA_IIC_Delay();
	ANA_IIC_SCL = 1;
	ANA_IIC_Delay();
	ANA_IIC_SCL = 0;
}

//IIC发送一个字节
//返回从机有无应答
//1，有应答
//0，无应答
void ANA_IIC_Send_Byte(u8 txd)
{                        
    u8 t;   
	ANA_SDA_OUT(); 	    
    ANA_IIC_SCL = 0;//拉低时钟开始数据传输
    for(t = 0; t < 8; t++)
    {              
        ANA_IIC_SDA = (txd & 0x80)>>7;
        txd<<=1; 	  
		ANA_IIC_SCL = 1;
		ANA_IIC_Delay(); 
		ANA_IIC_SCL = 0;	
		ANA_IIC_Delay();
    }	 
}

//读1个字节，ack=1时，发送ACK，ack=0，发送nACK
u8 ANA_IIC_Read_Byte(unsigned char ack)
{
	unsigned char i, receive = 0;
	ANA_SDA_IN();//SDA设置为输入
    for(i = 0; i < 8; i++ )
	{
        ANA_IIC_SCL = 0; 
        ANA_IIC_Delay();
		ANA_IIC_SCL = 1;
        receive <<= 1;
        if(ANA_READ_SDA)
			receive++;
		ANA_IIC_Delay(); 
    }					 
    if(!ack)
        ANA_IIC_NAck();//发送nACK
    else
        ANA_IIC_Ack(); //发送ACK   
    return receive;
}


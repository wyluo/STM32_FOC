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
	ANA_SDA_IN();      //SDA����Ϊ����
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
	ANA_IIC_SCL = 0;//ʱ�����0
	return 0;  
}

//����ACKӦ��
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

//������ACKӦ��
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

//IIC����һ���ֽ�
//���شӻ�����Ӧ��
//1����Ӧ��
//0����Ӧ��
void ANA_IIC_Send_Byte(u8 txd)
{                        
    u8 t;   
	ANA_SDA_OUT(); 	    
    ANA_IIC_SCL = 0;//����ʱ�ӿ�ʼ���ݴ���
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

//��1���ֽڣ�ack=1ʱ������ACK��ack=0������nACK
u8 ANA_IIC_Read_Byte(unsigned char ack)
{
	unsigned char i, receive = 0;
	ANA_SDA_IN();//SDA����Ϊ����
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
        ANA_IIC_NAck();//����nACK
    else
        ANA_IIC_Ack(); //����ACK   
    return receive;
}


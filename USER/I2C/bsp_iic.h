#ifndef BSP_IIC_H
#define BSP_IIC_H

#include "stm32f10x.h"
#include "config.h"

#define ANA_SDA_IN()  		{GPIOB->CRH &= 0XFFFFFFF0;GPIOB->CRH |= (u32)8<<0;}
#define ANA_SDA_OUT() 		{GPIOB->CRH &= 0XFFFFFFF0;GPIOB->CRH |= (u32)3<<0;}

#define ANA_W_SCL(x)		GPIO_WriteBit(GPIOB, GPIO_Pin_9, (BitAction)(x))
#define ANA_W_SDA(x)		GPIO_WriteBit(GPIOB, GPIO_Pin_8, (BitAction)(x))

#define ANA_IIC_SCL    		PBout(9)
#define ANA_IIC_SDA    		PBout(8)
#define ANA_READ_SDA   		PBin(8)

void ANA_IIC_GPIO_Init(void);
void ANA_I2C_Start(void);
void ANA_I2C_Stop(void);
u8 ANA_IIC_Wait_Ack(void);
void ANA_IIC_Ack(void);
void ANA_IIC_NAck(void);
void ANA_IIC_Send_Byte(u8 txd);
u8 ANA_IIC_Read_Byte(unsigned char ack);

#endif


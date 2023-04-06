//#include "N76E003.h"
//#include "SFR_Macro.h"
//#include "Common.h"
//#include "Delay.h"
//#include "Function_define.h"
//#include "IQS_I2C.h"
#include "IQS7211E.h"
#include "IQS7211E_init.h"
#include "bsp_iic.h"
#include "bsp_uart.h"
#include "bsp_delay.h"
//#include "IQS7211E_init_AZP1263A1.h"

static void IQS7211E_RDY_Init(void)
{
	GPIO_InitTypeDef GPIO_InitStructure = {0};
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);
	
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOB, &GPIO_InitStructure);
}

void IQS7211E_Extix_Init(void)
{
	EXTI_InitTypeDef EXTI_InitStructure = {0};
	NVIC_InitTypeDef NVIC_InitStructure = {0};
	
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO, ENABLE);
	
	IQS7211E_RDY_Init();
	
	GPIO_EXTILineConfig(GPIO_PortSourceGPIOA, GPIO_PinSource0);
	EXTI_InitStructure.EXTI_Line = EXTI_Line0;
  	EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;	
  	EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Falling;
  	EXTI_InitStructure.EXTI_LineCmd = ENABLE;
  	EXTI_Init(&EXTI_InitStructure);
	
	NVIC_InitStructure.NVIC_IRQChannel = EXTI0_IRQn;			//使能按键所在的外部中断通道
  	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0x02;	//抢占优先级2 
  	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0x02;					//子优先级1
  	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;								//使能外部中断通道
  	NVIC_Init(&NVIC_InitStructure);
}

void EXTI0_IRQHandler(void)
{
	delay_ms(SystemCoreClock, 10);//消抖
	if(GPIO_ReadInputDataBit(GPIOA, GPIO_Pin_0) == 0)
	{	  
		Process_IQS7211E_Events();	
	}
	EXTI_ClearITPendingBit(EXTI_Line0);  //清除EXTI0线路挂起位
}

//IIC写一个字节 
//reg:寄存器地址
//data:数据
//返回值:0,正常
//    其他,错误代码
u8 IQS7211E_Write_Byte(u32 addr, u8 reg, u8 data) 				 
{ 
    ANA_I2C_Start(); 
	ANA_IIC_Send_Byte((IQS7211E_ADDR) | 0);//发送器件地址+写命令
	if(ANA_IIC_Wait_Ack())	//等待应答
	{
		ANA_I2C_Stop();
		return 1;		
	}
    ANA_IIC_Send_Byte(reg);	//写寄存器地址
    ANA_IIC_Wait_Ack();		//等待应答 
	ANA_IIC_Send_Byte(data);//发送数据
	if(ANA_IIC_Wait_Ack())	//等待ACK
	{
		ANA_I2C_Stop();	 
		return 1;		 
	}		 
    ANA_I2C_Stop();
	return 0;
}

//IIC连续写
//addr:器件地址 
//reg:寄存器地址
//len:写入长度
//buf:数据区
//返回值:0,正常
//    其他,错误代码
uint8_t IQS7211E_Write_Len(uint8_t addr, uint8_t reg, uint8_t *buf, uint8_t len)
{
	uint8_t i; 
    ANA_I2C_Start(); 
	ANA_IIC_Send_Byte((addr) | 0);//发送器件地址+写命令	
	if(ANA_IIC_Wait_Ack())	//等待应答
	{
		ANA_I2C_Stop();		 
		return 1;		
	}
    ANA_IIC_Send_Byte(reg);	//写寄存器地址
    ANA_IIC_Wait_Ack();		//等待应答
	for(i = 0; i < len; i++)
	{
		ANA_IIC_Send_Byte(buf[i]);	//发送数据
		if(ANA_IIC_Wait_Ack())		//等待ACK
		{
			ANA_I2C_Stop();	 
			return 1;		 
		}		
	}    
    ANA_I2C_Stop();	 
	return 0;	
}

//IIC读一个字节 
//reg:寄存器地址 
//返回值:读到的数据
u8 IQS7211E_Read_Byte(u8 reg)
{
	u8 res;
    ANA_I2C_Start(); 
	ANA_IIC_Send_Byte((IQS7211E_ADDR) | 0);//发送器件地址+写命令	
	ANA_IIC_Wait_Ack();		//等待应答 
    ANA_IIC_Send_Byte(reg);	//写寄存器地址
    ANA_IIC_Wait_Ack();		//等待应答
    ANA_I2C_Start();
	ANA_IIC_Send_Byte((IQS7211E_ADDR) | 1);//发送器件地址+读命令	
    ANA_IIC_Wait_Ack();		//等待应答 
	res = ANA_IIC_Read_Byte(0);//读取数据,发送nACK 
    ANA_I2C_Stop();			//产生一个停止条件 
	return res;		
}

//IIC连续读
//addr:器件地址
//reg:要读取的寄存器地址
//len:要读取的长度
//buf:读取到的数据存储区
//返回值:0,正常
//    其他,错误代码
uint8_t IQS7211E_Read_Len(uint8_t addr, uint8_t reg, uint8_t *buf, uint8_t len)
{ 
 	ANA_I2C_Start(); 
	ANA_IIC_Send_Byte((addr) | 0);//发送器件地址+写命令
	if(ANA_IIC_Wait_Ack())	//等待应答
	{
		ANA_I2C_Stop();		 
		return 1;		
	}
    ANA_IIC_Send_Byte(reg);	//写寄存器地址
    ANA_IIC_Wait_Ack();		//等待应答
    ANA_I2C_Start();
	ANA_IIC_Send_Byte((addr) | 1);//发送器件地址+读命令
    ANA_IIC_Wait_Ack();		//等待应答 
	while(len)
	{
		if(len == 1)
			*buf = IQS7211E_Read_Byte(0);//读数据,发送nACK 
		else 
			*buf = IQS7211E_Read_Byte(1);//读数据,发送ACK  
		len--;
		buf++;
	}    
    ANA_I2C_Stop();	//产生一个停止条件 
	return 0;	
}

void IQS7211E_Init(void)
{
	uint8_t buffer[40];
//	int i = 0;

	/* Change the ALP ATI Compensation */
	/* Memory Map Position 0x1F - 0x20 */
	buffer[0] = ALP_COMPENSATION_A_0;
	buffer[1] = ALP_COMPENSATION_A_1;
	buffer[2] = ALP_COMPENSATION_B_0;
	buffer[3] = ALP_COMPENSATION_B_1;

//	Write_Data(IQS7211E_ADDR, 0x1F, &buffer[0], 4, STOP_TRUE);
//	for(i = 0; i < 4; i++)
//	{
//		IQS7211E_Write_Byte(IQS7211E_ADDR, 0x1F, buffer[i]);
//	}
	IQS7211E_Write_Len(IQS7211E_ADDR, 0x1F, &buffer[0], 4);
		
    /* Change the ATI Settings */
/* Memory Map Position 0x21 - 0x27 */
    buffer[0] = TP_ATI_MULTIPLIERS_DIVIDERS_0;
    buffer[1] = TP_ATI_MULTIPLIERS_DIVIDERS_1;
    buffer[2] = TP_COMPENSATION_DIV;
    buffer[3] = TP_REF_DRIFT_LIMIT;
	buffer[4] = TP_ATI_TARGET_0;
    buffer[5] = TP_ATI_TARGET_1;
    buffer[6] = TP_MIN_COUNT_REATI_0;
    buffer[7] = TP_MIN_COUNT_REATI_1;
    buffer[8] = ALP_ATI_MULTIPLIERS_DIVIDERS_0;
    buffer[9] = ALP_ATI_MULTIPLIERS_DIVIDERS_1;
    buffer[10] = ALP_COMPENSATION_DIV;
    buffer[11] = ALP_LTA_DRIFT_LIMIT;
    buffer[12] = ALP_ATI_TARGET_0;
    buffer[13] = ALP_ATI_TARGET_1;

//    Write_Data(IQS7211E_ADDR,0x21,&buffer[0],14,STOP_TRUE);
//	for(i = 0; i < 14; i++)
//	{
//		IQS7211E_Write_Byte(IQS7211E_ADDR, 0x21, buffer[i]);
//	}
	IQS7211E_Write_Len(IQS7211E_ADDR, 0x21, &buffer[0], 14);
		
    /* Change the Report Rates and Timing */
/* Memory Map Position 0x28 - 0x32 */
    buffer[0] = ACTIVE_MODE_REPORT_RATE_0;
    buffer[1] = ACTIVE_MODE_REPORT_RATE_1;
    buffer[2] = IDLE_TOUCH_MODE_REPORT_RATE_0;
    buffer[3] = IDLE_TOUCH_MODE_REPORT_RATE_1;
    buffer[4] = IDLE_MODE_REPORT_RATE_0;
    buffer[5] = IDLE_MODE_REPORT_RATE_1;
    buffer[6] = LP1_MODE_REPORT_RATE_0;
    buffer[7] = LP1_MODE_REPORT_RATE_1;
    buffer[8] = LP2_MODE_REPORT_RATE_0;
    buffer[9] = LP2_MODE_REPORT_RATE_1;
    buffer[10] = ACTIVE_MODE_TIMEOUT_0;
    buffer[11] = ACTIVE_MODE_TIMEOUT_1;
    buffer[12] = IDLE_TOUCH_MODE_TIMEOUT_0;
    buffer[13] = IDLE_TOUCH_MODE_TIMEOUT_1;
    buffer[14] = IDLE_MODE_TIMEOUT_0;
    buffer[15] = IDLE_MODE_TIMEOUT_1;
    buffer[16] = LP1_MODE_TIMEOUT_0;
    buffer[17] = LP1_MODE_TIMEOUT_1;
    buffer[18] = REATI_RETRY_TIME;
    buffer[19] = REF_UPDATE_TIME;
    buffer[20] = I2C_TIMEOUT_0;
    buffer[21] = I2C_TIMEOUT_1;
    
//	Write_Data(IQS7211E_ADDR, 0x28, &buffer[0], 22, STOP_TRUE);
//	for(i = 0; i < 22; i++)
//	{
//		IQS7211E_Write_Byte(IQS7211E_ADDR, 0x28, buffer[i]);
//	}
	IQS7211E_Write_Len(IQS7211E_ADDR, 0x28, &buffer[0], 22);

	/* Change the ALP Settings */
	/* Memory Map Position 0x36 - 0x40 */
	buffer[0] = ALP_SETUP_0;
	buffer[1] = ALP_SETUP_1;
	buffer[2] = ALP_TX_ENABLE_0;
	buffer[3] = ALP_TX_ENABLE_1;
	buffer[4] = TRACKPAD_TOUCH_SET_THRESHOLD;
	buffer[5] = TRACKPAD_TOUCH_CLEAR_THRESHOLD;
	buffer[6] = ALP_THRESHOLD_0;
	buffer[7] = ALP_THRESHOLD_1;
	buffer[8] = ALP_SET_DEBOUNCE;
	buffer[9] = ALP_CLEAR_DEBOUNCE;
	buffer[10] = ALP_COUNT_BETA_LP1;
	buffer[11] = ALP_LTA_BETA_LP1;
	buffer[12] = ALP_COUNT_BETA_LP2;
	buffer[13] = ALP_LTA_BETA_LP2;
	buffer[14] = TP_CONVERSION_FREQUENCY_UP_PASS_LENGTH;
	buffer[15] = TP_CONVERSION_FREQUENCY_FRACTION_VALUE;
	buffer[16] = ALP_CONVERSION_FREQUENCY_UP_PASS_LENGTH;
	buffer[17] = ALP_CONVERSION_FREQUENCY_FRACTION_VALUE;
	buffer[18] = TRACKPAD_HARDWARE_SETTINGS_0;
	buffer[19] = TRACKPAD_HARDWARE_SETTINGS_1;
	buffer[20] = ALP_HARDWARE_SETTINGS_0;
	buffer[21] = ALP_HARDWARE_SETTINGS_1;
		
//	Write_Data(IQS7211E_ADDR, 0x36, &buffer[0], 22, STOP_TRUE);
//	for(i = 0; i < 22; i++)
//	{
//		IQS7211E_Write_Byte(IQS7211E_ADDR, 0x36, buffer[i]);
//	}
	IQS7211E_Write_Len(IQS7211E_ADDR, 0x36, &buffer[0], 22);
			
	/* Change the Trackpad Settings */
/* Memory Map Position 0x41 - 0x4A */
	buffer[0] = TRACKPAD_SETTINGS_0_0;
	buffer[1] = TRACKPAD_SETTINGS_0_1;
	buffer[2] = TRACKPAD_SETTINGS_1_0;
	buffer[3] = TRACKPAD_SETTINGS_1_1;
	buffer[4] = X_RESOLUTION_0;
	buffer[5] = X_RESOLUTION_1;
	buffer[6] = Y_RESOLUTION_0;
	buffer[7] = Y_RESOLUTION_1;
	buffer[8] = XY_DYNAMIC_FILTER_BOTTOM_SPEED_0;
	buffer[9] = XY_DYNAMIC_FILTER_BOTTOM_SPEED_1;
	buffer[10] = XY_DYNAMIC_FILTER_TOP_SPEED_0;
	buffer[11] = XY_DYNAMIC_FILTER_TOP_SPEED_1;
	buffer[12] = XY_DYNAMIC_FILTER_BOTTOM_BETA;
	buffer[13] = XY_DYNAMIC_FILTER_STATIC_FILTER_BETA;
	buffer[14] = STATIONARY_TOUCH_MOV_THRESHOLD;
	buffer[15] = FINGER_SPLIT_FACTOR;
	buffer[16] = X_TRIM_VALUE;
	buffer[17] = Y_TRIM_VALUE;
	buffer[18] = MINOR_VERSION;
	buffer[19] = MAJOR_VERSION;
	
//	Write_Data(IQS7211E_ADDR,0x41,&buffer[0],20,STOP_TRUE);	
//	for(i = 0; i < 20; i++)
//	{
//		IQS7211E_Write_Byte(IQS7211E_ADDR, 0x41, buffer[i]);
//	}
	IQS7211E_Write_Len(IQS7211E_ADDR, 0x41, &buffer[0], 20);

	/* Change the Gesture Settings */
/* Memory Map Position 0x4B - 0x55 */
	buffer[0] = GESTURE_ENABLE_0;
	buffer[1] = GESTURE_ENABLE_1;
	buffer[2] = TAP_TOUCH_TIME_0;
	buffer[3] = TAP_TOUCH_TIME_1;
	buffer[4] = TAP_WAIT_TIME_0;
	buffer[5] = TAP_WAIT_TIME_1;
	buffer[6] = TAP_DISTANCE_0;
	buffer[7] = TAP_DISTANCE_1;
	buffer[8] = HOLD_TIME_0;
	buffer[9] = HOLD_TIME_1;
	buffer[10] = SWIPE_TIME_0;
	buffer[11] = SWIPE_TIME_1;
	buffer[12] = SWIPE_X_DISTANCE_0;
	buffer[13] = SWIPE_X_DISTANCE_1;
	buffer[14] = SWIPE_Y_DISTANCE_0;
	buffer[15] = SWIPE_Y_DISTANCE_1;
	buffer[16] = SWIPE_X_CONS_DIST_0;
	buffer[17] = SWIPE_X_CONS_DIST_1;
	buffer[18] = SWIPE_Y_CONS_DIST_0;
	buffer[19] = SWIPE_Y_CONS_DIST_1;
	buffer[20] = SWIPE_ANGLE;
	buffer[21] = PALM_THRESHOLD;
	
//	Write_Data(IQS7211E_ADDR,0x4B,&buffer[0],22,STOP_TRUE);
//	for(i = 0; i < 22; i++)
//	{
//		IQS7211E_Write_Byte(IQS7211E_ADDR, 0x4B, buffer[i]);
//	}
	IQS7211E_Write_Len(IQS7211E_ADDR, 0x4B, &buffer[0], 22);

	/* Change the RxTx Mapping */
/* Memory Map Position 0x56 - 0x5C */
	buffer[0] = RX_TX_MAP_0;
	buffer[1] = RX_TX_MAP_1;
	buffer[2] = RX_TX_MAP_2;
	buffer[3] = RX_TX_MAP_3;
	buffer[4] = RX_TX_MAP_4;
	buffer[5] = RX_TX_MAP_5;
	buffer[6] = RX_TX_MAP_6;
	buffer[7] = RX_TX_MAP_7;
	buffer[8] = RX_TX_MAP_8;
	buffer[9] = RX_TX_MAP_9;
	buffer[10] = RX_TX_MAP_10;
	buffer[11] = RX_TX_MAP_11;
	buffer[12] = RX_TX_MAP_12;
	buffer[13] = RX_TX_MAP_FILLER;
	
//	Write_Data(IQS7211E_ADDR,0x56,&buffer[0],14,STOP_TRUE);
//	for(i = 0; i < 14; i++)
//	{
//		IQS7211E_Write_Byte(IQS7211E_ADDR, 0x56, buffer[i]);
//	}
	IQS7211E_Write_Len(IQS7211E_ADDR, 0x56, &buffer[0], 14);
		
	/* Change the Allocation of channels into cycles 0-9 */
/* Memory Map Position 0x5D - 0x6B */
	buffer[0] = PLACEHOLDER_0;
	buffer[1] = CH_1_CYCLE_0;
	buffer[2] = CH_2_CYCLE_0;
	buffer[3] = PLACEHOLDER_1;
	buffer[4] = CH_1_CYCLE_1;
	buffer[5] = CH_2_CYCLE_1;
	buffer[6] = PLACEHOLDER_2;
	buffer[7] = CH_1_CYCLE_2;
	buffer[8] = CH_2_CYCLE_2;
	buffer[9] = PLACEHOLDER_3;
	buffer[10] = CH_1_CYCLE_3;
	buffer[11] = CH_2_CYCLE_3;
	buffer[12] = PLACEHOLDER_4;
	buffer[13] = CH_1_CYCLE_4;
	buffer[14] = CH_2_CYCLE_4;
	buffer[15] = PLACEHOLDER_5;
	buffer[16] = CH_1_CYCLE_5;
	buffer[17] = CH_2_CYCLE_5;
	buffer[18] = PLACEHOLDER_6;
	buffer[19] = CH_1_CYCLE_6;
	buffer[20] = CH_2_CYCLE_6;
	buffer[21] = PLACEHOLDER_7;
	buffer[22] = CH_1_CYCLE_7;
	buffer[23] = CH_2_CYCLE_7;
	buffer[24] = PLACEHOLDER_8;
	buffer[25] = CH_1_CYCLE_8;
	buffer[26] = CH_2_CYCLE_8;
	buffer[27] = PLACEHOLDER_9;
	buffer[28] = CH_1_CYCLE_9;
	buffer[29] = CH_2_CYCLE_9;		
	
//	Write_Data(IQS7211E_ADDR,0x5D,&buffer[0],30,STOP_TRUE);
//	for(i = 0; i < 30; i++)
//	{
//		IQS7211E_Write_Byte(IQS7211E_ADDR, 0x5D, buffer[i]);
//	}
	IQS7211E_Write_Len(IQS7211E_ADDR, 0x5D, &buffer[0], 30);
		
	/* Change the Allocation of channels into cycles 10-19 */
/* Memory Map Position 0x6C - 0x7C */
	buffer[0] = PLACEHOLDER_10;
	buffer[1] = CH_1_CYCLE_10;
	buffer[2] = CH_2_CYCLE_10;
	buffer[3] = PLACEHOLDER_11;
	buffer[4] = CH_1_CYCLE_11;
	buffer[5] = CH_2_CYCLE_11;
	buffer[6] = PLACEHOLDER_12;
	buffer[7] = CH_1_CYCLE_12;
	buffer[8] = CH_2_CYCLE_12;
	buffer[9] = PLACEHOLDER_13;
	buffer[10] = CH_1_CYCLE_13;
	buffer[11] = CH_2_CYCLE_13;
	buffer[12] = PLACEHOLDER_14;
	buffer[13] = CH_1_CYCLE_14;
	buffer[14] = CH_2_CYCLE_14;
	buffer[15] = PLACEHOLDER_15;
	buffer[16] = CH_1_CYCLE_15;
	buffer[17] = CH_2_CYCLE_15;
	buffer[18] = PLACEHOLDER_16;
	buffer[19] = CH_1_CYCLE_16;
	buffer[20] = CH_2_CYCLE_16;
	buffer[21] = PLACEHOLDER_17;
	buffer[22] = CH_1_CYCLE_17;
	buffer[23] = CH_2_CYCLE_17;
	buffer[24] = PLACEHOLDER_18;
	buffer[25] = CH_1_CYCLE_18;
	buffer[26] = CH_2_CYCLE_18;
	buffer[27] = PLACEHOLDER_19;
	buffer[28] = CH_1_CYCLE_19;
	buffer[29] = CH_2_CYCLE_19;
	buffer[30] = PLACEHOLDER_20;
	buffer[31] = CH_1_CYCLE_20;
	buffer[32] = CH_2_CYCLE_20;

//	Write_Data(IQS7211E_ADDR,0x6C,&buffer[0],33,STOP_TRUE);
//	for(i = 0; i < 33; i++)
//	{
//		IQS7211E_Write_Byte(IQS7211E_ADDR, 0x6C, buffer[i]);
//	}
	IQS7211E_Write_Len(IQS7211E_ADDR, 0x6C, &buffer[0], 33);
	
	/* Change the System Settings */
/* Memory Map Position 0x33 - 0x35 */
	buffer[0] = SYSTEM_CONTROL_0 | 0x80 | 0x20;  // Ack Reset, TP Re-ATI
	buffer[1] = SYSTEM_CONTROL_1;
	buffer[2] = CONFIG_SETTINGS0 | 0x40;  //bit6 : 1 Write one or two bytes (any data) to the address 0xFF followed by a STOP to end comms
	buffer[3] = CONFIG_SETTINGS1 | 0x01;  //0x01 : Enable Event Mode
	buffer[4] = OTHER_SETTINGS_0;
	buffer[5] = OTHER_SETTINGS_1;
	
//	Write_Data(IQS7211E_ADDR,0x33,&buffer[0],6,STOP_TRUE);
//   	for(i = 0; i < 6; i++)
//	{
//		IQS7211E_Write_Byte(IQS7211E_ADDR, 0x33, buffer[i]);
//	}
	IQS7211E_Write_Len(IQS7211E_ADDR, 0x33, &buffer[0], 6);
}

/*
//stop信号结束通信功能关闭
*/
void IQS7211E_Stop_Bit_Disabled(void)
{
	uint8_t buffer[2];

	buffer[0] = CONFIG_SETTINGS0 | 0x40;  //bit6 : 1 Write one or two bytes (any data) to the address 0xFF followed by a STOP to end comms;
	//Write_Data(IQS7211E_ADDR,0x34,&buffer[0],1,STOP_TRUE);
	IQS7211E_Write_Byte(IQS7211E_ADDR, 0x34, buffer[0]);
}

/*
//关闭通信窗口
*/
void IQS7211E_Stop_I2C_Comm_Window(void)
{
	uint8_t buffer[2];

	buffer[0] = CONFIG_SETTINGS0 & 0xBF;  //bit6 : 1 Write one or two bytes (any data) to the address 0xFF followed by a STOP to end comms;
	//Write_Data(IQS7211E_ADDR,0x34,&buffer[0],1,STOP_TRUE);
	IQS7211E_Write_Byte(IQS7211E_ADDR, 0x34, buffer[0]);	
}

void Process_IQS7211E_Events(void)
{
	uint8_t Version_Details_buffer[20];
	uint8_t System_Data_buffer[28];
//	uint8_t Channel_Touch_Status[6];
//	uint8_t PCC_Values[64];
//	uint8_t buffer[2] = {0};
	uint16_t Product_Number = 0;
	uint16_t Major_Version = 0;
	uint16_t Minor_Version = 0;	

	static uint8_t Finger_Amount = 0;
	uint16_t Finger_1_X_coordinate = 0;
	uint16_t Finger_1_Y_coordinate = 0;	
//	uint16_t Finger_2_X_coordinate = 0;
//	uint16_t Finger_2_Y_coordinate = 0;
	  
	//RDY GPIOA_PIN0
	if(GPIO_ReadInputDataBit(GPIOA, GPIO_Pin_0) == 0)
	{
		//printf("\n RDY occurred");
		IQS7211E_Stop_Bit_Disabled();

		//Read_Data(IQS7211E_ADDR,0x00,&Version_Details_buffer[0],6,STOP_TRUE);
		IQS7211E_Read_Len(IQS7211E_ADDR, 0x00, &Version_Details_buffer[0], 6);	
		//Read_Data(IQS7211E_ADDR,0x0A,&System_Data_buffer[0],20,STOP_TRUE);
		IQS7211E_Read_Len(IQS7211E_ADDR, 0x0A, &System_Data_buffer[0], 20);
		//Read_Data(IQS7211E_ADDR,0x20,&Channel_Touch_Status[0],4,STOP_FAULT);
			  		  
		//0x0F Register Info Flags
		if(System_Data_buffer[10] & 0x80)
		{//Reset occurred
			printf("\n IQS7211E_Reset_occurred");
			IQS7211E_Init();
			printf("\n IQS7211E_Init_finished");

			Product_Number = ((uint16_t)Version_Details_buffer[1] << 8) + \
								Version_Details_buffer[0];
			Major_Version = ((uint16_t)Version_Details_buffer[3] << 8) + \
								Version_Details_buffer[2];
			Minor_Version = ((uint16_t)Version_Details_buffer[5] << 8) + \
								Version_Details_buffer[4];
			printf("\n Product_Number =%d, \
						Major_Version =%d, \
						Minor_Version =%d", \
					Product_Number, Major_Version, Minor_Version);
		}
		else
		{
			//Read_Data(IQS7211E_ADDR,0xE3,&PCC_Values[0],64,STOP_FAULT);										 				  
			if(System_Data_buffer[10] & 0x20)
			{//ALP ATI Error
				printf("\n IQS7211E_ALP_ATI_Error");	  
			}
					
			if(System_Data_buffer[10] & 0x08)
			{//ATI Error
				printf("\n IQS7211E_ATI_Error");	  
			}
			else if(System_Data_buffer[10] & 0x10)
			{//ATI active
				printf("\n IQS7211E_ATI_active");	  
			}
			else
			{
				Finger_Amount = System_Data_buffer[11] & 0x03;
				Finger_1_X_coordinate = ((uint16_t)System_Data_buffer[13] << 8) + \
											System_Data_buffer[12];                 
				Finger_1_Y_coordinate = ((uint16_t)System_Data_buffer[15]<<8) + \
											System_Data_buffer[14];
				printf("\n X_coordinate>> %d", Finger_1_X_coordinate);
				printf("\n Y_coordinate>> %d", Finger_1_Y_coordinate);
				//Single Tap
				if(System_Data_buffer[8]&0x01)
				{
					printf("\n Single Tap");	
				}
									
				//Double Tap
				if(System_Data_buffer[8]&0x02)
				{
					printf("\n Double Tap");	
				}
									
				//Triple Tap
				if(System_Data_buffer[8]&0x04)
				{
					printf("\n Triple Tap");	
				}

				// Press and Hold
				if(System_Data_buffer[8]&0x08)
				{
					printf("\n Press and Hold");	
				}
									
				//Palm Gesture
				if(System_Data_buffer[8]&0x10)
				{
					printf("\n Palm Gesture");	
				}

				//Swipe X+
				if(System_Data_buffer[9]&0x01)
				{
					printf("\n Swipe X+");	
				}
									
				//Swipe X-
				if(System_Data_buffer[9]&0x02)
				{
					printf("\n Swipe X-");	
				}
								
				//Swipe Y+
				if(System_Data_buffer[9]&0x04)
				{
					printf("\n Swipe Y+");	
				}
								
				//Swipe Y-
				if(System_Data_buffer[9]&0x08)
				{
					printf("\n Swipe Y-");	
				}
									
				//Swipe and Hold X+
				if(System_Data_buffer[9]&0x10)
				{
					printf("\n Swipe and Hold X+");	
				}
			
				//Swipe and Hold X-
				if(System_Data_buffer[9]&0x20)
				{
					printf("\n Swipe and Hold X-");	
				}
									
				//Swipe and Hold Y+
				if(System_Data_buffer[9]&0x40)
				{
					printf("\n Swipe and Hold Y+");	
				}
			
				//Swipe and Hold Y-
				if(System_Data_buffer[9]&0x80)
				{
					printf("\n Swipe and Hold Y-");	
				}
			}
		}			
		//buffer[0] = 0;
		//Write_Data(IQS7211E_ADDR,0xFF,&buffer[0],1,STOP_TRUE);
        IQS7211E_Stop_I2C_Comm_Window();
				
        printf("\n END RDY Comms");				
	}
}







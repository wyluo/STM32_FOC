//#include "N76E003.h"
//#include "SFR_Macro.h"
//#include "Common.h"
//#include "Delay.h"
//#include "Function_define.h"
//#include "IQS_I2C.h"
#include "IQS7211E.h"
#include "IQS7211E_init.h"
#include "i2c_hw.h"
#include "elog.h"
#include "config.h"

//#include "bsp_uart.h"
//#include "bsp_delay.h"
//#include "IQS7211E_init_AZP1263A1.h"

#if IQS7211E_ENABLE

struct i2c_dev_device *iqs7211e_i2c_dev = NULL;
extern struct i2c_dev_device i2c0;

//write lp55231 register
static char iqs7211e_write_reg0(u8 write_addr, u8 write_data)
{
	struct i2c_dev_message iqs7211e_msg[1];
	u8 buff[2];
	u8 slave_addr;
	char ret;

	slave_addr = IQS7211E_ADDR;
	buff[0] = write_addr;
	buff[1] = write_data;
	iqs7211e_msg[0].addr  = slave_addr;
	iqs7211e_msg[0].flags = I2C_BUS_WR;
	iqs7211e_msg[0].buff  = buff;
	iqs7211e_msg[0].size  = 2;
	ret = i2c_bus_xfer(iqs7211e_i2c_dev, iqs7211e_msg, 1);

	return ret;
}

//write lp55231 register
static char iqs7211e_write_reg(u8 s_addr, u8 write_addr, u8 *write_buff, u8 write_size, u8 none)
{
	struct i2c_dev_message iqs7211e_msg[2];
	u8 slave_addr = 0;
	char ret;
	
	UNUSED(none);
	  
	slave_addr = s_addr;
	iqs7211e_msg[0].addr = slave_addr;
	iqs7211e_msg[0].flags = I2C_BUS_WR;
	iqs7211e_msg[0].buff  = &write_addr;
	iqs7211e_msg[0].size  = 1;
	iqs7211e_msg[1].addr = slave_addr;
	iqs7211e_msg[1].flags = I2C_BUS_WR | I2C_BUS_NO_START;
	iqs7211e_msg[1].buff  = write_buff;
	iqs7211e_msg[1].size  = write_size;
	ret = i2c_bus_xfer(iqs7211e_i2c_dev, iqs7211e_msg, 2);
	  
	return ret;
}

//read iqs7211e register
static char iqs7211e_read_reg(u8 s_addr, u8 read_addr, u8 *read_buff, u8 read_size, u8 none)
{
	struct i2c_dev_message iqs7211e_msg[2];
	u8 buff[1];
	u8 slave_addr = 0;
	char ret;
	
	UNUSED(none);
	  
	slave_addr = s_addr;
	buff[0] = read_addr;
	iqs7211e_msg[0].addr = slave_addr;
	iqs7211e_msg[0].flags = I2C_BUS_WR;
	iqs7211e_msg[0].buff  = buff;
	iqs7211e_msg[0].size  = 1;
	iqs7211e_msg[1].addr = slave_addr;
	iqs7211e_msg[1].flags = I2C_BUS_RD;
	iqs7211e_msg[1].buff  = read_buff;
	iqs7211e_msg[1].size  = read_size;
	ret = i2c_bus_xfer(iqs7211e_i2c_dev, iqs7211e_msg, 2);
	  
	return ret;
}

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
	
	GPIO_EXTILineConfig(GPIO_PortSourceGPIOB, GPIO_PinSource0);
	EXTI_InitStructure.EXTI_Line = EXTI_Line0;
  	EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;	
  	EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Falling;
  	EXTI_InitStructure.EXTI_LineCmd = ENABLE;
  	EXTI_Init(&EXTI_InitStructure);
	
	NVIC_InitStructure.NVIC_IRQChannel = EXTI0_IRQn;
  	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0x02;
  	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0x02;
  	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
  	NVIC_Init(&NVIC_InitStructure);
}

void EXTI0_IRQHandler(void)
{
	if(GPIO_ReadInputDataBit(GPIOB, GPIO_Pin_0) == 0)
	{	  
		Process_IQS7211E_Events();	
	}
	EXTI_ClearITPendingBit(EXTI_Line0);
}

int8_t IQS7211E_Init(struct i2c_dev_device *i2c_bus)
{
	if(NULL == i2c_bus)
	{
		return -1;
	}
	iqs7211e_i2c_dev = i2c_bus;	
	
	uint8_t buffer[40];

	/* Change the ALP ATI Compensation */
	/* Memory Map Position 0x1F - 0x20 */
	buffer[0] = ALP_COMPENSATION_A_0;
	buffer[1] = ALP_COMPENSATION_A_1;
	buffer[2] = ALP_COMPENSATION_B_0;
	buffer[3] = ALP_COMPENSATION_B_1;

	iqs7211e_write_reg(IQS7211E_ADDR, 0x1F, &buffer[0], 4, STOP_TRUE);
		
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

    iqs7211e_write_reg(IQS7211E_ADDR, 0x21, &buffer[0], 14, STOP_TRUE);

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
    
	iqs7211e_write_reg(IQS7211E_ADDR, 0x28, &buffer[0], 22, STOP_TRUE);

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
		
	iqs7211e_write_reg(IQS7211E_ADDR, 0x36, &buffer[0], 22, STOP_TRUE);
			
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
	
	iqs7211e_write_reg(IQS7211E_ADDR, 0x41, &buffer[0], 20, STOP_TRUE);	

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
	
	iqs7211e_write_reg(IQS7211E_ADDR, 0x4B, &buffer[0], 22, STOP_TRUE);

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
	
	iqs7211e_write_reg(IQS7211E_ADDR, 0x56, &buffer[0], 14, STOP_TRUE);
		
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
	
	iqs7211e_write_reg(IQS7211E_ADDR, 0x5D, &buffer[0], 30, STOP_TRUE);
		
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

	iqs7211e_write_reg(IQS7211E_ADDR, 0x6C, &buffer[0], 33, STOP_TRUE);
	
	/* Change the System Settings */
/* Memory Map Position 0x33 - 0x35 */
	buffer[0] = SYSTEM_CONTROL_0 | 0x80 | 0x20;  // Ack Reset, TP Re-ATI
	buffer[1] = SYSTEM_CONTROL_1;
	buffer[2] = CONFIG_SETTINGS0 | 0x40;  //bit6 : 1 Write one or two bytes (any data) to the address 0xFF followed by a STOP to end comms
	buffer[3] = CONFIG_SETTINGS1 | 0x01;  //0x01 : Enable Event Mode
	buffer[4] = OTHER_SETTINGS_0;
	buffer[5] = OTHER_SETTINGS_1;
	
	iqs7211e_write_reg(IQS7211E_ADDR, 0x33, &buffer[0], 6, STOP_TRUE);
	
	return 0;
}

/*
//stop信号结束通信功能关闭
*/
void IQS7211E_Stop_Bit_Disabled(void)
{
	uint8_t buffer[2];
		  
	buffer[0] = CONFIG_SETTINGS0 | 0x40;  //bit6 : 1 Write one or two bytes (any data) to the address 0xFF followed by a STOP to end comms;
	iqs7211e_write_reg(IQS7211E_ADDR, 0x34, &buffer[0], 1, STOP_TRUE);
}

/*
//关闭通信窗口
*/
void IQS7211E_Stop_I2C_Comm_Window(void)
{
	uint8_t buffer[2];

	buffer[0] = CONFIG_SETTINGS0 & 0xBF;  //bit6 : 1 Write one or two bytes (any data) to the address 0xFF followed by a STOP to end comms;
	iqs7211e_write_reg(IQS7211E_ADDR, 0x34, &buffer[0], 1, STOP_TRUE);
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
		iqs7211e_read_reg(IQS7211E_ADDR, 0x00, &Version_Details_buffer[0], 6, STOP_TRUE);
		iqs7211e_read_reg(IQS7211E_ADDR, 0x0A, &System_Data_buffer[0], 20, STOP_TRUE);
		//Read_Data(IQS7211E_ADDR,0x20,&Channel_Touch_Status[0],4,STOP_FAULT);

		//0x0F Register Info Flags
		if(System_Data_buffer[10] & 0x80)
		{//Reset occurred
			log_i("\n IQS7211E_Reset_occurred");
			IQS7211E_Init(&i2c0);
			log_i("\n IQS7211E_Init_finished");

			Product_Number = ((uint16_t)Version_Details_buffer[1] << 8) + \
								Version_Details_buffer[0];
			Major_Version = ((uint16_t)Version_Details_buffer[3] << 8) + \
								Version_Details_buffer[2];
			Minor_Version = ((uint16_t)Version_Details_buffer[5] << 8) + \
								Version_Details_buffer[4];
			log_i("\n Product_Number =%d, \
					Major_Version =%d, \
					Minor_Version =%d", \
					Product_Number, Major_Version, Minor_Version);
		}
		else
		{
			//Read_Data(IQS7211E_ADDR,0xE3,&PCC_Values[0],64,STOP_FAULT);
			if(System_Data_buffer[10] & 0x20)
			{//ALP ATI Error
				log_i("\n IQS7211E_ALP_ATI_Error");
			}

			if(System_Data_buffer[10] & 0x08)
			{//ATI Error
				log_i("\n IQS7211E_ATI_Error");
			}
			else if(System_Data_buffer[10] & 0x10)
			{//ATI active
				log_i("\n IQS7211E_ATI_active");
			}
			else
			{
				Finger_Amount = System_Data_buffer[11] & 0x03;
				Finger_1_X_coordinate = ((uint16_t)System_Data_buffer[13] << 8) + \
											System_Data_buffer[12];
				Finger_1_Y_coordinate = ((uint16_t)System_Data_buffer[15]<<8) + \
											System_Data_buffer[14];
				log_i("\n X_coordinate>> %d", Finger_1_X_coordinate);
				log_i("\n Y_coordinate>> %d", Finger_1_Y_coordinate);
				//Single Tap
				if(System_Data_buffer[8] & 0x01)
				{
					log_i("\n Single Tap");
				}

				//Double Tap
				if(System_Data_buffer[8] & 0x02)
				{
					log_i("\n Double Tap");
				}

				//Triple Tap
				if(System_Data_buffer[8] & 0x04)
				{
					log_i("\n Triple Tap");
				}

				// Press and Hold
				if(System_Data_buffer[8]&0x08)
				{
					log_i("\n Press and Hold");
				}

				//Palm Gesture
				if(System_Data_buffer[8] & 0x10)
				{
					log_i("\n Palm Gesture");	
				}

				//Swipe X+
				if(System_Data_buffer[9] & 0x01)
				{
					log_i("\n Swipe X+");	
				}

				//Swipe X-
				if(System_Data_buffer[9] & 0x02)
				{
					log_i("\n Swipe X-");
				}

				//Swipe Y+
				if(System_Data_buffer[9] & 0x04)
				{
					log_i("\n Swipe Y+");
				}

				//Swipe Y-
				if(System_Data_buffer[9] & 0x08)
				{
					log_i("\n Swipe Y-");	
				}

				//Swipe and Hold X+
				if(System_Data_buffer[9] & 0x10)
				{
					log_i("\n Swipe and Hold X+");
				}
			
				//Swipe and Hold X-
				if(System_Data_buffer[9] & 0x20)
				{
					log_i("\n Swipe and Hold X-");
				}

				//Swipe and Hold Y+
				if(System_Data_buffer[9] & 0x40)
				{
					log_i("\n Swipe and Hold Y+");
				}
			
				//Swipe and Hold Y-
				if(System_Data_buffer[9] & 0x80)
				{
					log_i("\n Swipe and Hold Y-");
				}
			}
		}
		//buffer[0] = 0;
		//Write_Data(IQS7211E_ADDR,0xFF,&buffer[0],1,STOP_TRUE);
        IQS7211E_Stop_I2C_Comm_Window();

        log_i("\n END RDY Comms");
	}
}
#endif

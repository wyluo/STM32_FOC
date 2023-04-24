#include "app_mpu6050.h"
#include "inv_mpu.h"
#include "i2c_hw.h"

#if MPU6050_ENABLE
static signed char gyro_orientation[9] = {-1, 0, 0, \
                                           0,-1, 0, \
                                           0, 0, 1};
extern u8 motor_start_flag;

struct i2c_dev_device *mpu6050_i2c_dev = NULL;

static char mpu6050_read_regs(uint8_t reg_addr, uint8_t *read_buff, uint8_t read_size)
{
    struct i2c_dev_message mpu6050_msg[2];
    char ret;

    mpu6050_msg[0].addr  = MPU6050_SLAVE_ADDR;
    mpu6050_msg[0].flags = I2C_BUS_WR;
    mpu6050_msg[0].buff  = &reg_addr;
    mpu6050_msg[0].size  = 1;
    mpu6050_msg[1].addr  = MPU6050_SLAVE_ADDR;
    mpu6050_msg[1].flags = I2C_BUS_RD;
    mpu6050_msg[1].buff  = read_buff;
    mpu6050_msg[1].size  = read_size;
    ret = i2c_bus_xfer(&mpu6050_i2c_dev, mpu6050_msg, 2);

    return ret;
}

/* 将一个字节写入寄存器 */
static char mpu6050_write_reg0(u8 reg, u8 data)
{
    struct i2c_dev_message mpu6050_msg[1];
    u8 buff[2] = {0};
    u8 slave_addr = 0;
    char ret = 0;
      
    slave_addr = MPU6050_SLAVE_ADDR;
    buff[0] = reg;
    buff[1] = data;

    mpu6050_msg[0].addr  = slave_addr;
    mpu6050_msg[0].flags = I2C_BUS_WR;
    mpu6050_msg[0].buff  = buff;
    mpu6050_msg[0].size  = 2;
    ret = i2c_bus_xfer(mpu6050_i2c_dev, mpu6050_msg, 1);
      
    return ret;
}

/*将多个字节写入寄存器*/
static char mpu6050_write_regs(u8 reg_addr, u8 *write_buff, u8 write_size)
{
    struct i2c_dev_message mpu6050_msg[2];
    u8  slave_addr;
    char ret;
      
    slave_addr = MPU6050_SLAVE_ADDR;
    mpu6050_msg[0].addr = slave_addr;
    mpu6050_msg[0].flags = I2C_BUS_WR;
    mpu6050_msg[0].buff  = &reg_addr;
    mpu6050_msg[0].size  = 1;
    mpu6050_msg[1].addr = slave_addr;
    mpu6050_msg[1].flags = I2C_BUS_WR | I2C_BUS_NO_START;
    mpu6050_msg[1].buff  = write_buff;
    mpu6050_msg[1].size  = write_size;
    ret = i2c_bus_xfer(mpu6050_i2c_dev, mpu6050_msg, 2);
      
    return ret;
}

/**************************************************************************
Function: Read, modify, and write one bit in a byte of the specified device specified register
Input   : dev   ：Target device IIC address；
          reg   ：Register address；
          bitNum：To modify the bitnum bit of the target byte；
          data  ：When it is 0, the target bit will be cleared, otherwise it will be set
Output  : 1：success；0：fail
函数功能：读 修改 写 指定设备 指定寄存器一个字节 中的1个位
入口参数：dev：目标设备地址；reg：寄存器地址；bitNum：要修改目标字节的bitNum位；
					data：为0时，目标位将被清，否则将被置位
返回  值：1：成功；0：失败
**************************************************************************/ 
static u8 mpu6050_write_regbit0(u8 reg, u8 bitNum, u8 data)
{
    u8 b;
    mpu6050_read_regs(reg, &b, 1);
    b = (data != 0) ? (b | (1 << bitNum)) : (b & ~(1 << bitNum));
    return mpu6050_write_reg0(reg, b);
}

/**************************************************************************
Function: Read, modify, and write multiple bits in a byte of the specified device specified register
Input   : dev       ：Target device IIC address；
          reg       ：Register address；
          length    ：Number of bytes；
          bitStart  ：Start bit of target byte；data：Stores the value of the target byte bit to be changed
Output  : 1：success；
          0：fail

函数功能 ：读 修改 写 指定设备 指定寄存器一个字节中的多个位
入口参数 ：dev       ：目标设备地址；
          reg       ：寄存器地址；
          bitStart  ：目标字节的起始位；
          data      ：存放改变目标字节位的值
返回值   ：1：成功；
          0：失败
**************************************************************************/ 
u8 mpu6050_write_regbit(u8 reg_addr, u8 bitStart, u8 length, u8 data)
{
    u8 b;
    if(mpu6050_read_regs(reg_addr, &b, 1) != 0)
    {
        u8 mask = (0xFF << (bitStart + 1)) | 0xFF >> ((8 - bitStart) + length - 1);
        data <<= (8 - length);
        data >>= (7 - bitStart);
        b &= mask;
        b |= data;
        return mpu6050_write_reg0(reg_addr, b);
    }
    else
    {
        return 0;
    }
}

/**************************实现函数********************************************
*函数原型:		void MPU6050_setClockSource(uint8_t source)
*功　　能:	    设置  MPU6050 的时钟源
 * CLK_SEL | Clock Source
 * --------+--------------------------------------
 * 0       | Internal oscillator
 * 1       | PLL with X Gyro reference
 * 2       | PLL with Y Gyro reference
 * 3       | PLL with Z Gyro reference
 * 4       | PLL with external 32.768kHz reference
 * 5       | PLL with external 19.2MHz reference
 * 6       | Reserved
 * 7       | Stops the clock and keeps the timing generator in reset
*******************************************************************************/
static mpu6050_set_clocksource(u8 source)
{
    mpu6050_write_regbit(MPU6050_RA_PWR_MGMT_1, MPU6050_PWR1_CLKSEL_BIT, MPU6050_PWR1_CLKSEL_LENGTH, source);
}

/** Set full-scale gyroscope range.
 * @param range New full-scale gyroscope range value
 * @see getFullScaleRange()
 * @see MPU6050_GYRO_FS_250
 * @see MPU6050_RA_GYRO_CONFIG
 * @see MPU6050_GCONFIG_FS_SEL_BIT
 * @see MPU6050_GCONFIG_FS_SEL_LENGTH
 */
static mpu6050_set_FullScaleGyroRange(u8 range)
{
    mpu6050_write_regbit(MPU6050_RA_GYRO_CONFIG, \
                        MPU6050_GCONFIG_FS_SEL_BIT, \
                        MPU6050_GCONFIG_FS_SEL_LENGTH, \
                        range);
}

/**************************************************************************
Function: Setting the maximum range of mpu6050 accelerometer
Input   : range：Acceleration maximum range number
Output  : none
函数功能 ：设置 MPU6050 加速度计的最大量程
入口参数 ：range：加速度最大量程编号
返回  值 ：无
**************************************************************************/
//#define MPU6050_ACCEL_FS_2          0x00//===最大量程+-2G
//#define MPU6050_ACCEL_FS_4          0x01//===最大量程+-4G
//#define MPU6050_ACCEL_FS_8          0x02//===最大量程+-8G
//#define MPU6050_ACCEL_FS_16         0x03//===最大量程+-16G
static void mpu6050_set_FullScaleAccelRange(uint8_t range)
{
    mpu6050_write_regbit(MPU6050_RA_ACCEL_CONFIG, \
                            MPU6050_ACONFIG_AFS_SEL_BIT, \
                            MPU6050_ACONFIG_AFS_SEL_LENGTH, \
                            range);
}

/**************************************************************************
Function: Set mpu6050 to sleep mode or not
Input   : enable：1，sleep；0，work；
Output  : none
函数功能：设置 MPU6050 是否进入睡眠模式
入口参数：enable：1，睡觉；0，工作；
返回  值：无
**************************************************************************/
static void mpu6050_set_SleepEnabled(uint8_t enabled)
{
    mpu6050_write_regbit0(MPU6050_RA_PWR_MGMT_1, MPU6050_PWR1_SLEEP_BIT, enabled);
}

/**************************************************************************
Function: Setting whether mpu6050 is the host of aux I2C cable
Input   : enable：1，yes；0;not
Output  : none
函数功能：设置 MPU6050 是否为AUX I2C线的主机
入口参数：enable：1，是；0：否
返回  值：无
**************************************************************************/
static void mpu6050_set_I2CMasterModeEnabled(uint8_t enabled)
{
    mpu6050_write_regbit0(MPU6050_RA_USER_CTRL, \
                            MPU6050_USERCTRL_I2C_MST_EN_BIT, \
                            enabled);
}

/**************************************************************************
Function: Setting whether mpu6050 is the host of aux I2C cable
Input   : enable：1，yes；0;not
Output  : none
函数功能：设置 MPU6050 是否为AUX I2C线的主机
入口参数：enable：1，是；0：否
返回  值：无
**************************************************************************/
static void mpu6050_set_I2CBypassEnabled(uint8_t enabled)
{
    mpu6050_write_regbit0(MPU6050_RA_INT_PIN_CFG, \
                            MPU6050_INTCFG_I2C_BYPASS_EN_BIT, \
                            enabled);
}

void tok_mpu6050_init(void)
{
    mpu6050_set_clocksource(MPU6050_CLOCK_PLL_YGYRO);
    mpu6050_set_FullScaleGyroRange(MPU6050_GYRO_FS_2000);//陀螺仪量程设置
    mpu6050_set_FullScaleAccelRange(MPU6050_ACCEL_FS_2);//加速度度最大量程 +-2G
    mpu6050_set_SleepEnabled(0);//进入工作状态
    mpu6050_set_I2CMasterModeEnabled(0);//不让MPU6050 控制AUXI2C
    mpu6050_set_I2CBypassEnabled(0);//主控制器的I2C与MPU6050的AUXI2C直通关闭
}

/**************************************************************************
Function: Initialization of DMP in mpu6050
Input   : none
Output  : none
函数功能：MPU6050内置DMP的初始化
入口参数：无
返回  值：无
**************************************************************************/
void mpu6050_dmp_init(void)
{ 
    u8 temp[1] = {0};
    motor_start_flag = 1;//Flag_Show = 1;
    mpu6050_read_regs(0x75, temp, 1);
    log_d("mpu_set_sensor complete ......\r\n");

    if(temp[0] != 0x68)
    {
        NVIC_SystemReset();
    }
    if(!mpu_init())
    {
        if(!mpu_set_sensors(INV_XYZ_GYRO | INV_XYZ_ACCEL))
        {
            log_d("mpu_set_sensor complete ......\r\n");
        }
        if(!mpu_configure_fifo(INV_XYZ_GYRO | INV_XYZ_ACCEL))
        {
            log_d("mpu_configure_fifo complete ......\r\n");
        }
        if(!mpu_set_sample_rate(DEFAULT_MPU_HZ))
        {
            log_d("mpu_set_sample_rate complete ......\r\n");
        }
        if(!dmp_load_motion_driver_firmware())
        {
            log_d("dmp_load_motion_driver_firmware complete ......\r\n");
        }
        if(!dmp_set_orientation(inv_orientation_matrix_to_scalar(gyro_orientation)))
        {
            log_d("dmp_set_orientation complete ......\r\n");
        }
        if(!dmp_enable_feature(DMP_FEATURE_6X_LP_QUAT | DMP_FEATURE_TAP \
                                | DMP_FEATURE_ANDROID_ORIENT | DMP_FEATURE_SEND_RAW_ACCEL \
                                | DMP_FEATURE_SEND_CAL_GYRO | DMP_FEATURE_GYRO_CAL))
        {
            log_d("dmp_enable_feature complete ......\r\n");
        }
        if(!dmp_set_fifo_rate(DEFAULT_MPU_HZ))
        {
            log_d("dmp_set_fifo_rate complete ......\r\n");
        }
        run_self_test();
        if(!mpu_set_dmp_state(1))
        {
            log_d("mpu_set_dmp_state complete ......\r\n");
        }
    }
    motor_start_flag = 0;//Flag_Show=0;
}


#endif

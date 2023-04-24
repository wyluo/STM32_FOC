#ifndef APP_MPU6050_H
#define APP_MPU6050_H

#include "config.h"

#if MPU6050_ENABLE

#define MPU6050_SLAVE_ADDR                      0xd0

#define PRINT_ACCEL                             (0x01)
#define PRINT_GYRO                              (0x02)
#define PRINT_QUAT                              (0x04)
#define ACCEL_ON                                (0x01)
#define GYRO_ON                                 (0x02)
#define MOTION                                  (0)
#define NO_MOTION                               (1)
#define DEFAULT_MPU_HZ                          (200)
#define FLASH_SIZE                              (512)

#define DMP_FEATURE_TAP                         (0x001)
#define DMP_FEATURE_ANDROID_ORIENT              (0x002)
#define DMP_FEATURE_LP_QUAT                     (0x004)
#define DMP_FEATURE_PEDOMETER                   (0x008)
#define DMP_FEATURE_6X_LP_QUAT                  (0x010)
#define DMP_FEATURE_GYRO_CAL                    (0x020)
#define DMP_FEATURE_SEND_RAW_ACCEL              (0x040)
#define DMP_FEATURE_SEND_RAW_GYRO               (0x080)
#define DMP_FEATURE_SEND_CAL_GYRO               (0x100)

#define MPU6050_PWR1_CLKSEL_BIT                 2
#define MPU6050_PWR1_CLKSEL_LENGTH              3

#define MPU6050_GCONFIG_FS_SEL_BIT              4
#define MPU6050_GCONFIG_FS_SEL_LENGTH           2

#define MPU6050_CLOCK_PLL_YGYRO                 0x02

#define MPU6050_GYRO_FS_250                     0x00
#define MPU6050_GYRO_FS_500                     0x01
#define MPU6050_GYRO_FS_1000                    0x02
#define MPU6050_GYRO_FS_2000                    0x03

#define MPU6050_ACCEL_FS_2                      0x00
#define MPU6050_ACCEL_FS_4                      0x01
#define MPU6050_ACCEL_FS_8                      0x02
#define MPU6050_ACCEL_FS_16                     0x03

#define MPU6050_ACONFIG_XA_ST_BIT               7
#define MPU6050_ACONFIG_YA_ST_BIT               6
#define MPU6050_ACONFIG_ZA_ST_BIT               5
#define MPU6050_ACONFIG_AFS_SEL_BIT             4
#define MPU6050_ACONFIG_AFS_SEL_LENGTH          2
#define MPU6050_ACONFIG_ACCEL_HPF_BIT           2
#define MPU6050_ACONFIG_ACCEL_HPF_LENGTH        3

#define MPU6050_PWR1_DEVICE_RESET_BIT           7
#define MPU6050_PWR1_SLEEP_BIT                  6
#define MPU6050_PWR1_CYCLE_BIT                  5
#define MPU6050_PWR1_TEMP_DIS_BIT               3
#define MPU6050_PWR1_CLKSEL_BIT                 2
#define MPU6050_PWR1_CLKSEL_LENGTH              3

#define MPU6050_USERCTRL_DMP_EN_BIT             7
#define MPU6050_USERCTRL_FIFO_EN_BIT            6
#define MPU6050_USERCTRL_I2C_MST_EN_BIT         5
#define MPU6050_USERCTRL_I2C_IF_DIS_BIT         4
#define MPU6050_USERCTRL_DMP_RESET_BIT          3
#define MPU6050_USERCTRL_FIFO_RESET_BIT         2
#define MPU6050_USERCTRL_I2C_MST_RESET_BIT      1
#define MPU6050_USERCTRL_SIG_COND_RESET_BIT     0

#define MPU6050_INTCFG_INT_LEVEL_BIT            7
#define MPU6050_INTCFG_INT_OPEN_BIT             6
#define MPU6050_INTCFG_LATCH_INT_EN_BIT         5
#define MPU6050_INTCFG_INT_RD_CLEAR_BIT         4
#define MPU6050_INTCFG_FSYNC_INT_LEVEL_BIT      3
#define MPU6050_INTCFG_FSYNC_INT_EN_BIT         2
#define MPU6050_INTCFG_I2C_BYPASS_EN_BIT        1
#define MPU6050_INTCFG_CLKOUT_EN_BIT            0

#define MPU6050_RA_GYRO_CONFIG                  0x1B
#define MPU6050_RA_ACCEL_CONFIG                 0x1C
#define MPU6050_RA_INT_PIN_CFG                  0x37
#define MPU6050_RA_USER_CTRL                    0x6A
#define MPU6050_RA_PWR_MGMT_1                   0x6B

void tok_mpu6050_init(void);
void mpu6050_dmp_init(void);
#endif

#endif

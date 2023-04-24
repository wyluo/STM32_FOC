#ifndef CONFIG_H
#define CONFIG_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <stdbool.h>

#define UNUSED(x)    (void)(x)

#define KEY_NUM    2

#define SYS_LEDPORT         GPIOC
#define SYS_LEDCTRL         GPIO_Pin_13

#ifndef ENABLE_USE_MICRO_LIB
#define ENABLE_USE_MICRO_LIB    1
#endif

#ifndef EASYLOGGER_ENABLE
#define EASYLOGGER_ENABLE       1
#endif

#ifndef MULTITIMER_ENABLE
#define MULTITIMER_ENABLE       1
#endif

#ifndef MULTIBUTTON_ENABLE
#define MULTIBUTTON_ENABLE      1
#endif

#ifndef TELINK_KEY_MOUDLE
#define TELINK_KEY_MOUDLE       0
#endif

#ifndef MW8266D_ENABLE
#define MW8266D_ENABLE          0
#endif

#ifndef IQS7211E_ENABLE
#define IQS7211E_ENABLE         0
#endif

#ifndef CHORME_DINO_ENABLE
#define CHORME_DINO_ENABLE      0
#endif

#ifndef MPU6050_ENABLE
#define MPU6050_ENABLE          0
#endif

#ifndef MOTOR_TB6612_ENABLE
#define MOTOR_TB6612_ENABLE     0
#endif

#ifndef ENABLE_USART1
#define ENABLE_USART1           1
#endif

/*
*总结已经使用的io口
*用于tb6612模块的GPIOB12,GPIOB13,GPIOB14,GPIOB15,GPIOA8,GPIOA11
*用于i2c通信的GPIOA6,GPIOA7
*按键GPIOA2,GPIOA3
*测试灯GPIOC13
*uart用于串口通信GPIOA9，GPIOA10
*uart用于esp8266d串口通信GPIOB10,GPIOB11
*/

#endif

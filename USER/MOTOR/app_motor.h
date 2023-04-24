#ifndef APP_MOTOR_H
#define APP_MOTOR_H

#include "stm32f10x.h"

#define ENCODER_TIM_PERIOD (u16)(65535)

void motor_pwm_init(u16 arr, u16 psc);
void motor_encoder_tim2_init(void);
void motor_encoder_tim4_init(void);

#endif

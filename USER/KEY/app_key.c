#include "app_key.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <stdbool.h>
#include "elog.h"

void app_Key_Init(void)
{
	GPIO_InitTypeDef GPIO_InitStructure = {0};

	GPIO_InitStructure.GPIO_Mode = KEY1_GPIO_MODE;
	GPIO_InitStructure.GPIO_Pin = KEY1_GPIO_PIN;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(KEY1_GPIO_PORT, &GPIO_InitStructure);

	GPIO_InitStructure.GPIO_Mode = KEY2_GPIO_MODE;
	GPIO_InitStructure.GPIO_Pin = KEY2_GPIO_PIN;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(KEY2_GPIO_PORT, &GPIO_InitStructure);

	GPIO_InitStructure.GPIO_Mode = KEY3_GPIO_MODE;
	GPIO_InitStructure.GPIO_Pin = KEY3_GPIO_PIN;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(KEY3_GPIO_PORT, &GPIO_InitStructure);

	GPIO_InitStructure.GPIO_Mode = KEY4_GPIO_MODE;
	GPIO_InitStructure.GPIO_Pin = KEY4_GPIO_PIN;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(KEY4_GPIO_PORT, &GPIO_InitStructure);

	log_i("Key gpio config complete.\r\n");
}




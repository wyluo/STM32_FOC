#include "app_key.h"
#include "elog.h"

///* qualcomm key driver */
//const KEY_MENU_T key_app_menu_tbl[] = {
//	{{KEY_ID_FN0, KEY_DOWN, 0xFF}, KEY_POWER},
//};
	
/* Telink key driver */
key_conrol_t key_control[KEY_NUM] = {0};

const key_cfg_t key_cfg[KEY_NUM] =
{
	{
		.port = KEY1_GPIO_PORT,
		.key_pin = KEY1_GPIO_PIN,
		.key_hold_cnt = 200,
		.key_long_cnt = 300,
		.key_intervel_cnt = 50,
		.key_default_level = KEY_DOWN,
		.key_down_level = 0,	/// ATI
		.key_short_click_func = key1_sclick_func,
		.key_double_click_func = key1_dclick_func,
		.key_tclick_func = key1_tclick_func,
//		.key_hold_func = key1_hold_func,
//		.key_long_func = key1_long_func,
//		.key_release_func = key1_release_func,
	},
	{
		.port = KEY2_GPIO_PORT,
		.key_pin = KEY2_GPIO_PIN,
		.key_hold_cnt = 200,
		.key_long_cnt = 300,
		.key_intervel_cnt = 50,
		.key_default_level = KEY_DOWN,
		.key_down_level = 0,	/// ATI
//		.key_short_click_func = power_key_sclick_func,
//		.key_double_click_func = power_key_dclick_func,
//		.key_tclick_func = power_key_tclick_func,
//		.key_hold_func = power_key_hold_func,
//		.key_long_func = power_key_long_func,
//		.key_release_func = power_key_release_func,
	},
	{
		.port = KEY3_GPIO_PORT,
		.key_pin = KEY3_GPIO_PIN,
		.key_hold_cnt = 200,
		.key_long_cnt = 300,
		.key_intervel_cnt = 50,
		.key_default_level = KEY_DOWN,
		.key_down_level = 0,	/// ATI
//		.key_short_click_func = power_key_sclick_func,
//		.key_double_click_func = power_key_dclick_func,
//		.key_tclick_func = power_key_tclick_func,
//		.key_hold_func = power_key_hold_func,
//		.key_long_func = power_key_long_func,
//		.key_release_func = power_key_release_func,
	},
	{
		.port = KEY4_GPIO_PORT,
		.key_pin = KEY4_GPIO_PIN,
		.key_hold_cnt = 200,
		.key_long_cnt = 300,
		.key_intervel_cnt = 50,
		.key_default_level = KEY_DOWN,
		.key_down_level = 0,	/// ATI
//		.key_short_click_func = power_key_sclick_func,
//		.key_double_click_func = power_key_dclick_func,
//		.key_tclick_func = power_key_tclick_func,
//		.key_hold_func = power_key_hold_func,
//		.key_long_func = power_key_long_func,
//		.key_release_func = power_key_release_func,
	},	
};

void hal_key_init(void)
{
	int i = 0;
	
	GPIO_InitTypeDef GPIO_InitStructure = {0};
	
	memset(key_control, 0, sizeof(key_control));
	
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	for(i = 0; i < KEY_NUM; i++)
	{
		GPIO_InitStructure.GPIO_Pin = key_cfg[i].key_pin;
		GPIO_Init(key_cfg[i].port, &GPIO_InitStructure);	
	}

//	GPIO_InitStructure.GPIO_Mode = KEY1_GPIO_MODE;
//	GPIO_InitStructure.GPIO_Pin = KEY1_GPIO_PIN;
//	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
//	GPIO_Init(KEY1_GPIO_PORT, &GPIO_InitStructure);

//	GPIO_InitStructure.GPIO_Mode = KEY2_GPIO_MODE;
//	GPIO_InitStructure.GPIO_Pin = KEY2_GPIO_PIN;
//	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
//	GPIO_Init(KEY2_GPIO_PORT, &GPIO_InitStructure);

//	GPIO_InitStructure.GPIO_Mode = KEY3_GPIO_MODE;
//	GPIO_InitStructure.GPIO_Pin = KEY3_GPIO_PIN;
//	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
//	GPIO_Init(KEY3_GPIO_PORT, &GPIO_InitStructure);

//	GPIO_InitStructure.GPIO_Mode = KEY4_GPIO_MODE;
//	GPIO_InitStructure.GPIO_Pin = KEY4_GPIO_PIN;
//	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
//	GPIO_Init(KEY4_GPIO_PORT, &GPIO_InitStructure);

	log_i("Key gpio config complete.\r\n");
}

bool check_key_realse(unsigned char index)
{
	if(index >= KEY_NUM)
		return 0;

	unsigned char key_val = GPIO_ReadInputDataBit(GPIOA, key_cfg[index].key_pin) ? 1 : 0;

	return (key_val == key_cfg[index].key_down_level);
}

static inline bool pin_level_get(const void *desc)
{
	key_cfg_t *pdesc;
	
	pdesc = (key_cfg_t *)desc;
	return GPIO_ReadInputDataBit(pdesc->port, pdesc->key_pin) == 0;
}

void key1_sclick_func(void)
{
	log_d("[lwy_key_debug]key1_sclick_func\r\n");
}

void key1_dclick_func(void)
{
	log_d("[lwy_key_debug]key1_dclick_func\r\n");
}

void key1_tclick_func(void)
{
	log_d("[lwy_key_debug]key1_tclick_func\r\n");	
}

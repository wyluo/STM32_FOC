#include "i2c_core.h"
#include "i2c_bitops.h"
#include "i2c_hw.h"

/********************eg: stm32f1 simulate i2c start*******************/
#define	I2C1_SDA_PORT			GPIOA
#define	I2C1_SCL_PORT			GPIOA
#define	I2C1_SDA_PIN			GPIO_Pin_6
#define	I2C1_SCL_PIN			GPIO_Pin_7

/* i2c1 device */
struct ops_i2c_dev ops_i2c1_dev;

/* i2c1 diver */
static void gpio_set_sda(int8_t state)
{
	if(state)
	{
		I2C1_SDA_PORT->BSRR = I2C1_SDA_PIN;
	}
	else
	{
		I2C1_SDA_PORT->BRR = I2C1_SDA_PIN;
	}
}

static void gpio_set_scl(int8_t state)
{
	if (state)
	{	
		I2C1_SCL_PORT->BSRR = I2C1_SCL_PIN;
	}
	else
	{	
		I2C1_SCL_PORT->BRR = I2C1_SCL_PIN;
	}
}

static int8_t gpio_get_sda(void)
{
	return (I2C1_SDA_PORT->IDR & I2C1_SDA_PIN);
}

static int8_t gpio_get_scl()
{
	return (I2C1_SCL_PORT->IDR & I2C1_SCL_PIN);
}

static void gpio_delayus(uint32_t us)
{
#if 1  /* ����ϵͳ��ʱʱ�����������*/
	volatile int32_t i;
	
	for (; us > 0; us--)
	{
		i = 30;/* mini 17 */
		while(i--);
	}
#else
		Delayus(us);
#endif
}

int ops_i2c_bus_xfer(struct i2c_dev_device *i2c_dev, struct i2c_dev_message msgs[], unsigned int num)
{
	return(i2c_bitops_bus_xfer((struct ops_i2c_dev*)(i2c_dev->i2c_phy), msgs, num));
}

/* ��ʼ��i2c���� */
void stm32f1xx_i2c_init(struct i2c_dev_device *i2c_bus)
{
	GPIO_InitTypeDef GPIO_InitStructure;
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);

	GPIO_InitStructure.GPIO_Pin = I2C1_SDA_PIN | I2C1_SCL_PIN;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_OD;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(I2C1_SDA_PORT, &GPIO_InitStructure);
	I2C1_SDA_PORT->BSRR = I2C1_SDA_PIN;
	I2C1_SCL_PORT->BSRR = I2C1_SCL_PIN;
		
	/* device init */
	ops_i2c1_dev.set_sda = gpio_set_sda;
	ops_i2c1_dev.get_sda = gpio_get_sda;
	ops_i2c1_dev.set_scl = gpio_set_scl;
	ops_i2c1_dev.get_scl = gpio_get_scl;
	ops_i2c1_dev.delayus = gpio_delayus;

	i2c_bus->i2c_phy = &ops_i2c1_dev;
	i2c_bus->xfer = ops_i2c_bus_xfer; 
}
/********************eg: stm32f1 simulate i2c end*******************/

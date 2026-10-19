#include <stdint.h>
#include <string.h>
#include "stm32f411x.h"
#include "stm32f411x_gpio_driver.h"
#include "stm32f411x_usart_driver.h"

USART_Handle USART2Handle;

void delay(void)
{
	for (uint32_t i = 0; i < 500000; i++)
		;
}

void USART2_GPIOInit(void)
{
	GPIO_Handle usart_gpios;

	usart_gpios.pGPIOx = GPIOA;
	usart_gpios.GPIO_PinConfig.GPIO_PIN_MODE = GPIO_MODE_ALT_FUNC;
	usart_gpios.GPIO_PinConfig.GPIO_PIN_OUTPUT_TYPE = GPIO_PUSH_PULL;
	usart_gpios.GPIO_PinConfig.GPIO_PIN_PUPD_MODE = GPIO_PUPD_PU;
	usart_gpios.GPIO_PinConfig.GPIO_PIN_SPEED = GPIO_SPEED_FAST;
	usart_gpios.GPIO_PinConfig.GPIO_PIN_ALT_FUNC_MODE = GPIO_AF_NO_7;

	usart_gpios.GPIO_PinConfig.GPIO_PIN_NUMBER = GPIO_PIN_NO_2;
	GPIO_Init(&usart_gpios);

	usart_gpios.GPIO_PinConfig.GPIO_PIN_NUMBER = GPIO_PIN_NO_3;
	GPIO_Init(&usart_gpios);

	return;
}

void USART2_Init_Config(void)
{
	USART2Handle.pUSARTx = USART2;
	USART2Handle.USART_Config.USART_Baud = USART_STD_BAUD_115200;
	USART2Handle.USART_Config.USART_HWFlowControl = USART_HW_FLOW_CTRL_NONE;
	USART2Handle.USART_Config.USART_Mode = USART_MODE_TXRX;
	USART2Handle.USART_Config.USART_NoOfStopBits = USART_STOPBITS_1;
	USART2Handle.USART_Config.USART_WordLength = USART_WORDLEN_9BITS;
	USART2Handle.USART_Config.USART_ParityControl = USART_PARITY_EN_EVEN;

	USART_Init(&USART2Handle);

	return;
}

int main(void)
{
	USART2_GPIOInit();
	USART2_Init_Config();
	USART_PeriControl(USART2, ENABLE);

	unsigned char *buffer = (unsigned char*) "Hello from stm32!\r\n";

	USART_SendData(&USART2Handle, buffer, strlen((char*) buffer));

	while (1) {
		delay();
		USART_SendData(&USART2Handle, buffer, strlen((char*) buffer));
	}

	return 0;
}

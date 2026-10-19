#include <stdint.h>
#include "stm32f411x_gpio_driver.h"

static void GPIO_Enable_EXTI(const GPIO_Handle *const pHandle);
static void GPIO_Config_EXTI_RFE(const GPIO_Handle *const pHandle);
static void GPIO_Config_Interrupt(const GPIO_Handle *const pHandle);
static void GPIO_Config_ALT_FUNC(GPIO_Handle *pHandle);
static void GPIO_Config_Pin_Mode(GPIO_Handle *pHandle);
static void GPIO_Config_Speed(GPIO_Handle *pHandle);
static void GPIO_Config_Output_Type(GPIO_Handle *pHandle);
static void GPIO_Config_PUPD(GPIO_Handle *pHandle);

static void GPIO_PeriClockControl(const GPIO_RegDef *const pGPIO, uint8_t State);

void GPIO_Enable_EXTI(const GPIO_Handle *const pHandle)
{
	const uint8_t PIN_EXTI_CR = pHandle->GPIO_PinConfig.GPIO_PIN_NUMBER
			/ SYSCFG_EXTI_CR_REG_SIZE;
	const uint8_t SHIFT_AMOUNT = (pHandle->GPIO_PinConfig.GPIO_PIN_NUMBER
			% SYSCFG_EXTI_CR_REG_SIZE) * SYSCFG_EXTI_CR_REG_SIZE;
	const uint8_t PORT_CODE = GPIO_BASEADDR_TO_EXTI_CODE(pHandle->pGPIOx);

	SYSCFG_PERI_CLCK_ENABLE();

	SYSCFG->SYSCFG_EXTICR[PIN_EXTI_CR] &= ~(15U << SHIFT_AMOUNT);
	SYSCFG->SYSCFG_EXTICR[PIN_EXTI_CR] |= (PORT_CODE << SHIFT_AMOUNT);

	return;
}

void GPIO_Config_EXTI_RFE(const GPIO_Handle *const pHandle)
{
	if (pHandle->GPIO_PinConfig.GPIO_PIN_MODE == GPIO_MODE_IT_FE) {
		EXTI->EXTI_FTSR |= (1U
				<< pHandle->GPIO_PinConfig.GPIO_PIN_NUMBER);
		EXTI->EXTI_RTSR &= ~(1U
				<< pHandle->GPIO_PinConfig.GPIO_PIN_NUMBER);
	} else if (pHandle->GPIO_PinConfig.GPIO_PIN_MODE == GPIO_MODE_IT_RE) {
		EXTI->EXTI_RTSR |= (1U
				<< pHandle->GPIO_PinConfig.GPIO_PIN_NUMBER);
		EXTI->EXTI_FTSR &= ~(1U
				<< pHandle->GPIO_PinConfig.GPIO_PIN_NUMBER);
	} else if (pHandle->GPIO_PinConfig.GPIO_PIN_MODE == GPIO_MODE_IT_RFE) {
		EXTI->EXTI_RTSR |= (1U
				<< pHandle->GPIO_PinConfig.GPIO_PIN_NUMBER);
		EXTI->EXTI_FTSR |= (1U
				<< pHandle->GPIO_PinConfig.GPIO_PIN_NUMBER);
	}

	return;
}

void GPIO_Config_Interrupt(const GPIO_Handle *const pHandle)
{
	GPIO_Config_EXTI_RFE(pHandle);
	GPIO_Enable_EXTI(pHandle);

	EXTI->EXTI_IMR |= (1U << pHandle->GPIO_PinConfig.GPIO_PIN_NUMBER);

	return;
}

void GPIO_Config_ALT_FUNC(GPIO_Handle *pHandle)
{
	const uint32_t config_value = pHandle->GPIO_PinConfig.GPIO_PIN_NUMBER
			% PINS_PER_AF_REG;

	if (pHandle->GPIO_PinConfig.GPIO_PIN_NUMBER >= AFRH_BOUND) {
		pHandle->pGPIOx->AFRH &= ~(15U
				<< (ALT_FUNC_REG_SIZE * config_value));
		pHandle->pGPIOx->AFRH |=
				pHandle->GPIO_PinConfig.GPIO_PIN_ALT_FUNC_MODE
						<< (ALT_FUNC_REG_SIZE
								* config_value);

		return;
	}

	pHandle->pGPIOx->AFRL &= ~(15U << (ALT_FUNC_REG_SIZE * config_value));
	pHandle->pGPIOx->AFRL |= pHandle->GPIO_PinConfig.GPIO_PIN_ALT_FUNC_MODE
			<< (ALT_FUNC_REG_SIZE * config_value);

	return;
}

void GPIO_Config_Pin_Mode(GPIO_Handle *pHandle)
{
	uint32_t config_value;

	if (pHandle->GPIO_PinConfig.GPIO_PIN_MODE <= GPIO_MODE_ANALOG) {
		config_value =
				pHandle->GPIO_PinConfig.GPIO_PIN_MODE
						<< (2U
								* pHandle->GPIO_PinConfig.GPIO_PIN_NUMBER);
		pHandle->pGPIOx->MODER &=
				~(3U
						<< (2U
								* pHandle->GPIO_PinConfig.GPIO_PIN_NUMBER));
		pHandle->pGPIOx->MODER |= config_value;

		if (pHandle->GPIO_PinConfig.GPIO_PIN_MODE
				== GPIO_MODE_ALT_FUNC) {
			GPIO_Config_ALT_FUNC(pHandle);

		}

		return;
	}

	pHandle->pGPIOx->MODER &= ~(3U
			<< (2U * pHandle->GPIO_PinConfig.GPIO_PIN_NUMBER));

	GPIO_Config_Interrupt(pHandle);

	return;
}

void GPIO_Config_Speed(GPIO_Handle *pHandle)
{
	const uint32_t config_value = pHandle->GPIO_PinConfig.GPIO_PIN_SPEED
			<< (2U * pHandle->GPIO_PinConfig.GPIO_PIN_NUMBER);

	pHandle->pGPIOx->OSPEEDR &= ~(3U
			<< (2U * pHandle->GPIO_PinConfig.GPIO_PIN_NUMBER));

	pHandle->pGPIOx->OSPEEDR |= config_value;

	return;
}

void GPIO_Config_Output_Type(GPIO_Handle *pHandle)
{
	const uint32_t config_value =
			pHandle->GPIO_PinConfig.GPIO_PIN_OUTPUT_TYPE
					<< pHandle->GPIO_PinConfig.GPIO_PIN_NUMBER;

	pHandle->pGPIOx->OTYPER &= ~(1U
			<< pHandle->GPIO_PinConfig.GPIO_PIN_NUMBER);

	pHandle->pGPIOx->OTYPER |= config_value;

	return;
}

void GPIO_Config_PUPD(GPIO_Handle *pHandle)
{
	const uint32_t config_value = pHandle->GPIO_PinConfig.GPIO_PIN_PUPD_MODE
			<< (2U * pHandle->GPIO_PinConfig.GPIO_PIN_NUMBER);

	pHandle->pGPIOx->PUPDR &= ~(3U
			<< (2U * pHandle->GPIO_PinConfig.GPIO_PIN_NUMBER));

	pHandle->pGPIOx->PUPDR |= config_value;

	return;
}

void GPIO_Init(GPIO_Handle *pHandle)
{
	GPIO_PeriClockControl(pHandle->pGPIOx, ENABLE);
	GPIO_Config_Pin_Mode(pHandle);
	GPIO_Config_Speed(pHandle);

	if (pHandle->GPIO_PinConfig.GPIO_PIN_MODE == GPIO_MODE_OUTPUT) {
		GPIO_Config_Output_Type(pHandle);
	}

	GPIO_Config_PUPD(pHandle);

	return;
}

void GPIO_DeInit(const GPIO_RegDef *const pGPIO)
{
	if (pGPIO == GPIOA)
		GPIOA_REG_RESET();
	else if (pGPIO == GPIOB)
		GPIOB_REG_RESET();
	else if (pGPIO == GPIOC)
		GPIOC_REG_RESET();
	else if (pGPIO == GPIOD)
		GPIOD_REG_RESET();
	else if (pGPIO == GPIOE)
		GPIOE_REG_RESET();
	else if (pGPIO == GPIOH)
		GPIOH_REG_RESET();

	return;
}

void GPIO_PeriClockControl(const GPIO_RegDef *const pGPIO, uint8_t State)
{
	if (State == ENABLE) {
		if (pGPIO == GPIOA)
			GPIOA_PERI_CLCK_ENABLE();
		else if (pGPIO == GPIOB)
			GPIOB_PERI_CLCK_ENABLE();
		else if (pGPIO == GPIOC)
			GPIOC_PERI_CLCK_ENABLE();
		else if (pGPIO == GPIOD)
			GPIOD_PERI_CLCK_ENABLE();
		else if (pGPIO == GPIOE)
			GPIOE_PERI_CLCK_ENABLE();
		else if (pGPIO == GPIOH)
			GPIOH_PERI_CLCK_ENABLE();
	} else {
		if (pGPIO == GPIOA)
			GPIOA_PERI_CLCK_DISABLE();
		else if (pGPIO == GPIOB)
			GPIOB_PERI_CLCK_DISABLE();
		else if (pGPIO == GPIOC)
			GPIOC_PERI_CLCK_DISABLE();
		else if (pGPIO == GPIOD)
			GPIOD_PERI_CLCK_DISABLE();
		else if (pGPIO == GPIOE)
			GPIOE_PERI_CLCK_DISABLE();
		else if (pGPIO == GPIOH)
			GPIOH_PERI_CLCK_DISABLE();
	}

	return;
}

uint8_t GPIO_ReadFromInputPin(const GPIO_RegDef *const pGPIO,
		GPIO_Pin_Numbers PIN_Number)
{
	const uint8_t value =
			(uint8_t) ((pGPIO->IDR >> PIN_Number) & 0x00000001);

	return value;
}

uint16_t GPIO_ReadFromInputPort(const GPIO_RegDef *const pGPIO)
{
	const uint16_t value = (uint16_t) pGPIO->IDR;

	return value;
}

void GPIO_WriteToOutputPin(GPIO_RegDef *pGPIO, GPIO_Pin_Numbers PIN_Number,
		uint8_t Value)
{
	if (Value == GPIO_PIN_SET) {
		pGPIO->ODR |= (1U << PIN_Number);

		return;
	}

	pGPIO->ODR &= ~(1U << PIN_Number);

	return;
}

void GPIO_WriteToOutputPort(GPIO_RegDef *pGPIO, uint16_t Value)
{
	pGPIO->ODR = Value;

	return;
}

void GPIO_ToggleOutputPin(GPIO_RegDef *pGPIO, GPIO_Pin_Numbers PIN_Number)
{
	pGPIO->ODR ^= (1U << PIN_Number);

	return;
}

void GPIO_IRQ_Interrupt_Config(uint8_t IRQ_Number, uint8_t State)
{
	if (State == ENABLE) {
		if (IRQ_Number <= 31) {
			*NVIC_ISER0REG |= (1U << IRQ_Number);
		} else if (IRQ_Number > 31 && IRQ_Number < 64) {
			*NVIC_ISER1REG |= (1U << (IRQ_Number % NVIC_REG_SIZE));
		} else if (IRQ_Number >= 64 && IRQ_Number < 96) {
			*NVIC_ISER2REG |= (1U << (IRQ_Number % NVIC_REG_SIZE));
		}
	} else {
		if (IRQ_Number <= 31) {
			*NVIC_ICER0REG |= (1U << IRQ_Number);
		} else if (IRQ_Number > 31 && IRQ_Number < 64) {
			*NVIC_ICER1REG |= (1U << (IRQ_Number % NVIC_REG_SIZE));
		} else if (IRQ_Number >= 64 && IRQ_Number < 96) {
			*NVIC_ICER2REG |= (1U << (IRQ_Number % NVIC_REG_SIZE));
		}
	}

	return;
}

void GPIO_IRQ_Priority_Config(uint8_t IRQ_Number, uint32_t IRQ_Priority)
{
	const uint8_t IPR_REG_NUMBER = IRQ_Number / IPR_REG_SIZE;
	const uint8_t IPR_SECTION = IRQ_Number % IPR_REG_SIZE;
	const uint8_t SHIFT_AMOUNT = (uint8_t) ((IPR_SECTION
			* IPR_REG_SEC_SIZE_BYTE) + IPR_UNIMPLEMENTED_BITS);

	volatile uint32_t *pIPRx = NVIC_IPR_BASEADDR + IPR_REG_NUMBER;

	*pIPRx &= ~(IPR_SECTION_MASK << (IPR_SECTION * IPR_REG_SEC_SIZE_BYTE));
	*pIPRx |= (IRQ_Priority << SHIFT_AMOUNT);

	return;
}

void GPIO_IRQHandling(GPIO_Pin_Numbers PIN_Number)
{
	if (EXTI->EXTI_PR & (1U << PIN_Number)) {
		EXTI->EXTI_PR |= (1U << PIN_Number);
	}

	return;
}

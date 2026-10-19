#pragma once

#include "stm32f411x.h"

typedef enum {
	GPIO_MODE_INPUT = 0U,
	GPIO_MODE_OUTPUT = 1U,
	GPIO_MODE_ALT_FUNC = 2U,
	GPIO_MODE_ANALOG = 3U,
	GPIO_MODE_IT_FE = 4U,
	GPIO_MODE_IT_RE = 5U,
	GPIO_MODE_IT_RFE = 6U
} GPIO_Mode;

typedef enum {
	GPIO_PIN_NO_0 = 0U,
	GPIO_PIN_NO_1 = 1U,
	GPIO_PIN_NO_2 = 2U,
	GPIO_PIN_NO_3 = 3U,
	GPIO_PIN_NO_4 = 4U,
	GPIO_PIN_NO_5 = 5U,
	GPIO_PIN_NO_6 = 6U,
	GPIO_PIN_NO_7 = 7U,
	GPIO_PIN_NO_8 = 8U,
	GPIO_PIN_NO_9 = 9U,
	GPIO_PIN_NO_10 = 10U,
	GPIO_PIN_NO_11 = 11U,
	GPIO_PIN_NO_12 = 12U,
	GPIO_PIN_NO_13 = 13U,
	GPIO_PIN_NO_14 = 14U,
	GPIO_PIN_NO_15 = 15U,
} GPIO_Pin_Numbers;

typedef enum {
	GPIO_PUSH_PULL = 0U, GPIO_OPEN_DRAIN = 1U
} GPIO_Output_Type;

typedef enum {
	GPIO_SPEED_LOW = 0U,
	GPIO_SPEED_MEDIUM = 1U,
	GPIO_SPEED_FAST = 2U,
	GPIO_SPEED_HIGH = 3U
} GPIO_Speed;

typedef enum {
	GPIO_NO_PUPD = 0U,
	GPIO_PUPD_PU = 1U,
	GPIO_PUPD_PD = 2U,
	GPIO_PUPD_RESERVED = 3U,
} GPIO_PUPD;

typedef enum {
	GPIO_AF_NO_0 = 0U,
	GPIO_AF_NO_1 = 1U,
	GPIO_AF_NO_2 = 2U,
	GPIO_AF_NO_3 = 3U,
	GPIO_AF_NO_4 = 4U,
	GPIO_AF_NO_5 = 5U,
	GPIO_AF_NO_6 = 6U,
	GPIO_AF_NO_7 = 7U,
	GPIO_AF_NO_8 = 8U,
	GPIO_AF_NO_9 = 9U,
	GPIO_AF_NO_10 = 10U,
	GPIO_AF_NO_11 = 11U,
	GPIO_AF_NO_12 = 12U,
	GPIO_AF_NO_13 = 13U,
	GPIO_AF_NO_14 = 14U,
	GPIO_AF_NO_15 = 15U,
} GPIO_AF_Mode_Numbers;

typedef struct {
	GPIO_Pin_Numbers GPIO_PIN_NUMBER;
	GPIO_Mode GPIO_PIN_MODE;
	GPIO_Output_Type GPIO_PIN_OUTPUT_TYPE;
	GPIO_Speed GPIO_PIN_SPEED;
	GPIO_PUPD GPIO_PIN_PUPD_MODE;
	GPIO_AF_Mode_Numbers GPIO_PIN_ALT_FUNC_MODE;
} GPIO_Pin_Config;

typedef struct {
	GPIO_RegDef *pGPIOx;
	GPIO_Pin_Config GPIO_PinConfig;
} GPIO_Handle;

void GPIO_Init(GPIO_Handle *pHandle);
void GPIO_DeInit(const GPIO_RegDef *const pGPIO);

uint8_t GPIO_ReadFromInputPin(const GPIO_RegDef *const pGPIO,
		GPIO_Pin_Numbers PIN_Number);
uint16_t GPIO_ReadFromInputPort(const GPIO_RegDef *const pGPIO);
void GPIO_WriteToOutputPin(GPIO_RegDef *pGPIO, GPIO_Pin_Numbers PIN_Number,
		uint8_t Value);
void GPIO_WriteToOutputPort(GPIO_RegDef *pGPIO, uint16_t Value);
void GPIO_ToggleOutputPin(GPIO_RegDef *pGPIO, GPIO_Pin_Numbers PIN_Number);

void GPIO_IRQ_Interrupt_Config(uint8_t IRQ_Number, uint8_t State);
void GPIO_IRQ_Priority_Config(uint8_t IRQ_Number, uint32_t IRQ_Priority);
void GPIO_IRQHandling(GPIO_Pin_Numbers PIN_Number);


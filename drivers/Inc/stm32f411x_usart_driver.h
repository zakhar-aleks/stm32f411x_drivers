#pragma once

#include <stdint.h>
#include "stm32f411x.h"

#define USART_FLAG_PE (1U << 0U)
#define USART_FLAG_FE (1U << 1U)
#define USART_FLAG_NF (1U << 2U)
#define USART_FLAG_ORE (1U << 3U)
#define USART_FLAG_IDLE (1U << 4U)
#define USART_FLAG_RXNE (1U << 5U)
#define USART_FLAG_TC (1U << 6U)
#define USART_FLAG_TXE (1U << 7U)
#define USART_FLAG_LBD (1U << 8U)
#define USART_FLAG_CTS (1U << 9U)

typedef enum {
	HSI = 0U, HSE = 1U, PLL = 2U,
} Clock_Sources;

typedef enum {
	USART_EVENT_TX_CMPLT = 0U,
	USART_EVENT_RX_CMPLT = 1U,
	USART_EVENT_CTS = 2U,
	USART_EVENT_IDLE = 3U,
	USART_EVENT_ORE = 4U,
	USART_ERREVENT_FE = 5U,
	USART_ERREVENT_NE = 6U,
	USART_ERREVENT_ORE = 7U,
} USART_AppEvent;

typedef enum {
	USART_MODE_ONLY_TX = 0U, USART_MODE_ONLY_RX = 1U, USART_MODE_TXRX = 2U,
} USART_Modes;

typedef enum {
	USART_STD_BAUD_1200 = 1200U,
	USART_STD_BAUD_2400 = 2400U,
	USART_STD_BAUD_9600 = 9600U,
	USART_STD_BAUD_19200 = 19200U,
	USART_STD_BAUD_38400 = 38400U,
	USART_STD_BAUD_57600 = 57600U,
	USART_STD_BAUD_115200 = 115200U,
	USART_STD_BAUD_230400 = 230400U,
	USART_STD_BAUD_460800 = 460800U,
	USART_STD_BAUD_921600 = 921600U,
	USART_STD_BAUD_2M = 2000000U,
	USART_STD_BAUD_3M = 3000000U,
} USART_STD_Baud;

typedef enum {
	USART_PARITY_DISABLE = 0U,
	USART_PARITY_EN_EVEN = 1U,
	USART_PARITY_EN_ODD = 2U,
} USART_Parity;

typedef enum {
	USART_WORDLEN_8BITS = 0U, USART_WORDLEN_9BITS = 1U,
} USART_Wordlen;

typedef enum {
	USART_STOPBITS_1 = 0U,
	USART_STOPBITS_0_5 = 1U,
	USART_STOPBITS_2 = 2U,
	USART_STOPBITS_1_5 = 3U,
} USART_StopBits;

typedef enum {
	USART_HW_FLOW_CTRL_NONE = 0U,
	USART_HW_FLOW_CTRL_CTS = 1U,
	USART_HW_FLOW_CTRL_RTS = 2U,
	USART_HW_FLOW_CTRL_CTS_RTS = 3U,
} USART_HW_FLOW;

typedef enum {
	USART_READY = 0U, USART_BUSY_TX = 1U, USART_BUSY_RX = 2U,
} USART_States;

typedef struct {
	USART_Modes USART_Mode;
	USART_STD_Baud USART_Baud;
	USART_StopBits USART_NoOfStopBits;
	USART_Wordlen USART_WordLength;
	USART_Parity USART_ParityControl;
	USART_HW_FLOW USART_HWFlowControl;
} USART_Pin_Config;

typedef struct {
	USART_RegDef *pUSARTx;
	USART_Pin_Config USART_Config;
	uint32_t TxLen;
	uint8_t *pTxBuffer;
	USART_States TxBusyState;
	uint32_t RxLen;
	uint8_t *pRxBuffer;
	USART_States RxBusyState;
} USART_Handle;

void USART_Clear_ORE(USART_RegDef *pUSARTx);

void USART_PeriClockControl(USART_RegDef *pUSARTx, uint8_t State);

void USART_PeriControl(USART_RegDef *pUSARTx, uint8_t State);

void USART_Init(USART_Handle *pHandle);
void USART_DeInit(USART_RegDef *pUSARTx);

void USART_SendData(USART_Handle *pHandle, uint8_t *pTxBuffer, uint32_t Len);
void USART_ReceiveData(USART_Handle *pHandle, uint8_t *pRxBuffer, uint32_t Len);
uint8_t USART_SendDataIT(USART_Handle *pHandle, uint8_t *pTxBuffer,
		uint32_t Len);
uint8_t USART_ReceiveDataIT(USART_Handle *pHandle, uint8_t *pRxBuffer,
		uint32_t Len);

void USART_SetBaudRate(USART_RegDef *pUSARTx, uint32_t BaudRate);

uint8_t USART_GetFlagStatus(USART_RegDef *pUSARTx, uint32_t StatusFlagName);
void USART_ClearFlag(USART_RegDef *pUSARTx, uint32_t StatusFlagName);

void USART_IRQ_Interrupt_Config(uint8_t IRQ_Number, uint8_t State);
void USART_IRQ_Priority_Config(uint8_t IRQ_Number, uint32_t IRQ_Priority);
void USART_IRQHandling(USART_Handle *pHandle);

void USART_ApplicationEventCallback(USART_Handle *pHandle,
		USART_AppEvent AppEvent);

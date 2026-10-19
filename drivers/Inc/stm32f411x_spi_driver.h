#pragma once

#include "stm32f411x.h"

#define SPI_CR1_CPHA_SHIFT 0U
#define SPI_CR1_CPOL_SHIFT 1U
#define SPI_CR1_MSTR_SHIFT 2U
#define SPI_CR1_BR_SHIFT 3U
#define SPI_CR1_SPE_SHIFT 6U
#define SPI_CR1_SSI_SHIFT 8U
#define SPI_CR1_SSM_SHIFT 9U
#define SPI_CR1_RX_ONLY_SHIFT 10U
#define SPI_CR1_DFF_SHIFT 11U
#define SPI_CR1_BIDI_MODE_SHIFT 15U

#define SPI_CR2_ERRIE_SHIFT 5U
#define SPI_CR2_RXNEIE_SHIFT 6U
#define SPI_CR2_TXEIE_SHIFT 7U

#define SPI_SR_RXNE_SHIFT 0U
#define SPI_SR_TXE_SHIFT 1U
#define SPI_SR_CHSIDE_SHIFT 2U
#define SPI_SR_UDR_SHIFT 3U
#define SPI_SR_CRCERR_SHIFT 4U
#define SPI_SR_MODF_SHIFT 1U
#define SPI_SR_OVR_SHIFT 1U
#define SPI_SR_BSY_SHIFT 1U
#define SPI_SR_FRE_SHIFT 1U

#define SPI_RXNE_FLAG (1U << SPI_SR_RXNE_SHIFT)
#define SPI_TXE_FLAG (1U << SPI_SR_TXE_SHIFT)
#define SPI_CHSIDE_FLAG (1U << SPI_SR_CHSIDE_SHIFT)
#define SPI_UDR_FLAG (1U << SPI_SR_UDR_SHIFT)
#define SPI_CRCERR_FLAG (1U << SPI_SR_CRCERR_SHIFT)
#define SPI_MODF_FLAG (1U << SPI_SR_MODF_SHIFT)
#define SPI_OVR_FLAG (1U << SPI_SR_OVR_SHIFT)
#define SPI_BSY_FLAG (1U << SPI_SR_BSY_SHIFT)
#define SPI_FRE_FLAG (1U << SPI_SR_FRE_SHIFT)

typedef enum {
	SPI_DEVICE_MODE_SLAVE = 0U, SPI_DEVICE_MODE_MASTER = 1U,
} SPI_Device_Modes;

typedef enum {
	SPI_BUS_CONFIG_FULL_DUPLEX = 0U,
	SPI_BUS_CONFIG_HALF_DUPLEX = 1U,
	SPI_BUS_CONFIG_SIMPLEX_RXONLY = 2U,
} SPI_BusConfigs;

typedef enum {
	SPI_SCLK_SPEED_DIV2 = 0U,
	SPI_SCLK_SPEED_DIV4 = 1U,
	SPI_SCLK_SPEED_DIV8 = 2U,
	SPI_SCLK_SPEED_DIV16 = 3U,
	SPI_SCLK_SPEED_DIV32 = 4U,
	SPI_SCLK_SPEED_DIV64 = 5U,
	SPI_SCLK_SPEED_DIV128 = 6U,
	SPI_SCLK_SPEED_DIV256 = 7U,
} SPI_ClockSpeed;

typedef enum {
	SPI_DFF_8BITS = 0U, SPI_DFF_16BITS = 1U,
} SPI_DFF_Modes;

typedef enum {
	SPI_CPOL_LOW = 0U, SPI_CPOL_HIGH = 1U,
} SPI_CPOL_MODES;

typedef enum {
	SPI_CPHA_LOW = 0U, SPI_CPHA_HIGH = 1U,
} SPI_CPHA_MODES;

typedef enum {
	SPI_SSM_SOFTWARE_DISABLED = 0U, SPI_SSM_SOFTWARE_ENABLED = 1U,
} SPI_SSM_MODES;

typedef enum {
	SPI_READY = 0U, SPI_BUSY_TX = 1U, SPI_BUSY_RX = 2U
} SPI_STATES;

typedef enum {
	SPI_EVENT_TX_CMPLT = 0U, SPI_EVENT_RX_CMPLT = 1U, SPI_EVENT_OVR_ERR = 2U,
} SPI_EVENTS;

typedef struct {
	SPI_Device_Modes SPI_DeviceMode;
	SPI_BusConfigs SPI_BusConfig;
	SPI_ClockSpeed SPI_SclkSpeed;
	SPI_DFF_Modes SPI_DFF;
	SPI_CPOL_MODES SPI_CPOL;
	SPI_CPHA_MODES SPI_CPHA;
	SPI_SSM_MODES SPI_SSM;
} SPI_Pin_Config;

typedef struct {
	SPI_RegDef *pSPIx;
	SPI_Pin_Config SPI_Config;
	uint8_t *pTxBuffer;
	uint8_t *pRxBuffer;
	uint32_t TxLen;
	uint32_t RxLen;
	SPI_STATES TxState;
	SPI_STATES RxState;
} SPI_Handle;

void SPI_Init(SPI_Handle *pHandle);
void SPI_DeInit(const SPI_RegDef *const pSPIx);

void SPI_SendData(SPI_RegDef *pSPIx, uint8_t *pTxBuffer, uint32_t Len);
void SPI_ReceiveData(SPI_RegDef *pSPIx, uint8_t *pRxBuffer, uint32_t Len);
SPI_STATES SPI_SendDataIT(SPI_Handle *pHandle, uint8_t *pTxBuffer, uint32_t Len);
SPI_STATES SPI_ReceiveDataIT(SPI_Handle *pHandle, uint8_t *pRxBuffer,
		uint32_t Len);

void SPi_IRQ_Interrupt_Config(uint8_t IRQ_Number, uint8_t State);
void SPI_IRQ_Priority_Config(uint8_t IRQ_Number, uint32_t IRQ_Priority);
void SPI_IRQHandling(SPI_Handle *pHandle);

void SPI_SSI_Config(SPI_RegDef *pSPIx, uint8_t State);
void SPI_Peripheral_Control(SPI_RegDef *pSPIx, uint8_t State);

void SPI_DummyWrite(SPI_RegDef *pSPIx);
void SPI_DummyRead(SPI_RegDef *pSPIx);
uint8_t SPI_VerifyResponse(uint8_t byte);

void SPI_ClearOVR(SPI_RegDef *pSPIx);
void SPI_CloseTransmission(SPI_Handle *pHandle);
void SPI_CloseReception(SPI_Handle *pHandle);

void SPI_ApplicationEventCallback(SPI_Handle *pHandle, SPI_EVENTS AppEvent);

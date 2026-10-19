#include <stdint.h>
#include <stddef.h>
#include "stm32f411x_spi_driver.h"

static void SPI_PeriClockControl(const SPI_RegDef *const pSPIx, uint8_t State);

//static uint8_t SPI_GetFlagStatus(const SPI_RegDef *const pSPIx,
//		uint32_t flag_name);

static void SPI_TXE_InterruptHandle(SPI_Handle *pHandle);
static void SPI_RXNE_InterruptHandle(SPI_Handle *pHandle);
static void SPI_ERRIE_OVR_InterruptHandle(SPI_Handle *pHandle);

void SPI_Init(SPI_Handle *pHandle)
{
	SPI_PeriClockControl(pHandle->pSPIx, ENABLE);

	uint32_t SPI_CR1_MOCK_REG = 0U;

	SPI_CR1_MOCK_REG |= (pHandle->SPI_Config.SPI_DeviceMode
			<< SPI_CR1_MSTR_SHIFT);

	if (pHandle->SPI_Config.SPI_BusConfig == SPI_BUS_CONFIG_FULL_DUPLEX) {
		SPI_CR1_MOCK_REG &= ~(1U << SPI_CR1_BIDI_MODE_SHIFT);
	} else if (pHandle->SPI_Config.SPI_BusConfig
			== SPI_BUS_CONFIG_HALF_DUPLEX) {
		SPI_CR1_MOCK_REG |= (1U << SPI_CR1_BIDI_MODE_SHIFT);
	} else if (pHandle->SPI_Config.SPI_BusConfig
			== SPI_BUS_CONFIG_SIMPLEX_RXONLY) {
		SPI_CR1_MOCK_REG &= ~(1U << SPI_CR1_BIDI_MODE_SHIFT);
		SPI_CR1_MOCK_REG |= (1U << SPI_CR1_RX_ONLY_SHIFT);
	}

	SPI_CR1_MOCK_REG |= (pHandle->SPI_Config.SPI_SclkSpeed
			<< SPI_CR1_BR_SHIFT);
	SPI_CR1_MOCK_REG |= (pHandle->SPI_Config.SPI_DFF << SPI_CR1_DFF_SHIFT);
	SPI_CR1_MOCK_REG |=
			(pHandle->SPI_Config.SPI_CPOL << SPI_CR1_CPOL_SHIFT);
	SPI_CR1_MOCK_REG |=
			(pHandle->SPI_Config.SPI_CPHA << SPI_CR1_CPHA_SHIFT);
	SPI_CR1_MOCK_REG |= (pHandle->SPI_Config.SPI_SSM << SPI_CR1_SSM_SHIFT);

	pHandle->pSPIx->SPI_CR1 = SPI_CR1_MOCK_REG;

	return;
}

void SPI_DeInit(const SPI_RegDef *const pSPIx)
{
	if (pSPIx == SPI1)
		SPI1_REG_RESET();
	else if (pSPIx == SPI2)
		SPI2_REG_RESET();
	else if (pSPIx == SPI3)
		SPI3_REG_RESET();
	else if (pSPIx == SPI4)
		SPI4_REG_RESET();
	else if (pSPIx == SPI5)
		SPI5_REG_RESET();

	return;
}

static void SPI_PeriClockControl(const SPI_RegDef *const pSPIx, uint8_t State)
{
	if (State == ENABLE) {
		if (pSPIx == SPI1)
			SPI1_PERI_CLCK_ENABLE();
		else if (pSPIx == SPI2)
			SPI2_PERI_CLCK_ENABLE();
		else if (pSPIx == SPI3)
			SPI3_PERI_CLCK_ENABLE();
		else if (pSPIx == SPI4)
			SPI4_PERI_CLCK_ENABLE();
		else if (pSPIx == SPI5)
			SPI5_PERI_CLCK_ENABLE();
	} else {
		if (pSPIx == SPI1)
			SPI1_PERI_CLCK_DISABLE();
		else if (pSPIx == SPI2)
			SPI2_PERI_CLCK_DISABLE();
		else if (pSPIx == SPI3)
			SPI3_PERI_CLCK_DISABLE();
		else if (pSPIx == SPI4)
			SPI4_PERI_CLCK_DISABLE();
		else if (pSPIx == SPI5)
			SPI5_PERI_CLCK_DISABLE();
	}

	return;
}

//static uint8_t SPI_GetFlagStatus(const SPI_RegDef *const pSPIx,
//		uint32_t flag_name)
//{
//	if (pSPIx->SPI_SR & flag_name) {
//		return FLAG_SET;
//	}
//
//	return FLAG_RESET;
//}

void SPI_SendData(SPI_RegDef *pSPIx, uint8_t *pTxBuffer, uint32_t Len)
{
	while (Len > 0) {
		while (!(pSPIx->SPI_SR & (1 << SPI_SR_TXE_SHIFT)))
			;

		if (pSPIx->SPI_CR1 & (1 << SPI_CR1_DFF_SHIFT)) {
			uint16_t data = (uint16_t) pTxBuffer[0]
					| ((uint16_t) pTxBuffer[1] << 8);
			pSPIx->SPI_DR = data;

			Len--;
			Len--;
			pTxBuffer++;
			pTxBuffer++;
		} else {
			pSPIx->SPI_DR = *pTxBuffer;
			Len--;
			pTxBuffer++;
		}
	}

	return;
}

void SPI_ReceiveData(SPI_RegDef *pSPIx, uint8_t *pRxBuffer, uint32_t Len)
{
	while (Len > 0) {
		while (!(pSPIx->SPI_SR & (1U << SPI_SR_RXNE_SHIFT)))
			;

		if (pSPIx->SPI_CR1 & (1U << SPI_CR1_DFF_SHIFT)) {
			uint16_t data = (uint16_t) pSPIx->SPI_DR;
			*pRxBuffer = (uint8_t) (data & 0xFF);
			pRxBuffer++;
			*pRxBuffer = (uint8_t) ((data >> 8) & 0xFF);
			pRxBuffer++;
			Len -= 2;
		} else {
			*pRxBuffer = (uint8_t) pSPIx->SPI_DR;
			Len--;
			pRxBuffer++;
		}
	}

	return;
}

SPI_STATES SPI_SendDataIT(SPI_Handle *pHandle, uint8_t *pTxBuffer, uint32_t Len)
{
	const SPI_STATES State = pHandle->TxState;

	if (State == SPI_BUSY_RX) {
		return State;
	}

	pHandle->pTxBuffer = pTxBuffer;
	pHandle->TxLen = Len;
	pHandle->TxState = SPI_BUSY_TX;
	pHandle->pSPIx->SPI_CR2 = (1U << SPI_CR2_TXEIE_SHIFT);

	return SPI_BUSY_TX;
}

SPI_STATES SPI_ReceiveDataIT(SPI_Handle *pHandle, uint8_t *pRxBuffer,
		uint32_t Len)
{
	const SPI_STATES State = pHandle->RxState;

	if (State == SPI_BUSY_TX) {
		return State;
	}

	pHandle->pRxBuffer = pRxBuffer;
	pHandle->RxLen = Len;
	pHandle->RxState = SPI_BUSY_RX;
	pHandle->pSPIx->SPI_CR2 = (1U << SPI_CR2_RXNEIE_SHIFT);

	return SPI_BUSY_RX;
}

void SPI_DummyWrite(SPI_RegDef *pSPIx)
{
	uint8_t dummy_byte = 0;

	SPI_SendData(pSPIx, &dummy_byte, 1);

	return;
}

void SPI_DummyRead(SPI_RegDef *pSPIx)
{
	uint8_t dummy_buffer = 0;

	SPI_SendData(pSPIx, &dummy_buffer, 1);

	return;
}

uint8_t SPI_VerifyResponse(uint8_t byte)
{
	const uint8_t AckByteSignature = 0xF5;

	if (byte == AckByteSignature) {
		return 1;
	}

	return 0;
}

void SPi_IRQ_Interrupt_Config(uint8_t IRQ_Number, uint8_t State)
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

void SPI_IRQ_Priority_Config(uint8_t IRQ_Number, uint32_t IRQ_Priority)
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

void SPI_IRQHandling(SPI_Handle *pHandle)
{
	const uint8_t TXE = pHandle->pSPIx->SPI_SR & (1 << SPI_SR_TXE_SHIFT);
	const uint8_t TXEIE = pHandle->pSPIx->SPI_CR2
			& (1 << SPI_CR2_TXEIE_SHIFT);
	const uint8_t RXNE = pHandle->pSPIx->SPI_SR & (1 << SPI_SR_RXNE_SHIFT);
	const uint8_t RXNEIE = pHandle->pSPIx->SPI_CR2
			& (1 << SPI_CR2_RXNEIE_SHIFT);
	const uint8_t OVR = pHandle->pSPIx->SPI_SR & (1 << SPI_SR_OVR_SHIFT);
	const uint8_t ERRIE = pHandle->pSPIx->SPI_CR2
			& (1 << SPI_CR2_ERRIE_SHIFT);

	if (TXE && TXEIE) {
		SPI_TXE_InterruptHandle(pHandle);
	} else if (RXNE && RXNEIE) {
		SPI_RXNE_InterruptHandle(pHandle);
	} else if (OVR && ERRIE) {
		SPI_ERRIE_OVR_InterruptHandle(pHandle);
	}

	return;
}

static void SPI_TXE_InterruptHandle(SPI_Handle *pHandle)
{
	if (pHandle->pSPIx->SPI_CR1 & (1 << SPI_CR1_DFF_SHIFT)) {
		uint16_t data = (uint16_t) pHandle->pTxBuffer[0]
				| ((uint16_t) pHandle->pTxBuffer[1] << 8);
		pHandle->pSPIx->SPI_DR = data;

		pHandle->TxLen--;
		pHandle->TxLen--;
		pHandle->pTxBuffer++;
		pHandle->pTxBuffer++;
	} else {
		pHandle->pSPIx->SPI_DR = *(pHandle->pTxBuffer);
		pHandle->TxLen--;
		pHandle->pTxBuffer++;
	}

	if (!pHandle->TxLen) {
		SPI_CloseTransmission(pHandle);

		SPI_ApplicationEventCallback(pHandle, SPI_EVENT_TX_CMPLT);
	}
}

static void SPI_RXNE_InterruptHandle(SPI_Handle *pHandle)
{
	if (pHandle->pSPIx->SPI_CR1 & (1 << SPI_CR1_DFF_SHIFT)) {
		uint16_t data = (uint16_t) pHandle->pSPIx->SPI_DR;

		*pHandle->pRxBuffer = (uint8_t) (data & 0xFF);
		pHandle->pRxBuffer++;
		*pHandle->pRxBuffer = (uint8_t) ((data >> 8) & 0xFF);
		pHandle->pRxBuffer++;

		pHandle->RxLen--;
		pHandle->RxLen--;
	} else {
		*(pHandle->pRxBuffer) = (uint8_t) pHandle->pSPIx->SPI_DR;
		pHandle->RxLen--;
		pHandle->pRxBuffer++;
	}

	if (!pHandle->RxLen) {
		SPI_CloseReception(pHandle);
		SPI_ApplicationEventCallback(pHandle, SPI_EVENT_RX_CMPLT);
	}
}

static void SPI_ERRIE_OVR_InterruptHandle(SPI_Handle *pHandle)
{

	if (pHandle->TxState != SPI_BUSY_TX) {
		SPI_ClearOVR(pHandle->pSPIx);
	}

	SPI_ApplicationEventCallback(pHandle, SPI_EVENT_OVR_ERR);

	return;
}

void SPI_ClearOVR(SPI_RegDef *pSPIx)
{
	uint32_t temp;

	temp = pSPIx->SPI_DR;
	temp = pSPIx->SPI_SR;

	(void) temp;

	return;
}

void SPI_CloseTransmission(SPI_Handle *pHandle)
{
	pHandle->pSPIx->SPI_CR2 &= ~(1U << SPI_CR2_TXEIE_SHIFT);
	pHandle->pTxBuffer = NULL;
	pHandle->TxLen = 0U;
	pHandle->TxState = SPI_READY;

	return;
}

void SPI_CloseReception(SPI_Handle *pHandle)
{
	pHandle->pSPIx->SPI_CR2 &= ~(1U << SPI_CR2_RXNEIE_SHIFT);
	pHandle->pRxBuffer = NULL;
	pHandle->RxLen = 0U;
	pHandle->RxState = SPI_READY;

	return;
}

void SPI_SSI_Config(SPI_RegDef *pSPIx, uint8_t State)
{
	if (State == ENABLE) {
		pSPIx->SPI_CR1 |= (1U << SPI_CR1_SSI_SHIFT);
	} else {
		pSPIx->SPI_CR1 &= ~(1U << SPI_CR1_SSI_SHIFT);
	}

	return;
}

void SPI_Peripheral_Control(SPI_RegDef *pSPIx, uint8_t State)
{
	if (State == ENABLE) {
		pSPIx->SPI_CR1 |= (1U << SPI_CR1_SPE_SHIFT);
	} else {
		pSPIx->SPI_CR1 &= ~(1U << SPI_CR1_SPE_SHIFT);
	}

	return;
}

__attribute__((weak)) void SPI_ApplicationEventCallback(
		SPI_Handle *pHandle, SPI_EVENTS AppEvent)
{
	(void) pHandle;
	(void) AppEvent;

	return;
}

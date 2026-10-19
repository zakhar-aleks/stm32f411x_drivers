#include <stddef.h>
#include "stm32f411x_usart_driver.h"

static uint32_t RCC_GetPCLK1Value(void);
static uint32_t RCC_GetPCLK2Value(void);
static void USART_Enable_TXEIE(USART_RegDef *pUSARTx);
static void USART_Enable_TCIE(USART_RegDef *pUSARTx);
static void USART_Enable_RXNEIE(USART_RegDef *pUSARTx);

static uint32_t RCC_GetPCLK1Value(void)
{
	uint16_t ahb_pre_scaler[8] = { 2U, 4U, 8U, 16U, 64U, 128U, 256U, 512U };
	uint8_t apb1_pre_scaler[4] = { 2U, 4U, 8U, 16U };

	uint32_t pclk1 = 0U;
	uint32_t systemclk = 0U;
	uint32_t ahbp = 0U;
	uint32_t apb1p = 0U;
	Clock_Sources clock_source =
			(Clock_Sources) ((RCC->RCC_CFGR >> 2) & 0x3);

	if (clock_source == HSI) {
		systemclk = 16000000U;
	} else if (clock_source == HSE) {
		systemclk = 8000000U;
	} else if (clock_source == PLL) {
	}

	uint32_t temp = ((RCC->RCC_CFGR >> 4) & 0xF);

	if (temp < 8U) {
		ahbp = 1U;
	} else {
		ahbp = ahb_pre_scaler[temp - 8U];
	}

	temp = ((RCC->RCC_CFGR >> 10) & 0x7);

	if (temp < 4U) {
		apb1p = 1U;
	} else {
		apb1p = apb1_pre_scaler[temp - 4U];
	}
	pclk1 = (systemclk / ahbp) / apb1p;

	return pclk1;
}

static uint32_t RCC_GetPCLK2Value(void)
{
	uint16_t ahb_pre_scaler[8] = { 2U, 4U, 8U, 16U, 64U, 128U, 256U, 512U };
	uint8_t apb2_pre_scaler[4] = { 2U, 4U, 8U, 16U };

	uint32_t pclk2 = 0U;
	uint32_t systemclk = 0U;
	uint32_t ahbp = 0U;
	uint32_t apb2p = 0U;
	Clock_Sources clock_source =
			(Clock_Sources) ((RCC->RCC_CFGR >> 2) & 0x3);

	if (clock_source == HSI) {
		systemclk = 16000000U;
	} else if (clock_source == HSE) {
		systemclk = 8000000U;
	} else if (clock_source == PLL) {
	}

	uint32_t temp = ((RCC->RCC_CFGR >> 4) & 0xF);

	if (temp < 8U) {
		ahbp = 1U;
	} else {
		ahbp = ahb_pre_scaler[temp - 8U];
	}

	temp = ((RCC->RCC_CFGR >> 13) & 0x7);

	if (temp < 4U) {
		apb2p = 1U;
	} else {
		apb2p = apb2_pre_scaler[temp - 4U];
	}
	pclk2 = (systemclk / ahbp) / apb2p;

	return pclk2;
}

static void USART_Enable_TXEIE(USART_RegDef *pUSARTx)
{
	pUSARTx->USART_CR1 |= (1U << USART_CR1_TXEIE);

	return;
}

static void USART_Enable_TCIE(USART_RegDef *pUSARTx)
{
	pUSARTx->USART_CR1 |= (1U << USART_CR1_TCIE);

	return;
}

static void USART_Enable_RXNEIE(USART_RegDef *pUSARTx)
{
	pUSARTx->USART_CR1 |= (1U << USART_CR1_RXNEIE);

	return;
}

void USART_Clear_Error_Flags(USART_RegDef *pUSARTx)
{
	uint32_t sr_read = pUSARTx->USART_SR;
	uint32_t dr_read = pUSARTx->USART_DR;

	(void) sr_read;
	(void) dr_read;

	return;
}

void USART_PeriClockControl(USART_RegDef *pUSARTx, uint8_t State)
{
	if (State == ENABLE) {
		if (pUSARTx == USART1) {
			USART1_PERI_CLCK_ENABLE();
		} else if (pUSARTx == USART2) {
			USART2_PERI_CLCK_ENABLE();
		} else if (pUSARTx == USART6) {
			USART6_PERI_CLCK_ENABLE();
		}
	} else {
		if (pUSARTx == USART1) {
			USART1_PERI_CLCK_DISABLE();
		} else if (pUSARTx == USART2) {
			USART2_PERI_CLCK_DISABLE();
		} else if (pUSARTx == USART6) {
			USART6_PERI_CLCK_DISABLE();
		}
	}

	return;
}

void USART_PeriControl(USART_RegDef *pUSARTx, uint8_t State)
{
	if (State == ENABLE) {
		pUSARTx->USART_CR1 |= (1U << USART_CR1_UE);
	} else {
		pUSARTx->USART_CR1 &= ~(1U << USART_CR1_UE);
	}

	return;
}

void USART_Init(USART_Handle *pHandle)
{
	uint32_t tempreg = 0;

	USART_PeriClockControl(pHandle->pUSARTx, ENABLE);

	if (pHandle->USART_Config.USART_Mode == USART_MODE_ONLY_RX) {
		tempreg |= (1U << USART_CR1_RE);
	} else if (pHandle->USART_Config.USART_Mode == USART_MODE_ONLY_TX) {
		tempreg |= (1U << USART_CR1_TE);

	} else if (pHandle->USART_Config.USART_Mode == USART_MODE_TXRX) {
		tempreg |= ((1U << USART_CR1_TE) | (1U << USART_CR1_RE));
	}

	tempreg |= pHandle->USART_Config.USART_WordLength << USART_CR1_M;

	if (pHandle->USART_Config.USART_ParityControl == USART_PARITY_EN_EVEN) {
		tempreg |= (1U << USART_CR1_PCE);

	} else if (pHandle->USART_Config.USART_ParityControl
			== USART_PARITY_EN_ODD) {
		tempreg |= (1U << USART_CR1_PCE);
		tempreg |= (1U << USART_CR1_PS);
	}

	pHandle->pUSARTx->USART_CR1 = tempreg;

	tempreg = 0;

	tempreg |= pHandle->USART_Config.USART_NoOfStopBits << USART_CR2_STOP;

	pHandle->pUSARTx->USART_CR2 = tempreg;

	tempreg = 0;

	if (pHandle->USART_Config.USART_HWFlowControl
			== USART_HW_FLOW_CTRL_CTS) {
		tempreg |= (1U << USART_CR3_CTSE);

	} else if (pHandle->USART_Config.USART_HWFlowControl
			== USART_HW_FLOW_CTRL_RTS) {
		tempreg |= (1U << USART_CR3_RTSE);

	} else if (pHandle->USART_Config.USART_HWFlowControl
			== USART_HW_FLOW_CTRL_CTS_RTS) {
		tempreg |= (1U << USART_CR3_CTSE);
		tempreg |= (1U << USART_CR3_RTSE);
	}

	pHandle->pUSARTx->USART_CR3 = tempreg;

	USART_SetBaudRate(pHandle->pUSARTx, pHandle->USART_Config.USART_Baud);

	return;

}

void USART_DeInit(USART_RegDef *pUSARTx)
{
	if (pUSARTx == USART1) {
		USART1_REG_RESET();
	} else if (pUSARTx == USART2) {
		USART2_REG_RESET();
	} else if (pUSARTx == USART6) {
		USART6_REG_RESET();
	}

	return;
}

void USART_SendData(USART_Handle *pHandle, uint8_t *pTxBuffer, uint32_t Len)
{
	for (uint32_t i = 0; i < Len; i++) {
		while (!USART_GetFlagStatus(pHandle->pUSARTx, USART_FLAG_TXE))
			;

		if (pHandle->USART_Config.USART_WordLength
				== USART_WORDLEN_9BITS) {
			uint16_t data = (uint16_t) ((pTxBuffer[1] << 8)
					| pTxBuffer[0]);
			pHandle->pUSARTx->USART_DR = (data & 0x01FF);

			if (pHandle->USART_Config.USART_ParityControl
					== USART_PARITY_DISABLE) {
				pTxBuffer++;
				pTxBuffer++;
			} else {
				pTxBuffer++;
			}
		} else {
			pHandle->pUSARTx->USART_DR = (*pTxBuffer
					& (uint8_t) 0xFF);

			pTxBuffer++;
		}
	}

	while (!USART_GetFlagStatus(pHandle->pUSARTx, USART_FLAG_TC))
		;

	return;
}

void USART_ReceiveData(USART_Handle *pHandle, uint8_t *pRxBuffer, uint32_t Len)
{
	for (uint32_t i = 0; i < Len; i++) {
		while (!USART_GetFlagStatus(pHandle->pUSARTx, USART_FLAG_RXNE))
			;

		if (pHandle->USART_Config.USART_WordLength
				== USART_WORDLEN_9BITS) {
			if (pHandle->USART_Config.USART_ParityControl
					== USART_PARITY_DISABLE) {
				uint16_t data = (pHandle->pUSARTx->USART_DR
						& 0x01FF);
				pRxBuffer[0] = (uint8_t) (data & 0xFF);
				pRxBuffer[1] = (uint8_t) ((data >> 8) & 0xFF);

				pRxBuffer++;
				pRxBuffer++;
			} else {
				*pRxBuffer = (pHandle->pUSARTx->USART_DR
						& (uint8_t) 0xFF);

				pRxBuffer++;
			}
		} else {
			if (pHandle->USART_Config.USART_ParityControl
					== USART_PARITY_DISABLE) {
				*pRxBuffer =
						(uint8_t) (pHandle->pUSARTx->USART_DR
								& 0xFF);
			} else {
				*pRxBuffer =
						(uint8_t) (pHandle->pUSARTx->USART_DR
								& 0x7F);
			}

			pRxBuffer++;
		}
	}

	return;
}

uint8_t USART_SendDataIT(USART_Handle *pHandle, uint8_t *pTxBuffer,
		uint32_t Len)
{
	uint8_t txstate = pHandle->TxBusyState;

	if (txstate != USART_BUSY_TX) {
		pHandle->TxLen = Len;
		pHandle->pTxBuffer = pTxBuffer;
		pHandle->TxBusyState = USART_BUSY_TX;

		USART_Enable_TXEIE(pHandle->pUSARTx);
		USART_Enable_TCIE(pHandle->pUSARTx);
	}

	return txstate;
}

uint8_t USART_ReceiveDataIT(USART_Handle *pHandle, uint8_t *pRxBuffer,
		uint32_t Len)
{
	uint8_t rxstate = pHandle->RxBusyState;

	if (rxstate != USART_BUSY_RX) {
		pHandle->RxLen = Len;
		pHandle->pRxBuffer = pRxBuffer;
		pHandle->RxBusyState = USART_BUSY_RX;

		USART_Enable_RXNEIE(pHandle->pUSARTx);
	}

	return rxstate;
}

void USART_SetBaudRate(USART_RegDef *pUSARTx, uint32_t BaudRate)
{
	uint32_t PCLKx;
	uint32_t usartdiv;
	uint32_t M_part, F_part;

	uint32_t tempreg = 0;

	if (pUSARTx == USART1 || pUSARTx == USART6) {
		PCLKx = RCC_GetPCLK2Value();
	} else {
		PCLKx = RCC_GetPCLK1Value();
	}

	if (pUSARTx->USART_CR1 & (1U << USART_CR1_OVER8)) {
		usartdiv = ((25 * PCLKx) / (2 * BaudRate));
	} else {
		usartdiv = ((25 * PCLKx) / (4 * BaudRate));
	}

	M_part = usartdiv / 100;

	tempreg |= M_part << 4;

	F_part = (usartdiv - (M_part * 100));

	if (pUSARTx->USART_CR1 & (1U << USART_CR1_OVER8)) {
		F_part = (((F_part * 8) + 50) / 100) & ((uint8_t) 0x07);
	} else {
		F_part = (((F_part * 16) + 50) / 100) & ((uint8_t) 0x0F);
	}

	tempreg |= F_part;

	pUSARTx->USART_BRR = tempreg;

	return;
}

uint8_t USART_GetFlagStatus(USART_RegDef *pUSARTx, uint32_t StatusFlagName)
{
	if (pUSARTx->USART_SR & StatusFlagName) {
		return FLAG_SET;
	}

	return FLAG_RESET;
}

void USART_ClearFlag(USART_RegDef *pUSARTx, uint32_t StatusFlagName)
{
	pUSARTx->USART_SR &= ~(1U << StatusFlagName);

	return;
}

void USART_IRQ_Interrupt_Config(uint8_t IRQ_Number, uint8_t State)
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

void USART_IRQ_Priority_Config(uint8_t IRQ_Number, uint32_t IRQ_Priority)
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

void USART_IRQHandling(USART_Handle *pHandle)
{

	uint32_t temp1, temp2, temp3;

	temp1 = pHandle->pUSARTx->USART_SR & (USART_FLAG_TC);
	temp2 = pHandle->pUSARTx->USART_CR1 & (1U << USART_CR1_TCIE);

	if (temp1 && temp2) {
		if (pHandle->TxBusyState == USART_BUSY_TX) {
			if (!(pHandle->TxLen)) {
				USART_ClearFlag(pHandle->pUSARTx,
				USART_FLAG_TC);
				pHandle->TxBusyState = USART_READY;

				pHandle->pTxBuffer = NULL;
				pHandle->TxLen = 0;

				USART_ApplicationEventCallback(pHandle,
						USART_EVENT_TX_CMPLT);
			}
		}
	}

	temp1 = pHandle->pUSARTx->USART_SR & (USART_FLAG_TXE);
	temp2 = pHandle->pUSARTx->USART_CR1 & (1U << USART_CR1_TXEIE);

	if (temp1 && temp2) {
		if (pHandle->TxBusyState == USART_BUSY_TX) {
			if (pHandle->TxLen > 0) {
				if (pHandle->USART_Config.USART_WordLength
						== USART_WORDLEN_9BITS) {
					uint16_t data =
							(uint16_t) ((pHandle->pTxBuffer[1]
									<< 8)
									| pHandle->pTxBuffer[0]);
					pHandle->pUSARTx->USART_DR = (data
							& 0x01FF);

					if (pHandle->USART_Config.USART_ParityControl
							== USART_PARITY_DISABLE) {
						pHandle->pTxBuffer++;
						pHandle->pTxBuffer++;
						pHandle->TxLen--;
						pHandle->TxLen--;
					} else {
						pHandle->pTxBuffer++;
						pHandle->TxLen--;
					}
				} else {
					pHandle->pUSARTx->USART_DR =
							(*pHandle->pTxBuffer
									& (uint8_t) 0xFF);
					pHandle->pTxBuffer++;
					pHandle->TxLen--;
				}

			}
			if (pHandle->TxLen == 0) {

				pHandle->pUSARTx->USART_CR1 &= ~(1U
						<< USART_CR1_TXEIE);
			}
		}
	}

	temp1 = pHandle->pUSARTx->USART_SR & (USART_FLAG_RXNE);
	temp2 = pHandle->pUSARTx->USART_CR1 & (1U << USART_CR1_RXNEIE);

	if (temp1 && temp2) {
		if (pHandle->RxBusyState == USART_BUSY_RX) {
			if (pHandle->RxLen > 0) {
				if (pHandle->USART_Config.USART_WordLength
						== USART_WORDLEN_9BITS) {
					if (pHandle->USART_Config.USART_ParityControl
							== USART_PARITY_DISABLE) {
						uint16_t data =
								(pHandle->pUSARTx->USART_DR
										& 0x01FF);
						pHandle->pRxBuffer[0] =
								(uint8_t) (data
										& 0xFF);
						pHandle->pRxBuffer[1] =
								(uint8_t) ((data
										>> 8)
										& 0xFF);

						pHandle->pRxBuffer++;
						pHandle->pRxBuffer++;
						pHandle->RxLen--;
						pHandle->RxLen--;
					} else {
						*(pHandle->pRxBuffer) =
								(pHandle->pUSARTx->USART_DR
										& (uint8_t) 0xFF);

						pHandle->pRxBuffer++;
						pHandle->RxLen--;
					}
				} else {
					if (pHandle->USART_Config.USART_ParityControl
							== USART_PARITY_DISABLE) {
						*(pHandle->pRxBuffer) =
								(uint8_t) (pHandle->pUSARTx->USART_DR
										& (uint8_t) 0xFF);
					}

					else {
						*(pHandle->pRxBuffer) =
								(uint8_t) (pHandle->pUSARTx->USART_DR
										& (uint8_t) 0x7F);

					}

					pHandle->pRxBuffer++;
					pHandle->RxLen--;
				}

			}

			if (!pHandle->RxLen) {
				pHandle->pUSARTx->USART_CR1 &= ~(1U
						<< USART_CR1_RXNEIE);
				pHandle->RxBusyState = USART_READY;
				USART_ApplicationEventCallback(pHandle,
						USART_EVENT_RX_CMPLT);
			}
		}
	}

	temp1 = pHandle->pUSARTx->USART_SR & (USART_FLAG_CTS);
	temp2 = pHandle->pUSARTx->USART_CR3 & (1U << USART_CR3_CTSE);
	temp3 = pHandle->pUSARTx->USART_CR3 & (1U << USART_CR3_CTSIE);

	if (temp1 && temp2 && temp3) {
		USART_ClearFlag(pHandle->pUSARTx, USART_FLAG_CTS);
		USART_ApplicationEventCallback(pHandle, USART_EVENT_CTS);
	}

	temp1 = pHandle->pUSARTx->USART_SR & (USART_FLAG_IDLE);
	temp2 = pHandle->pUSARTx->USART_CR3 & (1U << USART_CR1_IDLEIE);

	if (temp1 && temp2) {
		USART_ClearFlag(pHandle->pUSARTx, USART_FLAG_IDLE);
		USART_ApplicationEventCallback(pHandle, USART_EVENT_IDLE);
	}

	temp1 = pHandle->pUSARTx->USART_SR & (USART_FLAG_ORE);
	temp2 = pHandle->pUSARTx->USART_CR1 & (1U << USART_CR1_RXNEIE);

	if (temp1 && temp2) {
		USART_ApplicationEventCallback(pHandle, USART_EVENT_ORE);
	}

	temp2 = pHandle->pUSARTx->USART_CR3 & (1U << USART_CR3_EIE);

	if (temp2) {
		temp1 = pHandle->pUSARTx->USART_SR;
		if (temp1 & (USART_FLAG_FE)) {
			USART_ApplicationEventCallback(pHandle,
					USART_ERREVENT_FE);
		}

		if (temp1 & (USART_FLAG_NF)) {
			USART_ApplicationEventCallback(pHandle,
					USART_ERREVENT_NE);
		}

		if (temp1 & (USART_FLAG_ORE)) {
			USART_ApplicationEventCallback(pHandle,
					USART_ERREVENT_ORE);
		}
	}

	return;
}

__attribute__((weak)) void USART_ApplicationEventCallback(
		USART_Handle *pHandle, USART_AppEvent AppEvent)
{
	(void) pHandle;
	(void) AppEvent;

	return;
}

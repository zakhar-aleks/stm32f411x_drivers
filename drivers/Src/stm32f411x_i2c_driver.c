#include "stm32f411x_i2c_driver.h"

static void I2C_GenerateStartCondition(I2C_RegDef *pI2Cx);
static void I2C_ExecuteAdressPhase(I2C_RegDef *pI2Cx, uint8_t SlaveAddr,
		I2C_ReadWriteState State);
static void I2C_ClearADDRFlag(I2C_Handle *pHandle);
static void I2C_MasterHandleRxNEIT(I2C_Handle *pHandle);
static void I2C_MasterHandleTxEIT(I2C_Handle *pHandle);
static void I2C_CloseTransmision(I2C_Handle *pHandle);
static void I2C_CloseReception(I2C_Handle *pHandle);

static void I2C_GenerateStartCondition(I2C_RegDef *pI2Cx)
{
	pI2Cx->I2C_CR1 |= (1U << I2C_CR1_START);

	return;
}

static void I2C_GenerateStopCondition(I2C_RegDef *pI2Cx)
{
	pI2Cx->I2C_CR1 |= (1U << I2C_CR1_STOP);

	return;
}

static void I2C_ExecuteAdressPhase(I2C_RegDef *pI2Cx, uint8_t SlaveAddr,
		I2C_ReadWriteState State)
{
	SlaveAddr = SlaveAddr << 1;

	if (State == I2C_WRITE_STATE) {
		SlaveAddr &= (uint8_t) (~((1U << 0)));
	} else {
		SlaveAddr |= (uint8_t) (1U << 0);
	}

	pI2Cx->I2C_DR = SlaveAddr;

	return;
}

static void I2C_ClearADDRFlag(I2C_Handle *pHandle)
{
	if (pHandle->pI2Cx->I2C_SR2 & (1U << I2C_SR2_MSL)) {
		if (pHandle->TxRxState == I2C_BUSY_RX && pHandle->RxLen == 1) {
			I2C_ManageACK(pHandle->pI2Cx, I2C_ACK_DISABLE);
		}
	}

	uint32_t primask = __get_PRIMASK();
	__disable_irq();

	uint32_t dummyRead = pHandle->pI2Cx->I2C_SR1;
	dummyRead = pHandle->pI2Cx->I2C_SR2;
	(void) dummyRead;

	__set_PRIMASK(primask);

	return;
}

static void I2C_CloseTransmision(I2C_Handle *pHandle)
{
	pHandle->pI2Cx->I2C_CR2 &= ~(1U << I2C_CR2_ITBUFEN);
	pHandle->pI2Cx->I2C_CR2 &= ~(1U << I2C_CR2_ITEVTEN);

	pHandle->TxRxState = I2C_READY;
	pHandle->pTxBuffer = NULL;
	pHandle->TxLen = 0;

	return;
}

static void I2C_CloseReception(I2C_Handle *pHandle)
{
	pHandle->pI2Cx->I2C_CR2 &= ~(1U << I2C_CR2_ITBUFEN);
	pHandle->pI2Cx->I2C_CR2 &= ~(1U << I2C_CR2_ITEVTEN);

	pHandle->TxRxState = I2C_READY;
	pHandle->pRxBuffer = NULL;
	pHandle->RxSize = 0;
	pHandle->RxLen = 0;

	I2C_ManageACK(pHandle->pI2Cx, I2C_ACK_ENABLE);

	return;
}

static void I2C_MasterHandleTxEIT(I2C_Handle *pHandle)
{
	if (pHandle->TxRxState == I2C_BUSY_TX) {
		if (pHandle->TxLen > 0) {
			pHandle->pI2Cx->I2C_DR = *(pHandle->pRxBuffer);
			pHandle->TxLen--;
			pHandle->pTxBuffer++;
		}
	}
}

static void I2C_MasterHandleRxNEIT(I2C_Handle *pHandle)
{
	if (pHandle->RxLen == 1) {
		*pHandle->pRxBuffer = (uint8_t) pHandle->pI2Cx->I2C_DR;
		pHandle->RxLen--;
	} else if (pHandle->RxLen > 1) {
		if (pHandle->RxLen == 2) {
			I2C_ManageACK(pHandle->pI2Cx, I2C_ACK_DISABLE);

		}

		*pHandle->pRxBuffer = (uint8_t) (pHandle->pI2Cx->I2C_DR);
		pHandle->RxLen--;
		pHandle->pRxBuffer++;
	}

	if (pHandle->RxLen == 0) {
		if (pHandle->RS == I2C_DISABLE_RS) {
			I2C_GenerateStopCondition(pHandle->pI2Cx);
		}

		I2C_CloseReception(pHandle);
		I2C_ApplicationEventCallback(pHandle, I2C_EV_RX_CMPLT);
	}
}

static uint32_t RCC_GetPCLK1Value(void)
{
	typedef enum {
		HSI = 0U, HSE = 1U, PLL = 2U,
	} Clock_Sources;

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

void I2C_ManageACK(I2C_RegDef *pI2Cx, I2C_ACKControl State)
{
	if (State == I2C_ACK_ENABLE) {
		pI2Cx->I2C_CR1 |= (1U << I2C_CR1_ACK);
	} else {
		pI2Cx->I2C_CR1 &= ~(1U << I2C_CR1_ACK);
	}

	return;
}

uint8_t I2C_GetFlagStatus(I2C_RegDef *pI2Cx, uint32_t FlagName)
{
	if (pI2Cx->I2C_SR1 & FlagName) {
		return FLAG_SET;
	}

	return FLAG_RESET;
}

void I2C_PeriClockControl(const I2C_RegDef *const pI2Cx, uint8_t State)
{
	if (State == ENABLE) {
		if (pI2Cx == I2C1) {
			I2C1_PERI_CLCK_ENABLE();
		} else if (pI2Cx == I2C2) {
			I2C2_PERI_CLCK_ENABLE();
		} else if (pI2Cx == I2C3) {
			I2C3_PERI_CLCK_ENABLE();
		}
	} else {
		if (pI2Cx == I2C1) {
			I2C1_PERI_CLCK_DISABLE();
		} else if (pI2Cx == I2C2) {
			I2C2_PERI_CLCK_DISABLE();
		} else if (pI2Cx == I2C3) {
			I2C3_PERI_CLCK_DISABLE();
		}
	}

	return;
}

void I2C_Init(I2C_Handle *pHandle)
{
	const uint32_t MHz = 1000000U;
	uint32_t mock_reg = 0U;

	mock_reg |= (pHandle->I2C_Config.ACKControl << I2C_CR1_ACK);
	pHandle->pI2Cx->I2C_CR1 = mock_reg;

	mock_reg = 0U;
	mock_reg |= RCC_GetPCLK1Value() / MHz;
	pHandle->pI2Cx->I2C_CR2 |= (mock_reg & 0x3F);

	mock_reg = 0U;
	mock_reg |= (pHandle->I2C_Config.DeviceAddress << 1);
	mock_reg |= (1U << 14);
	pHandle->pI2Cx->I2C_OAR1 |= mock_reg;

	uint16_t CCR_Value = 0U;
	mock_reg = 0U;

	if (pHandle->I2C_Config.SCLSpeedMode == I2C_FM_MODE) {
		mock_reg |= (1U << I2C_CCR_FS);
	}

	if (pHandle->I2C_Config.SCLSpeed <= I2C_SCL_SPEED_SM) {
		CCR_Value = (uint16_t) (RCC_GetPCLK1Value()
				/ (2 * pHandle->I2C_Config.SCLSpeed));
		mock_reg |= (CCR_Value & 0xFFF);
	} else {
		mock_reg |= (pHandle->I2C_Config.FMDutyCycle << I2C_CCR_DUTY);

		if (pHandle->I2C_Config.FMDutyCycle == I2C_FM_DUTY_MODE_2) {
			CCR_Value = (uint16_t) (RCC_GetPCLK1Value()
					/ (3 * pHandle->I2C_Config.SCLSpeed));
		} else {
			CCR_Value = (uint16_t) (RCC_GetPCLK1Value()
					/ (25 * pHandle->I2C_Config.SCLSpeed));
		}

		mock_reg |= (CCR_Value & 0xFFF);
	}

	pHandle->pI2Cx->I2C_CCR = mock_reg;

	if (pHandle->I2C_Config.SCLSpeed <= I2C_SCL_SPEED_SM) {
		mock_reg = (RCC_GetPCLK1Value() / MHz) + 1;
	} else {
		mock_reg = ((RCC_GetPCLK1Value() / MHz) * 300 / 1000) + 1;
	}

	pHandle->pI2Cx->I2C_TRISE = (mock_reg & 0x3F);

	return;
}

void I2C_DeInit(const I2C_RegDef *const pI2Cx)
{
	if (pI2Cx == I2C1) {
		I2C1_REG_RESET();
	} else if (pI2Cx == I2C2) {
		I2C2_REG_RESET();
	} else if (pI2Cx == I2C3) {
		I2C3_REG_RESET();
	}

	return;
}

void I2C_MasterSendData(I2C_Handle *pHandle, uint8_t *pTxBuffer, uint32_t Len,
		uint8_t SlaveAddr, I2C_RepeatedStart RS)
{
	I2C_GenerateStartCondition(pHandle->pI2Cx);

	while (!I2C_GetFlagStatus(pHandle->pI2Cx, I2C_FLAG_SB))
		;

	I2C_ExecuteAdressPhase(pHandle->pI2Cx, SlaveAddr, I2C_WRITE_STATE);

	while (!I2C_GetFlagStatus(pHandle->pI2Cx, I2C_FLAG_ADDR))
		;

	I2C_ClearADDRFlag(pHandle);

	while (Len > 0) {
		while (!I2C_GetFlagStatus(pHandle->pI2Cx, I2C_FLAG_TxE))
			;

		pHandle->pI2Cx->I2C_DR = *pTxBuffer;
		pTxBuffer++;
		Len--;
	}

	while (!I2C_GetFlagStatus(pHandle->pI2Cx, I2C_FLAG_TxE))
		;
	while (!I2C_GetFlagStatus(pHandle->pI2Cx, I2C_FLAG_BTF))
		;

	if (RS == I2C_DISABLE_RS) {
		I2C_GenerateStopCondition(pHandle->pI2Cx);
	}

	return;
}

void I2C_MasterReceiveData(I2C_Handle *pHandle, uint8_t *pRxBuffer, uint8_t Len,
		uint8_t SlaveAddr, I2C_RepeatedStart RS)
{
	I2C_GenerateStartCondition(pHandle->pI2Cx);

	while (!I2C_GetFlagStatus(pHandle->pI2Cx, I2C_FLAG_SB))
		;

	I2C_ExecuteAdressPhase(pHandle->pI2Cx, SlaveAddr, I2C_READ_STATE);

	while (!I2C_GetFlagStatus(pHandle->pI2Cx, I2C_FLAG_ADDR))
		;

	if (Len == 1) {
		I2C_ManageACK(pHandle->pI2Cx, I2C_ACK_DISABLE);
		I2C_ClearADDRFlag(pHandle);

		while (!I2C_GetFlagStatus(pHandle->pI2Cx, I2C_FLAG_RxNE))
			;

		if (RS == I2C_DISABLE_RS) {
			I2C_GenerateStopCondition(pHandle->pI2Cx);
		}

		*pRxBuffer = (uint8_t) (pHandle->pI2Cx->I2C_DR);

	} else if (Len > 1) {
		I2C_ClearADDRFlag(pHandle);

		for (uint32_t i = 0; i < Len; i++) {
			while (!I2C_GetFlagStatus(pHandle->pI2Cx, I2C_FLAG_RxNE))
				;

			if (i == 2) {
				I2C_ManageACK(pHandle->pI2Cx, I2C_ACK_DISABLE);

				if (RS == I2C_DISABLE_RS) {
					I2C_GenerateStopCondition(
							pHandle->pI2Cx);
				}
			}

			*pRxBuffer = (uint8_t) (pHandle->pI2Cx->I2C_DR);
			pRxBuffer++;
		}
	}

	if (pHandle->I2C_Config.ACKControl == I2C_ACK_ENABLE) {
		I2C_ManageACK(pHandle->pI2Cx, I2C_ACK_ENABLE);
	}

	return;
}

I2C_States I2C_MasterSendDataIT(I2C_Handle *pHandle, uint8_t *pTxBuffer,
		uint32_t Len, uint8_t SlaveAddr, I2C_RepeatedStart RS)
{
	I2C_States busystate = pHandle->TxRxState;

	if ((busystate != I2C_BUSY_TX) && (busystate != I2C_BUSY_RX)) {
		pHandle->pTxBuffer = pTxBuffer;
		pHandle->TxLen = Len;
		pHandle->TxRxState = I2C_BUSY_TX;
		pHandle->DevAddr = SlaveAddr;
		pHandle->RS = RS;

		I2C_GenerateStartCondition(pHandle->pI2Cx);

		pHandle->pI2Cx->I2C_CR2 |= (1U << I2C_CR2_ITBUFEN);
		pHandle->pI2Cx->I2C_CR2 |= (1U << I2C_CR2_ITEVTEN);
		pHandle->pI2Cx->I2C_CR2 |= (1U << I2C_CR2_ITERREN);

	}

	return busystate;
}

I2C_States I2C_MasterReceiveDataIT(I2C_Handle *pHandle, uint8_t *pRxBuffer,
		uint8_t Len, uint8_t SlaveAddr, I2C_RepeatedStart RS)
{
	I2C_States busystate = pHandle->TxRxState;

	if ((busystate != I2C_BUSY_TX) && (busystate != I2C_BUSY_RX)) {
		pHandle->pRxBuffer = pRxBuffer;
		pHandle->RxLen = Len;
		pHandle->TxRxState = I2C_BUSY_RX;
		pHandle->RxSize = Len;
		pHandle->DevAddr = SlaveAddr;
		pHandle->RS = RS;

		I2C_GenerateStartCondition(pHandle->pI2Cx);

		pHandle->pI2Cx->I2C_CR2 |= (1U << I2C_CR2_ITBUFEN);
		pHandle->pI2Cx->I2C_CR2 |= (1U << I2C_CR2_ITEVTEN);
		pHandle->pI2Cx->I2C_CR2 |= (1U << I2C_CR2_ITERREN);

	}

	return busystate;
}

void I2C_SlaveSendData(I2C_RegDef *pI2Cx, uint8_t data)
{
	pI2Cx->I2C_DR = data;

	return;
}

uint8_t I2C_SlaveReceiveData(I2C_RegDef *pI2Cx)
{
	return (uint8_t) pI2Cx->I2C_DR;
}

void I2C_SlaveControlCallbackEvents(I2C_RegDef *pI2Cx, uint8_t State)
{
	if (State == ENABLE) {
		pI2Cx->I2C_CR2 |= (1U << I2C_CR2_ITBUFEN);
		pI2Cx->I2C_CR2 |= (1U << I2C_CR2_ITEVTEN);
		pI2Cx->I2C_CR2 |= (1U << I2C_CR2_ITERREN);
	} else {
		pI2Cx->I2C_CR2 &= ~(1U << I2C_CR2_ITBUFEN);
		pI2Cx->I2C_CR2 &= ~(1U << I2C_CR2_ITEVTEN);
		pI2Cx->I2C_CR2 &= ~(1U << I2C_CR2_ITERREN);
	}

	return;
}

void I2C_IRQ_Interrupt_Config(uint8_t IRQ_Number, uint8_t State)
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

void I2C_IRQ_Priority_Config(uint8_t IRQ_Number, uint32_t IRQ_Priority)
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

void I2C_EV_IRQ_Handling(I2C_Handle *pHandle)
{
	uint8_t temp1, temp2, temp3;

	temp1 = (uint8_t) (pHandle->pI2Cx->I2C_CR2 & (1U << I2C_CR2_ITEVTEN));
	temp2 = (uint8_t) (pHandle->pI2Cx->I2C_CR2 & (1U << I2C_CR2_ITBUFEN));
	temp3 = I2C_GetFlagStatus(pHandle->pI2Cx, I2C_FLAG_SB);

	const uint8_t SB_FLAG_SET = temp1 && temp3;

	if (SB_FLAG_SET) {
		if (pHandle->TxRxState == I2C_BUSY_TX) {
			I2C_ExecuteAdressPhase(pHandle->pI2Cx, pHandle->DevAddr,
					I2C_WRITE_STATE);
		} else if (pHandle->TxRxState == I2C_BUSY_RX) {
			I2C_ExecuteAdressPhase(pHandle->pI2Cx, pHandle->DevAddr,
					I2C_READ_STATE);
		}
	}

	temp3 = I2C_GetFlagStatus(pHandle->pI2Cx, I2C_FLAG_ADDR);
	const uint8_t ADDR_FLAG_SET = temp1 && temp3;

	if (ADDR_FLAG_SET) {
		I2C_ClearADDRFlag(pHandle);
	}

	temp3 = I2C_GetFlagStatus(pHandle->pI2Cx, I2C_FLAG_BTF);
	const uint8_t BTF_FLAG_SET = temp1 && temp3;

	if (BTF_FLAG_SET) {
		if (pHandle->TxRxState == I2C_BUSY_TX) {
			if (I2C_GetFlagStatus(pHandle->pI2Cx,
			I2C_FLAG_TxE) == FLAG_SET) {
				if (pHandle->TxLen == 0) {
					if (pHandle->RS == I2C_DISABLE_RS) {
						I2C_GenerateStopCondition(
								pHandle->pI2Cx);
					}

					I2C_CloseTransmision(pHandle);
					I2C_ApplicationEventCallback(pHandle,
							I2C_EV_TX_CMPLT);
				}
			}
		}
	}

	temp3 = I2C_GetFlagStatus(pHandle->pI2Cx, I2C_FLAG_STOPF);
	const uint8_t STOPF_FLAG_SET = temp1 && temp3;
	const uint8_t DUMMY_WRITE = 0x0000;

	if (STOPF_FLAG_SET) {
		pHandle->pI2Cx->I2C_CR1 |= DUMMY_WRITE;
		I2C_ApplicationEventCallback(pHandle, I2C_EV_STOP);
	}

	temp3 = I2C_GetFlagStatus(pHandle->pI2Cx, I2C_FLAG_TxE);
	const uint8_t TxE_FLAG_SET = temp1 && temp2 && temp3;

	if (TxE_FLAG_SET) {
		if (pHandle->pI2Cx->I2C_SR2 & (1U << I2C_SR2_MSL)) {
			I2C_MasterHandleTxEIT(pHandle);
		} else {
			if (pHandle->pI2Cx->I2C_SR2 & (1U << I2C_SR2_TRA)) {
				I2C_ApplicationEventCallback(pHandle,
						I2C_EV_DATA_REQ);
			}
		}
	}

	temp3 = I2C_GetFlagStatus(pHandle->pI2Cx, I2C_FLAG_RxNE);
	const uint8_t RxNE_FLAG_SET = temp1 && temp2 && temp3;

	if (RxNE_FLAG_SET) {
		if (pHandle->pI2Cx->I2C_SR2 & (1U << I2C_SR2_MSL)) {
			if (pHandle->TxRxState == I2C_BUSY_RX) {
				I2C_MasterHandleRxNEIT(pHandle);
			}
		} else {
			if (!(pHandle->pI2Cx->I2C_SR2 & (1U << I2C_SR2_TRA))) {
				I2C_ApplicationEventCallback(pHandle,
						I2C_EV_DATA_RCV);
			}
		}
	}

	return;
}

void I2C_ER_IRQ_Handling(I2C_Handle *pHandle)
{
	uint32_t temp1, temp2;

	temp1 = (pHandle->pI2Cx->I2C_SR1) & (1U << I2C_SR1_BERR);
	temp2 = (pHandle->pI2Cx->I2C_CR2) & (1U << I2C_CR2_ITERREN);

	if (temp1 && temp2) {
		pHandle->pI2Cx->I2C_SR1 &= ~(1U << I2C_SR1_BERR);

		I2C_ApplicationEventCallback(pHandle, I2C_ERROR_BERR);
	}

	temp1 = (pHandle->pI2Cx->I2C_SR1) & (1U << I2C_SR1_ARLO);

	if (temp1 && temp2) {
		pHandle->pI2Cx->I2C_SR1 &= ~(1U << I2C_SR1_ARLO);

		I2C_ApplicationEventCallback(pHandle, I2C_ERROR_ARLO);

	}

	temp1 = (pHandle->pI2Cx->I2C_SR1) & (1U << I2C_SR1_AF);

	if (temp1 && temp2) {
		pHandle->pI2Cx->I2C_SR1 &= ~(1U << I2C_SR1_AF);

		I2C_ApplicationEventCallback(pHandle, I2C_ERROR_AF);
	}

	temp1 = (pHandle->pI2Cx->I2C_SR1) & (1U << I2C_SR1_OVR);

	if (temp1 && temp2) {
		pHandle->pI2Cx->I2C_SR1 &= ~(1U << I2C_SR1_OVR);

		I2C_ApplicationEventCallback(pHandle, I2C_ERROR_OVR);
	}

	temp1 = (pHandle->pI2Cx->I2C_SR1) & (1 << I2C_SR1_TIMEOUT);

	if (temp1 && temp2) {
		pHandle->pI2Cx->I2C_SR1 &= ~(1U << I2C_SR1_TIMEOUT);

		I2C_ApplicationEventCallback(pHandle, I2C_ERROR_TIMEOUT);
	}

	return;
}

void I2C_Peripheral_Control(I2C_RegDef *pI2Cx, uint8_t State)
{
	if (State == ENABLE) {
		pI2Cx->I2C_CR1 |= (1U << I2C_CR1_PE);
	} else {
		pI2Cx->I2C_CR1 &= ~(1U << I2C_CR1_PE);
	}

	return;
}

__attribute__((weak)) void I2C_ApplicationEventCallback(
		I2C_Handle *pHandle, I2C_Application_Events AppEvent)
{
	(void) pHandle;
	(void) AppEvent;

	return;
}

#pragma once

#include <stdint.h>
#include <stddef.h>
#include "stm32f411x.h"

#define IRQ_NO_I2C1_EV 31U
#define IRQ_NO_I2C1_ER 32U
#define IRQ_NO_I2C2_EV 33U
#define IRQ_NO_I2C2_ER 34U
#define IRQ_NO_I2C3_EV 72U
#define IRQ_NO_I2C3_ER 73U

#define I2C_SCL_SPEED_SM 100000U
#define I2C_SCL_SPEED_FM 400000U

#define I2C_FLAG_SB (1U << I2C_SR1_SB)
#define I2C_FLAG_ADDR (1U << I2C_SR1_ADDR)
#define I2C_FLAG_BTF (1U << I2C_SR1_BTF)
#define I2C_FLAG_ADD10 (1U << I2C_SR1_ADD10)
#define I2C_FLAG_STOPF (1U << I2C_SR1_STOPF)
#define I2C_FLAG_RxNE (1U << I2C_SR1_RxNE)
#define I2C_FLAG_TxE (1U << I2C_SR1_TxE)
#define I2C_FLAG_BERR (1U << I2C_SR1_BERR)
#define I2C_FLAG_ARLO (1U << I2C_SR1_ARLO)
#define I2C_FLAG_AF (1U << I2C_SR1_AF)
#define I2C_FLAG_OVR (1U << I2C_SR1_OVR)
#define I2C_FLAG_PECERR (1U << I2C_SR1_PECERR)
#define I2C_FLAG_TIMEOUT (1U << I2C_SR1_TIMEOUT)
#define I2C_FLAG_SMBALERT (1U << I2C_SR1_SMBALERT)

typedef enum {
	I2C_WRITE_STATE = 0U, I2C_READ_STATE = 1U,
} I2C_ReadWriteState;

typedef enum {
	I2C_DISABLE_RS = 0U, I2C_ENABLE_RS = 1U,
} I2C_RepeatedStart;

typedef enum {
	I2C_SM_MODE = 0U, I2C_FM_MODE = 1U,
} I2C_SCLSpeedMode;

typedef enum {
	I2C_ACK_DISABLE = 0U, I2C_ACK_ENABLE = 1U,
} I2C_ACKControl;

typedef enum {
	I2C_FM_DUTY_MODE_2 = 0U, I2C_FM_DUTY_MODE_16_9 = 1U,
} I2C_FMDutyCycle;

typedef enum {
	I2C_READY = 0U, I2C_BUSY_TX = 1U, I2C_BUSY_RX = 2U,
} I2C_States;

typedef enum {
	I2C_EV_TX_CMPLT = 0U,
	I2C_EV_RX_CMPLT = 1U,
	I2C_EV_STOP = 2U,
	I2C_ERROR_BERR = 3U,
	I2C_ERROR_ARLO = 4U,
	I2C_ERROR_AF = 5U,
	I2C_ERROR_OVR = 6U,
	I2C_ERROR_TIMEOUT = 7U,
	I2C_EV_DATA_REQ = 8U,
	I2C_EV_DATA_RCV = 9U,
} I2C_Application_Events;

typedef struct {
	uint32_t SCLSpeed;
	I2C_SCLSpeedMode SCLSpeedMode;
	uint8_t DeviceAddress;
	I2C_ACKControl ACKControl;
	I2C_FMDutyCycle FMDutyCycle;
} I2C_Pin_Config;

typedef struct {
	I2C_RegDef *pI2Cx;
	I2C_Pin_Config I2C_Config;
	uint8_t *pTxBuffer;
	uint8_t *pRxBuffer;
	uint32_t TxLen;
	uint32_t RxLen;
	I2C_States TxRxState;
	uint8_t DevAddr;
	uint32_t RxSize;
	I2C_RepeatedStart RS;
} I2C_Handle;

void I2C_PeriClockControl(const I2C_RegDef *const pI2Cx, uint8_t State);

void I2C_Init(I2C_Handle *pHandle);
void I2C_DeInit(const I2C_RegDef *const pI2Cx);

void I2C_MasterSendData(I2C_Handle *pHandle, uint8_t *pTxBuffer, uint32_t Len,
		uint8_t SlaveAddr, I2C_RepeatedStart RS);
void I2C_MasterReceiveData(I2C_Handle *pHandle, uint8_t *pRxBuffer, uint8_t Len,
		uint8_t SlaveAddr, I2C_RepeatedStart RS);
I2C_States I2C_MasterSendDataIT(I2C_Handle *pHandle, uint8_t *pTxBuffer,
		uint32_t Len, uint8_t SlaveAddr, I2C_RepeatedStart RS);
I2C_States I2C_MasterReceiveDataIT(I2C_Handle *pHandle, uint8_t *pRxBuffer,
		uint8_t Len, uint8_t SlaveAddr, I2C_RepeatedStart RS);

void I2C_SlaveSendData(I2C_RegDef *pI2Cx, uint8_t data);
uint8_t I2C_SlaveReceiveData(I2C_RegDef *pI2Cx);

void I2C_SlaveControlCallbackEvents(I2C_RegDef *pI2Cx, uint8_t State);

void I2C_ManageACK(I2C_RegDef *pI2Cx, I2C_ACKControl State);

void I2C_IRQ_Interrupt_Config(uint8_t IRQ_Number, uint8_t State);
void I2C_IRQ_Priority_Config(uint8_t IRQ_Number, uint32_t IRQ_Priority);
void I2C_EV_IRQ_Handling(I2C_Handle *pHandle);
void I2C_ER_IRQ_Handling(I2C_Handle *pHandle);

void I2C_Peripheral_Control(I2C_RegDef *pI2Cx, uint8_t State);
uint8_t I2C_GetFlagStatus(I2C_RegDef *pI2Cx, uint32_t FlagName);

void I2C_ApplicationEventCallback(I2C_Handle *pHandle,
		I2C_Application_Events AppEvent);

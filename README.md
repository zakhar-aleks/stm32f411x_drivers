# STM32F411 Custom Hardware Abstraction Layer (HAL)

A custom, register-level Hardware Abstraction Layer written in **C** for the **STM32F411 (ARM Cortex-M4)** microcontroller.

The project provides reusable peripheral drivers and hardware abstraction APIs for GPIO, I2C, SPI, and USART communication. It is designed to explore low-level embedded software development, peripheral register configuration, interrupt handling, and modular driver architecture without relying on STM32Cube HAL for the driver implementations.

## Features

- **Register-level programming:** Direct access to STM32F411 memory-mapped peripheral registers.
- **Custom peripheral abstraction:** Reusable initialization, configuration, and data-transfer APIs.
- **Handle-based architecture:** Configuration structures and runtime state management for peripheral drivers.
- **GPIO:** Digital input/output, alternate functions, pull-up/down configuration, and external interrupt support.
- **I2C:** Master transmission and reception, polling and interrupt-driven APIs, acknowledgment management, and error-event handling.
- **SPI:** Master/slave configuration, multiple bus modes, configurable clock settings, and polling/interrupt-driven APIs.
- **USART:** Configurable baud rate, parity, word length, stop bits, hardware flow control, and polling/interrupt-driven communication.
- **Interrupt management:** NVIC configuration, interrupt priorities, peripheral interrupt handlers, and application callbacks.
- **Bare-metal design:** Peripheral register access through custom register structures and bit masks.

## Hardware and Technologies

| Component | Technology |
|---|---|
| Microcontroller | STM32F411 |
| CPU | ARM Cortex-M4 |
| Language | C |
| Architecture | Bare-metal firmware |
| Peripherals | GPIO, I2C, SPI, USART |
| Interrupts | NVIC, EXTI, peripheral interrupts |
| Communication | I2C, SPI, UART/USART |
| Hardware abstraction | Custom register definitions and driver APIs |
| Toolchain | ARM GCC-compatible toolchain |

## Architecture

The HAL follows a layered architecture separating application code, peripheral driver interfaces, and hardware registers.

```text
+----------------------------------+
|         Application Code         |
+----------------------------------+
                 |
+----------------------------------+
|       Peripheral Driver APIs     |
|                                  |
|    GPIO | I2C | SPI | USART      |
+----------------------------------+
                 |
+----------------------------------+
|    Peripheral Configuration      |
|    and Runtime Handle Objects    |
+----------------------------------+
                 |
+----------------------------------+
|    STM32F411 Register Layer      |
|                                  |
|     RCC | NVIC | EXTI | GPIO     |
|          I2C | SPI | USART       |
+----------------------------------+
                 |
+----------------------------------+
|       STM32F411 Hardware         |
+----------------------------------+
```

The device-specific register layer defines peripheral base addresses, register structures, RCC clock controls, interrupt-related definitions, and register bit positions.

Individual peripheral drivers provide configuration structures and public functions for managing hardware.

## GPIO Driver

The GPIO interface supports configurable operation through a `GPIO_Handle` containing a peripheral register pointer and `GPIO_Pin_Config` structure.

### Configuration options

- Input, output, alternate-function, and analog modes
- Rising-edge, falling-edge, and both-edge interrupt modes
- Push-pull and open-drain output configurations
- Output speed configuration
- Pull-up and pull-down resistors
- Alternate function selection for pins 0–15

### Public API

```c
void GPIO_Init(GPIO_Handle *pHandle);
void GPIO_DeInit(
    const GPIO_RegDef *const pGPIO
);

uint8_t GPIO_ReadFromInputPin(
    const GPIO_RegDef *const pGPIO,
    GPIO_Pin_Numbers PIN_Number
);

uint16_t GPIO_ReadFromInputPort(
    const GPIO_RegDef *const pGPIO
);

void GPIO_WriteToOutputPin(
    GPIO_RegDef *pGPIO,
    GPIO_Pin_Numbers PIN_Number,
    uint8_t Value
);

void GPIO_WriteToOutputPort(
    GPIO_RegDef *pGPIO,
    uint16_t Value
);

void GPIO_ToggleOutputPin(
    GPIO_RegDef *pGPIO,
    GPIO_Pin_Numbers PIN_Number
);
```

The GPIO driver also declares functions for interrupt configuration, priority management, and interrupt handling.

## I2C Driver

The I2C driver exposes both polling and interrupt-driven master communication interfaces, along with slave byte-access functions.

### Configuration options

- Standard-mode and Fast-mode clock configuration
- Configurable ACK handling
- Fast-mode duty-cycle selection
- Repeated START conditions
- Peripheral enable/disable control
- Event and error interrupt handling

### Master communication API

```c
void I2C_MasterSendData(
    I2C_Handle *pHandle,
    uint8_t *pTxBuffer,
    uint32_t Len,
    uint8_t SlaveAddr,
    I2C_RepeatedStart RS
);

void I2C_MasterReceiveData(
    I2C_Handle *pHandle,
    uint8_t *pRxBuffer,
    uint8_t Len,
    uint8_t SlaveAddr,
    I2C_RepeatedStart RS
);
```

Interrupt-driven variants are provided through:

```c
I2C_MasterSendDataIT();
I2C_MasterReceiveDataIT();
```

The driver uses an `I2C_Handle` to store peripheral configuration, transmission/reception buffers, transfer lengths, repeated-start settings, and communication state.

### Error and event handling

The public interface defines events for:

- Transmission and reception completion
- STOP detection
- Bus error (`BERR`)
- Arbitration loss (`ARLO`)
- Acknowledge failure (`AF`)
- Overrun (`OVR`)
- Timeout conditions
- Data requested/received events

Separate event and error IRQ handlers are declared:

```c
void I2C_EV_IRQ_Handling(
    I2C_Handle *pHandle
);

void I2C_ER_IRQ_Handling(
    I2C_Handle *pHandle
);
```

Application-level events are exposed through:

```c
void I2C_ApplicationEventCallback(
    I2C_Handle *pHandle,
    I2C_Application_Events AppEvent
);
```

### Practical application

The custom I2C implementation has been used to interface an **SSD1306 128×64 OLED display**, including display initialization and rendering image data.

The project also involved debugging an unresponsive DS3231 RTC module by analyzing I2C acknowledgment failures and peripheral status flags.

## SPI Driver

The SPI driver provides configuration and data-transfer interfaces for STM32F411 SPI peripherals.

### Configuration options

- Master and slave operation
- Full-duplex communication
- Half-duplex communication
- Receive-only mode
- Configurable clock prescaler
- Clock polarity (CPOL)
- Clock phase (CPHA)
- 8-bit and 16-bit frame selection
- Software slave management

### Communication API

```c
void SPI_SendData(
    SPI_RegDef *pSPIx,
    uint8_t *pTxBuffer,
    uint32_t Len
);

void SPI_ReceiveData(
    SPI_RegDef *pSPIx,
    uint8_t *pRxBuffer,
    uint32_t Len
);
```

Interrupt-driven interfaces are also declared:

```c
SPI_SendDataIT();
SPI_ReceiveDataIT();
```

Additional API functions cover peripheral initialization and deinitialization, interrupt configuration, transmission/reception completion, overrun handling, and application callbacks.

The SPI handle maintains separate transmission and reception states, buffer pointers, and remaining transfer lengths.

## USART Driver

The USART driver supports configurable serial communication through a handle-based interface.

### Configuration options

- Transmit-only, receive-only, and transmit/receive modes
- Baud-rate configuration
- 8-bit and 9-bit word-length settings
- Even, odd, or disabled parity
- Configurable stop bits
- RTS/CTS hardware flow control
- Polling and interrupt-driven data transfer

The header provides predefined baud rates ranging from 1,200 to 3,000,000 baud.

### Communication API

```c
void USART_SendData(
    USART_Handle *pHandle,
    uint8_t *pTxBuffer,
    uint32_t Len
);

void USART_ReceiveData(
    USART_Handle *pHandle,
    uint8_t *pRxBuffer,
    uint32_t Len
);
```

Interrupt-driven transmission and reception are exposed through:

```c
USART_SendDataIT();
USART_ReceiveDataIT();
```

The driver also declares status-flag functions, baud-rate configuration, interrupt handlers, and application callbacks.

## Interrupt Handling

The HAL exposes interrupt-management interfaces for GPIO, I2C, SPI, and USART.

The register-definition layer includes NVIC enable/disable registers, interrupt priority registers, IRQ numbers, and low-level interrupt-mask helpers.

Peripheral drivers define dedicated interrupt configuration and handling functions.

For asynchronous communication, driver handles contain state and buffer information associated with ongoing transfers.

Application callback interfaces allow peripheral events to be communicated to higher-level application code.

Actual interrupt vector integration and callback behavior depend on the corresponding driver implementations and startup configuration.

## Register-Level Hardware Abstraction

The project includes custom STM32F411 peripheral register structures using `volatile` register fields.

Supported register definitions include:

- GPIO
- RCC
- EXTI
- SYSCFG
- I2C
- SPI
- USART

Peripheral pointers are mapped to their hardware base addresses.

Example:

```c
#define GPIOA \
    ((GPIO_RegDef *) GPIOA_BASEADDR)

#define I2C1 \
    ((I2C_RegDef *) I2C1_BASEADDR)
```

Peripheral clock management is exposed through RCC register operations and helper macros.

The device header also defines low-level interrupt controls using ARM assembly instructions and restricts selected dynamic allocation functions through a GCC-specific directive.

## Design Approach

The implementation emphasizes several embedded software engineering concepts.

**Separation of concerns:** Applications interact with peripheral APIs while register definitions remain in the hardware-specific layer.

**Configuration-driven initialization:** Peripheral handles and configuration structures group setup parameters instead of exposing register operations throughout application code.

**Stateful communication:** Interrupt-driven APIs maintain transfer state, buffers, and lengths across interrupt events.

**Direct hardware control:** The HAL exposes a higher-level programming interface while retaining the ability to work directly with hardware register specifications.

**Static-memory-oriented development:** The device header explicitly restricts several heap-allocation functions, reflecting a design preference for predictable memory usage in bare-metal applications.

## Current Implementation and Validation

The provided headers declare interfaces for GPIO, I2C, SPI, and USART, including polling and interrupt-driven functionality.

The GPIO/I2C work has been used in an SSD1306 display integration, and I2C hardware faults have been investigated using protocol-level status information.

The full set of `.c` implementations, hardware test results, build scripts, and board-specific configurations are not included in this documentation review. Therefore, support for every declared API, interrupt mode, and peripheral configuration should be verified before treating the library as production-ready.

## Planned Improvements

- Expand hardware test coverage.
- Add documented examples for each peripheral.
- Improve automated testing of platform-independent logic.
- Document error handling and recovery behavior.
- Review peripheral register masks and reset helpers.
- Integrate additional STM32 peripherals where useful.

## References

- STMicroelectronics — STM32F411 Reference Manual
- STMicroelectronics — STM32F411 Datasheet
- ARM — Cortex-M4 Technical Reference Documentation

## Author

**Zakhar Aleksandruk**

Computer Science student specializing in embedded software development, STM32 microcontrollers, and hardware–software integration.

This project demonstrates practical experience with register-level programming, peripheral driver architecture, communication protocols, and embedded C development.
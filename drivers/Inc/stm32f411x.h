#pragma once
#pragma GCC poison malloc calloc realloc free

#include <stdint.h>

#define NVIC_ISER0REG     ((volatile uint32_t *) 0xE000E100U)
#define NVIC_ISER1REG     ((volatile uint32_t *) 0xE000E104U)
#define NVIC_ISER2REG     ((volatile uint32_t *) 0xE000E108U)
#define NVIC_ISER3REG     ((volatile uint32_t *) 0xE000E10CU)

#define NVIC_ICER0REG     ((volatile uint32_t *) 0XE000E180U)
#define NVIC_ICER1REG     ((volatile uint32_t *) 0XE000E184U)
#define NVIC_ICER2REG     ((volatile uint32_t *) 0XE000E188U)
#define NVIC_ICER3REG     ((volatile uint32_t *) 0XE000E18CU)

#define NVIC_IPR_BASEADDR ((volatile uint32_t *) 0xE000E400U)

#define NVIC_REG_SIZE           32U
#define IPR_REG_SIZE            4U
#define IPR_REG_SEC_SIZE_BYTE   8U
#define IPR_SECTION_MASK        0xFFU
#define IPR_UNIMPLEMENTED_BITS  4U

#define ENABLE            1U
#define DISABLE           0U
#define SET               ENABLE
#define RESET             DISABLE
#define GPIO_PIN_SET      ENABLE
#define GPIO_PIN_RESET    DISABLE
#define FLAG_SET          SET
#define FLAG_RESET        RESET

#define FLASH_BASEADDR    0x08000000UL
#define SRAM1_BASEADDR    0x20000000UL
#define SRAM_BASEADDR     SRAM1_BASEADDR
#define ROM_BASEADDR      0x1FFF0000UL

#define PERIPHERAL_BASE   0x40000000UL
#define AHB1_BASEADDR     0x40020000UL
#define AHB2_BASEADDR     0x50000000UL
#define APB1_BASEADDR     PERIPHERAL_BASE
#define APB2_BASEADDR     0x40010000UL

#define GPIOA_BASEADDR    (AHB1_BASEADDR + 0x0000)
#define GPIOB_BASEADDR    (AHB1_BASEADDR + 0x0400)
#define GPIOC_BASEADDR    (AHB1_BASEADDR + 0x0800)
#define GPIOD_BASEADDR    (AHB1_BASEADDR + 0x0C00)
#define GPIOE_BASEADDR    (AHB1_BASEADDR + 0x1000)
#define GPIOH_BASEADDR    (AHB1_BASEADDR + 0x1C00)

#define I2C1_BASEADDR     (APB1_BASEADDR + 0x5400)
#define I2C2_BASEADDR     (APB1_BASEADDR + 0x5800)
#define I2C3_BASEADDR     (APB1_BASEADDR + 0x5C00)

#define SPI1_BASEADDR     (APB2_BASEADDR + 0x3000)
#define SPI2_BASEADDR     (APB1_BASEADDR + 0x3800)
#define SPI3_BASEADDR     (APB1_BASEADDR + 0x3C00)
#define SPI4_BASEADDR     (APB2_BASEADDR + 0x3400)
#define SPI5_BASEADDR     (APB2_BASEADDR + 0x5000)

#define USART1_BASEADDR   (APB2_BASEADDR + 0x1000)
#define USART2_BASEADDR   (APB1_BASEADDR + 0x4400)
#define USART6_BASEADDR   (APB2_BASEADDR + 0x1400)

#define RCC_BASEADDR      (AHB1_BASEADDR + 0x3800)

#define EXTI_BASEADDR     (APB2_BASEADDR + 0x3C00)

#define SYSCFG_BASEADDR   (APB2_BASEADDR + 0x3800)

#define IRQ_NO_EXTI_0     6U
#define IRQ_NO_EXTI_1     7U
#define IRQ_NO_EXTI_2     8U
#define IRQ_NO_EXTI_3     9U
#define IRQ_NO_EXTI_4     10U
#define IRQ_NO_EXTI_9_5   23U
#define IRQ_NO_EXTI_15_10 40U

#define IRQ_NO_SPI1       35U
#define IRQ_NO_SPI2       36U
#define IRQ_NO_SPI3       51U
#define IRQ_NO_SPI4       84U
#define IRQ_NO_SPI5       85U

#define IRQ_NO_DMA2_CHANNEL0 56U

#define NVIC_IRQ_PRIO0    0U
#define NVIC_IRQ_PRIO1    1U
#define NVIC_IRQ_PRIO2    2U
#define NVIC_IRQ_PRIO3    3U
#define NVIC_IRQ_PRIO4    4U
#define NVIC_IRQ_PRIO5    5U
#define NVIC_IRQ_PRIO6    6U
#define NVIC_IRQ_PRIO7    7U
#define NVIC_IRQ_PRIO8    8U
#define NVIC_IRQ_PRIO9    9U
#define NVIC_IRQ_PRIO10   10U
#define NVIC_IRQ_PRIO11   11U
#define NVIC_IRQ_PRIO12   12U
#define NVIC_IRQ_PRIO13   13U
#define NVIC_IRQ_PRIO14   14U
#define NVIC_IRQ_PRIO15   15U

typedef struct {
	volatile uint32_t MODER;
	volatile uint32_t OTYPER;
	volatile uint32_t OSPEEDR;
	volatile uint32_t PUPDR;
	volatile uint32_t IDR;
	volatile uint32_t ODR;
	volatile uint32_t BSRR;
	volatile uint32_t LCKR;
	volatile uint32_t AFRL;
	volatile uint32_t AFRH;
} GPIO_RegDef;

#define GPIOA ((GPIO_RegDef *) GPIOA_BASEADDR)
#define GPIOB ((GPIO_RegDef *) GPIOB_BASEADDR)
#define GPIOC ((GPIO_RegDef *) GPIOC_BASEADDR)
#define GPIOD ((GPIO_RegDef *) GPIOD_BASEADDR)
#define GPIOE ((GPIO_RegDef *) GPIOE_BASEADDR)
#define GPIOH ((GPIO_RegDef *) GPIOH_BASEADDR)

typedef struct {
	volatile uint32_t RCC_CR;
	volatile uint32_t RCC_PLLCFGR;
	volatile uint32_t RCC_CFGR;
	volatile uint32_t RCC_CIR;
	volatile uint32_t RCC_AHB1RSTR;
	volatile uint32_t RCC_AHB2RSTR;
	uint32_t RESERVED0[2];
	volatile uint32_t RCC_APB1RSTR;
	volatile uint32_t RCC_APB2RSTR;
	uint32_t RESERVED1[2];
	volatile uint32_t RCC_AHB1ENR;
	volatile uint32_t RCC_AHB2ENR;
	uint32_t RESERVED2[2];
	volatile uint32_t RCC_APB1ENR;
	volatile uint32_t RCC_APB2ENR;
	uint32_t RESERVED3[2];
	volatile uint32_t RCC_AHB1LPENR;
	volatile uint32_t RCC_AHB2LPENR;
	uint32_t RESERVED4[2];
	volatile uint32_t RCC_APB1LPENR;
	volatile uint32_t RCC_APB2LPENR;
	uint32_t RESERVED5[2];
	volatile uint32_t RCC_BDCR;
	volatile uint32_t RCC_CSR;
	uint32_t RESERVED6[2];
	volatile uint32_t RCC_SSCGR;
	volatile uint32_t RCC_PLLI2SCFGR;
	uint32_t RESERVED7;
	volatile uint32_t RCC_DCKCFGR;
} RCC_RegDef;

#define RCC ((RCC_RegDef *) RCC_BASEADDR)

typedef struct {
	volatile uint32_t EXTI_IMR;
	volatile uint32_t EXTI_EMR;
	volatile uint32_t EXTI_RTSR;
	volatile uint32_t EXTI_FTSR;
	volatile uint32_t EXTI_SWIER;
	volatile uint32_t EXTI_PR;
} EXTI_RegDef;

#define EXTI ((EXTI_RegDef *) EXTI_BASEADDR)

typedef struct {
	volatile uint32_t SYSCFG_MEMRMP;
	volatile uint32_t SYSCFG_PMC;
	volatile uint32_t SYSCFG_EXTICR[4];
	volatile uint32_t RESERVED[2];
	volatile uint32_t SYSCFG_CMPCR;
} SYSCFG_RegDef;

#define SYSCFG ((SYSCFG_RegDef *) SYSCFG_BASEADDR)

typedef struct {
	volatile uint32_t SPI_CR1;
	volatile uint32_t SPI_CR2;
	volatile uint32_t SPI_SR;
	volatile uint32_t SPI_DR;
	volatile uint32_t SPI_CRCPR;
	volatile uint32_t SPI_RXCRCR;
	volatile uint32_t SPI_TXCRCR;
	volatile uint32_t SPI_I2SCFGR;
	volatile uint32_t SPI_I2SPR;
} SPI_RegDef;

#define SPI1 ((SPI_RegDef *) SPI1_BASEADDR)
#define SPI2 ((SPI_RegDef *) SPI2_BASEADDR)
#define SPI3 ((SPI_RegDef *) SPI3_BASEADDR)
#define SPI4 ((SPI_RegDef *) SPI4_BASEADDR)
#define SPI5 ((SPI_RegDef *) SPI5_BASEADDR)

typedef struct {
	volatile uint32_t I2C_CR1;
	volatile uint32_t I2C_CR2;
	volatile uint32_t I2C_OAR1;
	volatile uint32_t I2C_OAR2;
	volatile uint32_t I2C_DR;
	volatile uint32_t I2C_SR1;
	volatile uint32_t I2C_SR2;
	volatile uint32_t I2C_CCR;
	volatile uint32_t I2C_TRISE;
	volatile uint32_t I2C_FLTR;
} I2C_RegDef;

#define I2C1 ((I2C_RegDef *) I2C1_BASEADDR)
#define I2C2 ((I2C_RegDef *) I2C2_BASEADDR)
#define I2C3 ((I2C_RegDef *) I2C3_BASEADDR)

typedef struct {
	volatile uint32_t USART_SR;
	volatile uint32_t USART_DR;
	volatile uint32_t USART_BRR;
	volatile uint32_t USART_CR1;
	volatile uint32_t USART_CR2;
	volatile uint32_t USART_CR3;
	volatile uint32_t USART_GTPR;
} USART_RegDef;

#define USART1 ((USART_RegDef *) USART1_BASEADDR)
#define USART2 ((USART_RegDef *) USART2_BASEADDR)
#define USART6 ((USART_RegDef *) USART6_BASEADDR)

#define GPIOA_PERI_CLCK_ENABLE()    (RCC -> RCC_AHB1ENR |= (1U << 0))
#define GPIOB_PERI_CLCK_ENABLE()    (RCC -> RCC_AHB1ENR |= (1U << 1))
#define GPIOC_PERI_CLCK_ENABLE()    (RCC -> RCC_AHB1ENR |= (1U << 2))
#define GPIOD_PERI_CLCK_ENABLE()    (RCC -> RCC_AHB1ENR |= (1U << 3))
#define GPIOE_PERI_CLCK_ENABLE()    (RCC -> RCC_AHB1ENR |= (1U << 4))
#define GPIOH_PERI_CLCK_ENABLE()    (RCC -> RCC_AHB1ENR |= (1U << 7))

#define GPIOA_PERI_CLCK_DISABLE()   (RCC -> RCC_AHB1ENR &= ~(1U << 0))
#define GPIOB_PERI_CLCK_DISABLE()   (RCC -> RCC_AHB1ENR &= ~(1U << 1))
#define GPIOC_PERI_CLCK_DISABLE()   (RCC -> RCC_AHB1ENR &= ~(1U << 2))
#define GPIOD_PERI_CLCK_DISABLE()   (RCC -> RCC_AHB1ENR &= ~(1U << 3))
#define GPIOE_PERI_CLCK_DISABLE()   (RCC -> RCC_AHB1ENR &= ~(1U << 4))
#define GPIOH_PERI_CLCK_DISABLE()   (RCC -> RCC_AHB1ENR &= ~(1U << 7))

#define GPIOA_REG_RESET()           do { (RCC -> RCC_AHB1RSTR |= (1U << 0));  (RCC -> RCC_AHB1RSTR &= ~(1U << 0)); } while(0)
#define GPIOB_REG_RESET()           do { (RCC -> RCC_AHB1RSTR |= (1U << 1));  (RCC -> RCC_AHB1RSTR &= ~(1U << 1)); } while(0)
#define GPIOC_REG_RESET()           do { (RCC -> RCC_AHB1RSTR |= (1U << 2));  (RCC -> RCC_AHB1RSTR &= ~(1U << 2)); } while(0)
#define GPIOD_REG_RESET()           do { (RCC -> RCC_AHB1RSTR |= (1U << 3));  (RCC -> RCC_AHB1RSTR &= ~(1U << 3)); } while(0)
#define GPIOE_REG_RESET()           do { (RCC -> RCC_AHB1RSTR |= (1U << 4));  (RCC -> RCC_AHB1RSTR &= ~(1U << 4)); } while(0)
#define GPIOH_REG_RESET()           do { (RCC -> RCC_AHB1RSTR |= (1U << 7));  (RCC -> RCC_AHB1RSTR &= ~(1U << 7)); } while(0)

#define I2C1_PERI_CLCK_ENABLE()     (RCC -> RCC_APB1ENR |= (1U << 21))
#define I2C2_PERI_CLCK_ENABLE()     (RCC -> RCC_APB1ENR |= (1U << 22))
#define I2C3_PERI_CLCK_ENABLE()     (RCC -> RCC_APB1ENR |= (1U << 23))

#define I2C1_PERI_CLCK_DISABLE()     (RCC -> RCC_APB1ENR &= ~(1U << 21))
#define I2C2_PERI_CLCK_DISABLE()     (RCC -> RCC_APB1ENR &= ~(1U << 22))
#define I2C3_PERI_CLCK_DISABLE()     (RCC -> RCC_APB1ENR &= ~(1U << 23))

#define I2C1_REG_RESET()     (RCC -> RCC_APB1RSTR |= (1U << 21))
#define I2C2_REG_RESET()     (RCC -> RCC_APB1RSTR |= (1U << 22))
#define I2C3_REG_RESET()     (RCC -> RCC_APB1RSTR |= (1U << 23))

#define SPI1_PERI_CLCK_ENABLE()     (RCC -> RCC_APB2ENR |= (1U << 12))
#define SPI2_PERI_CLCK_ENABLE()     (RCC -> RCC_APB1ENR |= (1U << 14))
#define SPI3_PERI_CLCK_ENABLE()     (RCC -> RCC_APB1ENR |= (1U << 15))
#define SPI4_PERI_CLCK_ENABLE()     (RCC -> RCC_APB2ENR |= (1U << 13))
#define SPI5_PERI_CLCK_ENABLE()     (RCC -> RCC_APB2ENR |= (1U << 20))

#define SPI1_PERI_CLCK_DISABLE()    (RCC -> RCC_APB2ENR &= ~(1U << 12))
#define SPI2_PERI_CLCK_DISABLE()    (RCC -> RCC_APB1ENR &= ~(1U << 14))
#define SPI3_PERI_CLCK_DISABLE()    (RCC -> RCC_APB1ENR &= ~(1U << 15))
#define SPI4_PERI_CLCK_DISABLE()    (RCC -> RCC_APB2ENR &= ~(1U << 13))
#define SPI5_PERI_CLCK_DISABLE()    (RCC -> RCC_APB2ENR &= ~(1U << 20))

#define SPI1_REG_RESET()            do { (RCC -> RCC_APB2RSTR |= (1U << 0));  (RCC -> RCC_APB2RSTR &= ~(1U << 12)); } while(0)
#define SPI2_REG_RESET()            do { (RCC -> RCC_APB1RSTR |= (1U << 1));  (RCC -> RCC_APB1RSTR &= ~(1U << 14)); } while(0)
#define SPI3_REG_RESET()            do { (RCC -> RCC_APB1RSTR |= (1U << 2));  (RCC -> RCC_APB1RSTR &= ~(1U << 15)); } while(0)
#define SPI4_REG_RESET()            do { (RCC -> RCC_APB2RSTR |= (1U << 3));  (RCC -> RCC_APB2RSTR &= ~(1U << 13)); } while(0)
#define SPI5_REG_RESET()            do { (RCC -> RCC_APB2RSTR |= (1U << 4));  (RCC -> RCC_APB2RSTR &= ~(1U << 20)); } while(0)

#define USART1_PERI_CLCK_ENABLE()   (RCC -> RCC_APB2ENR |= (1U << 4))
#define USART2_PERI_CLCK_ENABLE()   (RCC -> RCC_APB1ENR |= (1U << 17))
#define USART6_PERI_CLCK_ENABLE()   (RCC -> RCC_APB2ENR |= (1U << 5))

#define USART1_PERI_CLCK_DISABLE()   (RCC -> RCC_APB2ENR &= ~(1U << 4))
#define USART2_PERI_CLCK_DISABLE()   (RCC -> RCC_APB1ENR &= ~(1U << 17))
#define USART6_PERI_CLCK_DISABLE()   (RCC -> RCC_APB2ENR &= ~(1U << 5))

#define USART1_REG_RESET()   	     (RCC -> RCC_APB2RSTR |= (1U << 4))
#define USART2_REG_RESET()   	     (RCC -> RCC_APB1RSTR |= (1U << 17))
#define USART6_REG_RESET()   	     (RCC -> RCC_APB2RSTR |= (1U << 5))

#define SYSCFG_PERI_CLCK_ENABLE()   (RCC -> RCC_APB2ENR |= (1U << 14))

#define GPIO_BASEADDR_TO_EXTI_CODE(x) ((x == GPIOA) ? 0 :\
									  (x == GPIOB) ? 1 :\
									  (x == GPIOC) ? 2 :\
									  (x == GPIOD) ? 3 :\
									  (x == GPIOE) ? 4 :\
									  (x == GPIOH) ? 7 : 0)

#define I2C_CR1_PE 		    0U
#define I2C_CR1_SMBUS 		    1U
#define I2C_CR1_RESERVED 	    2U
#define I2C_CR1_SMBTYPE 	    3U
#define I2C_CR1_ENARP 		    4U
#define I2C_CR1_ENPEC 		    5U
#define I2C_CR1_ENGC 		    6U
#define I2C_CR1_NOSTRETCH 	    7U
#define I2C_CR1_START 		    8U
#define I2C_CR1_STOP 		    9U
#define I2C_CR1_ACK 		    10U
#define I2C_CR1_POS		    11U
#define I2C_CR1_PEC 		    12U
#define I2C_CR1_ALERT 		    13U
#define I2C_CR1_RESERVED2 	    14U
#define I2C_CR1_SWRST 		    15U

#define I2C_CR2_FREQ 		    0U
#define I2C_CR2_RESERVED            6U
#define I2C_CR2_RESERVED2 	    7U
#define I2C_CR2_ITERREN 	    8U
#define I2C_CR2_ITEVTEN 	    9U
#define I2C_CR2_ITBUFEN             10U
#define I2C_CR2_DMAEN 		    11U
#define I2C_CR2_LAST  		    12U
#define I2C_CR2_RESERVED3 	    13U
#define I2C_CR2_RESERVED4 	    14U
#define I2C_CR2_RESERVED5	    15U

#define I2C_SR1_SB 		    0U
#define I2C_SR1_ADDR  		    1U
#define I2C_SR1_BTF  	    	    2U
#define I2C_SR1_ADD10  	    	    3U
#define I2C_SR1_STOPF 		    4U
#define I2C_SR1_RESERVED 	    5U
#define I2C_SR1_RxNE 		    6U
#define I2C_SR1_TxE  	    	    7U
#define I2C_SR1_BERR  		    8U
#define I2C_SR1_ARLO 		    9U
#define I2C_SR1_AF  		    10U
#define I2C_SR1_OVR 		    11U
#define I2C_SR1_PECERR 		    12U
#define I2C_SR1_RESERVED2 	    13U
#define I2C_SR1_TIMEOUT 	    14U
#define I2C_SR1_SMBALERT 	    15U

#define I2C_SR2_MSL 		    0U
#define I2C_SR2_BUSY   		    1U
#define I2C_SR2_TRA   	    	    2U
#define I2C_SR2_RESERVED  	    3U
#define I2C_SR2_GENCALL  	    4U
#define I2C_SR2_SMBDEFAULT 	    5U
#define I2C_SR2_SMBHOST 	    6U
#define I2C_SR2_DUALF   	    7U
#define I2C_SR2_PEC  		    8U

#define I2C_CCR_CCR		    0U
#define I2C_CCR_RESERVED	    12U
#define I2C_CCR_RESERVED2	    13U
#define I2C_CCR_DUTY	    	    14U
#define I2C_CCR_FS	    	    15U

#define I2C_TRISE_TRISE             0U

#define USART_CR1_SBK               0U
#define USART_CR1_RWU               1U
#define USART_CR1_RE                2U
#define USART_CR1_TE                3U
#define USART_CR1_IDLEIE            4U
#define USART_CR1_RXNEIE            5U
#define USART_CR1_TCIE              6U
#define USART_CR1_TXEIE             7U
#define USART_CR1_PEIE              8U
#define USART_CR1_PS                9U
#define USART_CR1_PCE               10U
#define USART_CR1_WAKE              11U
#define USART_CR1_M                 12U
#define USART_CR1_UE                13U
#define USART_CR1_RESERVED          14U
#define USART_CR1_OVER8             15U

#define USART_CR2_ADD               0U
#define USART_CR2_RESERVED          4U
#define USART_CR2_LBDL              5U
#define USART_CR2_LBDIE             6U
#define USART_CR2_RESERVED2         7U
#define USART_CR2_LBCL              8U
#define USART_CR2_CPHA              9U
#define USART_CR2_CPOL              10U
#define USART_CR2_CLKEN             11U
#define USART_CR2_STOP              12U
#define USART_CR2_LINEN             14U
#define USART_CR2_RESERVED3         15U

#define USART_CR3_EIE               0U
#define USART_CR3_IREN              1U
#define USART_CR3_IRLP              2U
#define USART_CR3_HDSEL             3U
#define USART_CR3_NACK              4U
#define USART_CR3_SCEN              5U
#define USART_CR3_DMAR              6U
#define USART_CR3_DMAT              7U
#define USART_CR3_RTSE              8U
#define USART_CR3_CTSE              9U
#define USART_CR3_CTSIE             10U
#define USART_CR3_ONEBIT            11U

#define FIELD_SIZE_2_BIT            2U
#define ALT_FUNC_REG_SIZE           4U
#define PINS_PER_AF_REG             8U
#define AFRH_BOUND                  8U
#define SYSCFG_EXTI_CR_REG_SIZE     4U

static inline uint32_t __get_PRIMASK(void)
{
	uint32_t result;
	__asm volatile ("mrs %0, primask" : "=r" (result) );
	return result;
}

static inline void __set_PRIMASK(uint32_t priMask)
{
	__asm volatile ("msr primask, %0" : : "r" (priMask) : "memory");
}

static inline void __disable_irq(void)
{
	__asm volatile ("cpsid i" : : : "memory");
}

static inline void __enable_irq(void)
{
	__asm volatile ("cpsie i" : : : "memory");
}

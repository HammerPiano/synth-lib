#include "RCC.h"

/* Peripheral structure definitions */
typedef struct
{
	__IO uint32_t CR;
	__IO uint32_t CFGR;
	__IO uint32_t CIR;
	__IO uint32_t APB2RSTR;
	__IO uint32_t APB1RSTR;
	__IO uint32_t AHBENR;
	__IO uint32_t APB2ENR;
	__IO uint32_t APB1ENR;
	__IO uint32_t BDCR;
	__IO uint32_t CSR;

} RCC_TypeDef;

typedef struct
{
	__IO uint32_t ACR;
	__IO uint32_t KEYR;
	__IO uint32_t OPTKEYR;
	__IO uint32_t SR;
	__IO uint32_t CR;
	__IO uint32_t AR;
	__IO uint32_t RESERVED;
	__IO uint32_t OBR;
	__IO uint32_t WRPR;
} FLASH_TypeDef;

/* peripheral definitions */

#define FLASH_R_BASE   (AHBPERIPH_BASE + 0x00002000U) /*!< Flash registers base address */
#define FLASHSIZE_BASE 0x1FFFF7E0U					  /*!< FLASH Size register base address */
#define FLASH		   ((FLASH_TypeDef *)FLASH_R_BASE)
#define UID_BASE	   0x1FFFF7E8U /*!< Unique device ID register base address */
#define OB_BASE		   0x1FFFF800U /*!< Flash Option Bytes base address */
#define RCC_BASE	   (AHBPERIPH_BASE + 0x00001000U)
#define RCC			   ((RCC_TypeDef *)RCC_BASE)

/* RCC configuration flags: */

/* Multipler of 9 */
#define PLL_MUL (0x001C0000)

/* PLL Source from external crystal */
#define PLL_SRC_PREDIV1 (0x00010000)

/* ADC max frequency 14MHz, F_CPU is 72MHz, divs are 2,4,6,8, closest value below 14 is 12MHz=72/6 */
#define ADC_DIV (0x00008000)

/* APB1 max frequency 36MHz, half of F_CPU */
#define APB1_DIV (0x00000100)

/* system clock select source */
#define CLK_SRC_SELECT_HSI (0x00000000)
#define CLK_SRC_SELECT_PLL (0x00000002)
#define CLK_SRC_SELECT_MSK (0x00000003)
/* actual system clock source */
#define CLK_SRC_CURRENT_HSI (0x00000000)
#define CLK_SRC_CURRENT_PLL (0x00000008)
#define CLK_SRC_CURRENT_MSK (0x0000000C)

/* clock subsystem flags */
/* HSI - internal high speed clock */
#define CLK_HSI_ON	(0x00000001)
#define CLK_HSI_RDY (0x00000002)

/* HSE - external high speed clock, crystals */
#define CLK_HSE_ON	(0x00010000)
#define CLK_HSE_RDY (0x00020000)

/* PLL - clock multipler */
#define CLK_PLL_ON	(0x01000000)
#define CLK_PLL_RDY (0x02000000)
/* CSS - clock security system, when the HSE fails the chip wont be completly dead */
#define CLK_CSS_ON (0x00080000)

/* The enum RCC_Peripherals_t is seperated to 3 32 entry long parts, each for clock domain */
#define CLOCK_DOMAIN_PERIPH_COUNT 32
#define AHB_INDEX				  0
#define APB1_INDEX				  32
#define APB2_INDEX				  64

/*
? Peripheral control functions:
*/
void RCC_peripheral_set_clock(RCC_Peripherals_t periph, bool enable)
{
	periph_ptr_t clock_ptr = NULL;
	if (periph < APB1_INDEX)
	{
		clock_ptr = &(RCC->AHBENR);
	}
	else if (periph < APB2_INDEX)
	{
		clock_ptr = &(RCC->APB1ENR);
	}
	else
	{
		clock_ptr = &(RCC->APB2ENR);
	}

	*clock_ptr |= enable << (periph % CLOCK_DOMAIN_PERIPH_COUNT);
}

void RCC_peripheral_reset(RCC_Peripherals_t periph)
{
	periph_ptr_t reset_ptr = NULL;
	if (periph < APB1_INDEX)
	{
		return; /* AHB Doesn't support peripheral reset */
	}
	else if (periph < APB2_INDEX)
	{
		reset_ptr = &(RCC->APB1RSTR);
	}
	else
	{
		reset_ptr = &(RCC->APB2RSTR);
	}

	*reset_ptr |= 1 << (periph % CLOCK_DOMAIN_PERIPH_COUNT);
}

/*
? System clock control functions
*/
void RCC_reset_clock()
{
	/* Reset the RCC clock configuration to the default reset state(for debug purpose) */
	/* Set HSION bit */
	RCC->CR |= 0x00000001U;

	/* Reset SW, HPRE, PPRE1, PPRE2, ADCPRE and MCO bits */
	RCC->CFGR &= 0xF8FF0000U;

	/* Reset HSEON, CSSON and PLLON bits */
	RCC->CR &= 0xFEF6FFFFU;

	/* Reset HSEBYP bit */
	RCC->CR &= 0xFFFBFFFFU;

	/* Reset PLLSRC, PLLXTPRE, PLLMUL and USBPRE/OTGFSPRE bits */
	RCC->CFGR &= 0xFF80FFFFU;

	/* Disable all interrupts and clear pending bits  */
	RCC->CIR = 0x009F0000U;
}

void RCC_init_clock()
{
	FLASH->ACR |= 2; // 1 wait state
	// 8MHz * 9 = 72MHz, adc requeires f <= 14MHz, closest value is 6:72/6=12
	RCC->CFGR = PLL_MUL | PLL_SRC_PREDIV1 | ADC_DIV | APB1_DIV;

	// turn on HSE (crystal)
	RCC->CR |= CLK_HSE_ON;
	WAIT((RCC->CR & CLK_HSE_RDY) == 0)

	// turn on PLL
	RCC->CR |= CLK_PLL_ON;
	WAIT((RCC->CR & CLK_PLL_RDY) == 0)

	// turn on CSS
	RCC->CR |= CLK_CSS_ON;

	// enable clock for HSE pins
	RCC_peripheral_set_clock(RCC_GPIOC, true);

	// Switch to PLL + HSE:
	RCC->CFGR |= CLK_SRC_SELECT_PLL;
	while (1)
	{
		if ((RCC->CFGR & CLK_SRC_CURRENT_MSK) == CLK_SRC_CURRENT_PLL)
		{
			break;
		}
	}
}

void RCC_reset_system()
{
	NVIC_SystemReset();
}
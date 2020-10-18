#include "RCC.h"

/* RCC definitions */
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

#define FLASH_R_BASE (AHBPERIPH_BASE + 0x00002000U) /*!< Flash registers base address */
#define RCC_BASE	 (AHBPERIPH_BASE + 0x00001000U)
#define RCC			 ((RCC_TypeDef *)RCC_BASE)

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
	uint32_t * clock_ptr = NULL;
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

	*clock_ptr = enable << (periph % CLOCK_DOMAIN_PERIPH_COUNT);
}

void RCC_peripheral_reset(RCC_Peripherals_t periph)
{
	uint32_t * reset_ptr = NULL;
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

	*reset_ptr = 1 << (periph % CLOCK_DOMAIN_PERIPH_COUNT);
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

// TODO: Fix missing defines, add get_clock_frequency

void RCC_init_clock()
{
	FLASH->ACR |= FLASH_ACR_LATENCY_1; // 2 wait states
	// 8MHz * 9 = 72MHz, adc requeires f <= 14MHz, closest value is 6:72/6=12
	RCC->CFGR = RCC_CFGR_PLLMULL9 | RCC_CFGR_ADCPRE_DIV6 | RCC_CFGR_PLLSRC;

	// turn on HSE (crystal)
	RCC->CR |= RCC_CR_HSEON;
	WAIT((RCC->CR & RCC_CR_HSERDY) == 0)

	// turn on PLL
	RCC->CR |= RCC_CR_PLLON;
	WAIT((RCC->CR & RCC_CR_PLLRDY) == 0)

	// turn on CSS
	RCC->CR |= RCC_CR_CSSON;

	// Switch to PLL + HSE:
	RCC->CFGR |= RCC_CFGR_SW_PLL;
	WAIT((RCC->CFGR & RCC_CFGR_SWS_PLL) == 0)
}
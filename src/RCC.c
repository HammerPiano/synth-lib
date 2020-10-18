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

#define RCC_BASE (AHBPERIPH_BASE + 0x00001000U)
#define RCC		 ((RCC_TypeDef *)RCC_BASE)

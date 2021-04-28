#include "USART.h"

typedef struct
{
  __IO uint32_t SR;         /*!< USART Status register,                   Address offset: 0x00 */
  __IO uint32_t DR;         /*!< USART Data register,                     Address offset: 0x04 */
  __IO uint32_t BRR;        /*!< USART Baud rate register,                Address offset: 0x08 */
  __IO uint32_t CR1;        /*!< USART Control register 1,                Address offset: 0x0C */
  __IO uint32_t CR2;        /*!< USART Control register 2,                Address offset: 0x10 */
  __IO uint32_t CR3;        /*!< USART Control register 3,                Address offset: 0x14 */
  __IO uint32_t GTPR;       /*!< USART Guard time and prescaler register, Address offset: 0x18 */
} USART_TypeDef;

#define USART1_BASE           (APB2PERIPH_BASE + 0x00003800U)
#define USART2_BASE           (APB1PERIPH_BASE + 0x00004400U)
#define USART3_BASE			  (APB1PERIPH_BASE + 0x00004800)

#define USART1              ((USART_TypeDef *)USART1_BASE)
#define USART2              ((USART_TypeDef *)USART2_BASE)
#define USART3              ((USART_TypeDef *)USART3_BASE)

/**
 * @brief Get the usart object ptr 
 * 
 * @param usart_num which usart
 * @return USART_TypeDef* pointer to object
 */
static USART_TypeDef * get_usart_ptr(USART_NUMBER_e usart_num)
{
	if (usart_num == USART_NO_1)
	{
		return USART1;
	}
	else if (usart_num == USART_NO_2)
	{
		return USART2;
	}
	else if (usart_num == USART_NO_3)
	{
		return USART3;
	}
	return NULL;
}

/**
 * @brief This function calculates the value to store in the BRR (Baud rate register)
 * 
 * @param usart_num which USART
 * @param baud_rate desired baud rate
 * @return uint16_t value to store
 */
static uint16_t calculate_baud_rate_reg_value(USART_NUMBER_e usart_num, uint32_t baud_rate)
{
	uint32_t periph_clock = 0;
	// * Usart number 1 has a clock that runs at 72MHz, the others has clock that runs at 36MHz
	if (usart_num == USART_NO_1)
	{
		periph_clock = F_CPU;
	}
	else
	{
		periph_clock = F_CPU / 2;
	}
	/*
	The register is made out of 2 parts: 12bits 'mantissa', 4bits fraction
	Meaning, the high 12 bits are the whole part, the lower 4bits are the fraction (in sixteenths)
	baud_rate = (F_clk) / (16 * BRR)
	Therefor:
	BRR = (f_clk) / (16 * baud_rate)
	But, this value represents the sampling rate, not the actuall tranmission rate.
	Sampling rate is 16 times higher than transmission rate, and there for the 16 cancels out
	And we left only with the following:
	*/
	return periph_clock / baud_rate; // ? This
}

/**
 * @brief This function will initialize the given usart interface
 * 
 * @param usart_num which interface to init
 * @param baud_rate what baud rate to set
 * @param flags any flags set by the USART_CONF_FLAG_*
 * @return true initializaion complete
 * @return false initializaion failure
 */
bool USART_init(USART_NUMBER_e usart_num, uint32_t baud_rate, uint32_t flags)
{
	USART_TypeDef * usart = NULL;
	// If all flags are off, nothing is being set, error...
	if (flags == 0)
	{
		return false;
	}
	usart = get_usart_ptr(usart_num);
	if (usart == NULL)
	{
		return false;
	}

	if (baud_rate > USART_BAUD_RATE_MAX)
	{
		return false;
	}
	

}

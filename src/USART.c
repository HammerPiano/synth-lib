#include "USART.h"

#include "RCC.h"

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

#define USART_ON	(0x00002000)

/*
 ? Static functions
*/

static void activate_clock(USART_NUMBER_t usart_num)
{
	RCC_Peripherals_t rcc_periph = 0;
	if (usart_num == USART_NO_1)
	{
		rcc_periph = RCC_UART1;
	}
	else if (usart_num == USART_NO_2)
	{
		rcc_periph = RCC_UART2;
	}
	else if (usart_num == USART_NO_3)
	{
		rcc_periph = RCC_UART3;
	}
	else
	{
		return;
	}
	RCC_peripheral_set_clock(rcc_periph, true);
}

/**
 * @brief Get the usart object ptr 
 * 
 * @param usart_num which usart
 * @return USART_TypeDef* pointer to object
 */
static USART_TypeDef * get_usart_ptr(USART_NUMBER_t usart_num)
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
static uint16_t calculate_baud_rate_reg_value(USART_NUMBER_t usart_num, uint32_t baud_rate)
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

/*
 ? Exported functions
*/

DMA_CH_PERIPHERALS_t USART_to_dma_channel(USART_NUMBER_t usart_num, bool tx)
{
	DMA_CH_PERIPHERALS_t dma_ch = DMA_CH_INVALID;
	if (usart_num == USART_NO_1)
	{
		dma_ch = tx ? DMA_CH4_USART1_TX : DMA_CH5_USART1_RX;
	}
	else if (usart_num == USART_NO_2)
	{
		dma_ch = tx ? DMA_CH7_USART2_TX : DMA_CH6_USART2_RX;
	}
	else if (usart_num == USART_NO_3)
	{
		dma_ch = tx ? DMA_CH2_USART3_TX : DMA_CH3_USART3_RX;
	}
	return dma_ch;
}

bool USART_init(USART_NUMBER_t usart_num, uint32_t baud_rate, uint32_t flags)
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

	// 9 bits without parity isn't supported
	if ((flags & USART_CONF_FLAG_WORD_9_BIT) && (flags & USART_CONF_FLAG_PARITY_ON) == 0)
	{
		return false;
	}

	// 8 bits with parity isn't supported
	if ((flags & USART_CONF_FLAG_WORD_MASK) == 0 && flags & USART_CONF_FLAG_PARITY_ON)
	{
		return false;
	}
	activate_clock(usart_num);

	usart->BRR = calculate_baud_rate_reg_value(usart_num, baud_rate);
	usart->CR1 = flags & 0xFFFF; // Lower 16 bits are for CR1, higher are for CR2
	usart->CR2 = flags >> 16;
	usart->CR1 |= USART_ON;

	return true;
}

void USART_byte_write(USART_NUMBER_t usart_num, uint8_t data)
{
	USART_TypeDef * usart = NULL;
	usart = get_usart_ptr(usart_num);
	if (usart == NULL)
	{
		return;
	}
	// Transmitter is off, cannot send any data
	if ((usart->CR1 & USART_CONF_FLAG_TX_ON) == 0)
	{
		return;
	}
	usart->DR = data;
}

void USART_data_write(USART_NUMBER_t usart_num, const void * data, uint32_t data_size)
{
	const uint8_t * buffer = (const uint8_t *)data;

	// Check if usart num is valid
	if (get_usart_ptr(usart_num) == NULL)
	{
		return;
	}
	for (size_t i = 0; i < data_size; i++)
	{
		USART_byte_write(usart_num, buffer[i]);
		if (i < (data_size - 1))
		{
			WAIT(USART_get_flag(usart_num, USART_STAT_FLAG_TX_EMPTY) == 0);
		}
	}
	
}

bool USART_get_flag(USART_NUMBER_t usart_num, uint32_t flag)
{
	USART_TypeDef * usart = get_usart_ptr(usart_num);
	if (usart == NULL)
	{
		return false;
	}
	return usart->SR & flag;
}
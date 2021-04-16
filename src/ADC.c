#include "ADC.h"
#include "DMA.h"
#include "RCC.h"
#include "utils.h"
typedef struct
{
	__IO uint32_t SR;
	__IO uint32_t CR1;
	__IO uint32_t CR2;
	__IO uint32_t SMPR1;
	__IO uint32_t SMPR2;
	__IO uint32_t JOFR1;
	__IO uint32_t JOFR2;
	__IO uint32_t JOFR3;
	__IO uint32_t JOFR4;
	__IO uint32_t HTR;
	__IO uint32_t LTR;
	__IO uint32_t SQR1;
	__IO uint32_t SQR2;
	__IO uint32_t SQR3;
	__IO uint32_t JSQR;
	__IO uint32_t JDR1;
	__IO uint32_t JDR2;
	__IO uint32_t JDR3;
	__IO uint32_t JDR4;
	__IO uint32_t DR;
} ADC_TypeDef;

#define ADC1_BASE (APB2PERIPH_BASE + 0x00002400U)
#define ADC1	  ((ADC_TypeDef *)ADC1_BASE)

#define SEQUENCE_REG_CH_COUNT (6)
#define SEQUENCE_REG_3_MIN_CH (0)
#define SEQUENCE_REG_3_MAX_CH (SEQUENCE_REG_3_MIN_CH + SEQUENCE_REG_CH_COUNT - 1)
#define SEQUENCE_REG_2_MIN_CH (SEQUENCE_REG_3_MIN_CH + SEQUENCE_REG_CH_COUNT)
#define SEQUENCE_REG_2_MAX_CH (SEQUENCE_REG_2_MIN_CH + SEQUENCE_REG_CH_COUNT - 1)
#define SEQUENCE_REG_1_MIN_CH (SEQUENCE_REG_2_MIN_CH + SEQUENCE_REG_CH_COUNT)
#define SEQUENCE_REG_1_MAX_CH (SEQUENCE_REG_1_MIN_CH + SEQUENCE_REG_CH_COUNT - 1)
#define SEQUENCE_CH_SIZE	  (5)
#define SEQUENCE_LEN_OFFSET	  (20)
#define SEQUENCE_LEN_SIZE	  (4)

#define SAMPLING_REG_CH_COUNT (10)
#define SAMPLING_REG_2_MIN_CH (0)
#define SAMPLING_REG_2_MAX_CH (SAMPLING_REG_2_MIN_CH + SAMPLING_REG_CH_COUNT - 1)
#define SAMPLING_REG_1_MIN_CH (SAMPLING_REG_2_MIN_CH + SAMPLING_REG_CH_COUNT)
#define SAMPLING_REG_1_MAX_CH (SAMPLING_REG_1_MIN_CH + SAMPLING_REG_CH_COUNT - 1)
#define SAMPLING_TIME_SIZE	  (3)

#define ADC_CR2_ADON (0)
#define ADC_CR2_CAL	 (2)
#define ADC_CR1_CONT (1)
#define ADC_CR2_SCAN (8)
#define ADC_CR2_DMA	 (8)
static bool s_adc1_init = false;

/**
 * @brief This function starts up the ADC
 *
 */
static void startup_adc()
{
	// ! Start the clocks!!
	RCC_peripheral_set_clock(RCC_ADC1, true);

	ADC1->CR2 = 1 << ADC_CR2_ADON;		  // Enable ADC, ADON bit 0
	ADC1->CR2 |= 1 << ADC_CR2_CAL;		  // Calibrate
	WAIT(ADC1->CR2 & (1 << ADC_CR2_CAL)); // Wait for calibration to end
	ADC1->CR2 |= 1 << ADC_CR2_DMA;
}

/**
 * @brief Set the up dma channel for the ADC
 *
 * @param output where shall the DMA store the data
 * @param count_channels how much data is allocated
 */
static bool setup_dma_channel(uint16_t * output, uint8_t count_channels)
{
	RCC_peripheral_set_clock(RCC_DMA1, true);
	DMA_address_t periph = { .access_size = DMA_ACCESS_16BIT, .address = (uint32_t *)ADC_get_data_register(), .increament_address = false };
	DMA_address_t memory = { .access_size = DMA_ACCESS_16BIT, .address = output, .increament_address = true };
	if (DMA_init_channel(DMA_CH1_ADC1, &periph, &memory, DMA_CH_PRIORITY_HIGH, DMA_DIRECTION_PERIPH_TO_MEM, 0) == false)
	{
		// Is the channel reserved?
		return false;
	}
	return true;
}

/**
 * @brief Set the channel in the sequence selection registers
 *
 * @param sequence_index channel index in sequence
 * @param channel which channel to measure
 */
static void set_channel_sequence_index(uint8_t sequence_index, uint8_t channel)
{
	periph_ptr_t sequence_register = NULL;
	uint8_t		 start_bit		   = SEQUENCE_CH_SIZE * (sequence_index % SEQUENCE_REG_CH_COUNT);
	if (channel > MAX_CHANNEL || sequence_index > MAX_SEQUENCE)
	{
		return;
	}

	if (sequence_index <= SEQUENCE_REG_3_MAX_CH)
	{
		sequence_register = &ADC1->SQR3;
	}
	else if (sequence_index <= SEQUENCE_REG_2_MAX_CH)
	{
		sequence_register = &ADC1->SQR2;
	}
	else if (sequence_index <= SEQUENCE_REG_1_MAX_CH)
	{
		sequence_register = &ADC1->SQR1;
	}
	else
	{
		return; // Invalid channel
	}
	// clear previous selection
	*sequence_register &= ~(utils_generate_mask(start_bit, start_bit + SEQUENCE_CH_SIZE));
	*sequence_register |= channel << start_bit;
}

static void set_channel_sampling_time(uint8_t channel, ADC_SAMPLING_TIME_t sampling_time)
{
	// Max sampling rate is 239.5 adc clock cycles
	periph_ptr_t sampling_register = NULL;
	uint8_t		 start_bit		   = SAMPLING_TIME_SIZE * (channel % SAMPLING_REG_CH_COUNT);
	if (channel > MAX_CHANNEL || sampling_time > ADC_SAMPLING_239_5)
	{
		return;
	}
	if (channel <= SAMPLING_REG_2_MAX_CH)
	{
		sampling_register = &ADC1->SMPR2;
	}
	else if (channel <= SAMPLING_REG_1_MAX_CH)
	{
		sampling_register = &ADC1->SMPR1;
	}
	else
	{
		return;
	}
	*sampling_register &= ~(utils_generate_mask(start_bit, start_bit + SAMPLING_TIME_SIZE));
	*sampling_register |= sampling_time << start_bit;
}

static void set_sequence_channel_count(uint8_t channel_count)
{
	if (channel_count > MAX_SEQUENCE)
	{
		return;
	}
	ADC1->SQR1 &= ~(utils_generate_mask(SEQUENCE_LEN_OFFSET, SEQUENCE_LEN_OFFSET + SEQUENCE_LEN_SIZE - 1));
	// the value is count x times + 1
	ADC1->SQR1 |= (channel_count - 1) << SEQUENCE_LEN_OFFSET;
}

/*
 ? Public functions
*/

bool ADC_init(uint8_t * channels, uint8_t count_channels, uint16_t * output)
{
	if (s_adc1_init)
	{
		return false;
	}
	startup_adc();
	if (setup_dma_channel(output, count_channels) == false)
	{
		return false;
	}
	for (size_t i = 0; i < count_channels; i++)
	{
		set_channel_sampling_time(channels[i], ADC_DEFAULT_SAMPLING_TIME);
		set_channel_sequence_index(i, channels[i]);
	}
	set_sequence_channel_count(count_channels);
	s_adc1_init = true;
	return true;
}

bool ADC_init_ex(uint8_t * channels, ADC_SAMPLING_TIME_t * channels_sampling_time, uint8_t count_channels, uint16_t * output)
{
	if (s_adc1_init)
	{
		return false;
	}
	startup_adc();
	if (setup_dma_channel(output, count_channels) == false)
	{
		return false;
	}
	for (size_t i = 0; i < count_channels; i++)
	{
		set_channel_sampling_time(channels[i], channels_sampling_time[i]);
		set_channel_sequence_index(i, channels[i]);
	}
	set_sequence_channel_count(count_channels);
	s_adc1_init = true;
	return true;
}

bool ADC_get_flag(ADC_flag_t flag)
{
	return ADC1->SR & flag;
}

periph_ptr_t ADC_get_data_register()
{
	return &ADC1->DR;
}

void ADC_start(ADC_mode_t mode, uint8_t count_channels)
{
	if (!s_adc1_init)
	{
		return; // ADC isn't ready
	}
	switch (mode)
	{
	case ADC_mode_loop:
		ADC1->CR2 |= 1 << ADC_CR1_CONT; // enable continius mode
	case ADC_mode_single:
		ADC1->CR1 |= 1 << ADC_CR2_SCAN; // enable scan mode
		break;
	default:
		return;
		break;
	}
	DMA_start_channel(DMA_CH1_ADC1, count_channels, false);
	ADC1->CR2 |= 1 << ADC_CR2_ADON; // ADC on!
}

void ADC_stop()
{
	if (!s_adc1_init)
	{
		return; // ADC isn't ready
	}
	ADC1->CR2 &= ~0x1; // ADC off!
}

void ADC_startup()
{
	s_adc1_init = false;
}
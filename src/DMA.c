#include "DMA.h"
#include "RCC.h"
#include "USART.h"

#define DMA1_BASE		   (AHBPERIPH_BASE + 0x00000000U)
#define DMA1_Channels_BASE (AHBPERIPH_BASE + 0x00000008U)
#define DMA1_END		   (DMA1_BASE + 0x00000400U)

#define DMA_CCR_SOFT_TRIG	(14)
#define DMA_CCR_PRIORITY	(12)
#define DMA_CCR_MEMORY_SIZE (10)
#define DMA_CCR_PERIPH_SIZE (8)
#define DMA_CCR_MEMORY_INC	(7)
#define DMA_CCR_PERIPH_INC	(6)
#define DMA_CCR_DIRECTION	(4)
#define DMA_CCR_INTERRUPTS	(1)
#define DMA_CCR_ENABLE		(0)

#define DMA_ISR_FLAG_PER_CHANNEL (4)

typedef struct
{
	__IO uint32_t ISR;
	__IO uint32_t IFCR;
} DMA_COMMON_t;

typedef struct
{
	__IO uint32_t CCR;
	__IO uint32_t CNDTR;
	__IO uint32_t CPAR;
	__IO uint32_t CMAR;
	__IO uint32_t reserved;
} DMA_CH_CONFIG_t;

#define s_DMA_CHANNELS ((DMA_CH_CONFIG_t *)DMA1_Channels_BASE)
#define s_DMA1		   ((DMA_COMMON_t *)DMA1_BASE)

static DMA_CH_PERIPHERALS_t s_reserved_channels[DMA_CH_COUNT] = {DMA_CH_INVALID};

/*
 ? Channel usage monitoring functions
*/

/**
 * @brief This function will clear all interrupts flags for the given channel
 * 
 * @param channel 
 */
static void clear_dma_irq_flags(DMA_CHANNELS_t ch_num)
{
	s_DMA1->IFCR |= DMA_INTERRUPT_ALL << (ch_num * DMA_ISR_FLAG_PER_CHANNEL + 1);
}

/**
 * @brief This function will check if the given channel is reserved
 *
 * @param channel channel number
 * @return true channel is reserved
 * @return false channel is free
 */
static bool is_channel_reserved(DMA_CH_PERIPHERALS_t channel)
{
	return s_reserved_channels[DMA_CH_DECODE(channel)] != DMA_CH_INVALID;
}

/**
 * @brief This function will try to reserve a channel, if it is free
 *
 * @param channel channel to reserve
 * @return true channel successfully reserved
 * @return false channel already reserved
 */
static bool reserve_channel(DMA_CH_PERIPHERALS_t channel)
{
	if (is_channel_reserved(channel))
	{
		return false;
	}
	s_reserved_channels[DMA_CH_DECODE(channel)] = channel;
	return true;
}

/**
 * @brief This function will free the given channel
 *
 * @param channel channel to free
 */
static void free_channel(DMA_CH_PERIPHERALS_t channel)
{
	s_reserved_channels[DMA_CH_DECODE(channel)] = DMA_CH_INVALID;
}

/*
 ? Generic static functions
*/

/**
 * @brief Get the channel object
 *
 * @param channel_config channel number
 * @return DMA_CH_CONFIG_t* address of channel, NULL incase of faliure
 */
static DMA_CH_CONFIG_t * get_channel(DMA_CH_PERIPHERALS_t channel_config)
{
	DMA_CH_CONFIG_t * channel = NULL;
	DMA_CHANNELS_t ch_num = DMA_CH_DECODE(channel_config);
	if (ch_num >= DMA_CH_COUNT)
	{
		return NULL;
	}
	channel = s_DMA_CHANNELS + ch_num;
	if (channel > (DMA_CH_CONFIG_t *)DMA1_END) /* should never happen */
	{
		return NULL;
	}
	return channel;
}

bool DMA_init_channel(DMA_CH_PERIPHERALS_t	channel_config,
					  const DMA_address_t * peripheral,
					  const DMA_address_t * memory,
					  uint32_t				priority,
					  uint32_t				direction,
					  uint32_t				interrupt_mask)
{
	DMA_CH_CONFIG_t * channel = get_channel(channel_config);
	if (channel == NULL) /* should never happen */
	{
		return false;
	}

	if (reserve_channel(channel_config) == false)
	{
		return false;
	}
	/* initialize channel configuration register */
	channel->CCR = priority << DMA_CCR_PRIORITY;
	channel->CCR |= (memory->access_size << DMA_CCR_MEMORY_SIZE) | (memory->increament_address << DMA_CCR_MEMORY_INC);
	channel->CCR |= (peripheral->access_size << DMA_CCR_PERIPH_SIZE) | (peripheral->increament_address << DMA_CCR_PERIPH_INC);
	channel->CCR |= direction << DMA_CCR_DIRECTION;
	channel->CCR |= interrupt_mask;

	/* initialize addresses */
	channel->CMAR = (volatile uint32_t) memory->address;
	channel->CPAR = (volatile uint32_t) peripheral->address;
	// TODO make proper NVIC module implementation
	if (interrupt_mask != 0)
	{
		NVIC_SetPriority(DMA_CH_DECODE(channel_config) + DMA1_Channel1_IRQn, 0);
		NVIC_EnableIRQ(DMA_CH_DECODE(channel_config) + DMA1_Channel1_IRQn);
	}
	return true;
}

bool DMA_de_init_channel(DMA_CH_PERIPHERALS_t channel_config)
{
	DMA_CH_CONFIG_t * channel = get_channel(channel_config);
	if (channel == NULL) /* should never happen */
	{
		return false;
	}

	free_channel(channel_config);
	channel->CCR   = 0;
	channel->CMAR  = 0;
	channel->CPAR  = 0;
	channel->CNDTR = 0;
	// TODO make proper NVIC module implementation
	NVIC_DisableIRQ(DMA_CH_DECODE(channel_config) + DMA1_Channel1_IRQn);
	return true;
}

bool DMA_start_channel(DMA_CH_PERIPHERALS_t channel_config, uint16_t data_count, bool blocking)
{
	DMA_CH_CONFIG_t * channel = get_channel(channel_config);
	if (channel == NULL) /* should never happen */
	{
		return false;
	}
	if (data_count == 0)
	{
		return false;
	}
	channel->CNDTR = data_count;
	if (blocking)
	{
		while (!DMA_channel_get_flag(channel_config, DMA_FLAG_FINISHED))
		{
			asm("nop");
		}
	}

	/* ready for enabling! */
	channel->CCR |= 1 << DMA_CCR_ENABLE;
	return true;
}

bool DMA_stop_channel(DMA_CH_PERIPHERALS_t channel_config)
{
	DMA_CH_CONFIG_t * channel = get_channel(channel_config);
	if (channel == NULL) /* should never happen */
	{
		return false;
	}
	channel->CCR &= ~(1 << DMA_CCR_ENABLE);
	return true;
}

bool DMA_set_software_trigger(DMA_CH_PERIPHERALS_t channel_config, bool software_trigger)
{
	DMA_CH_CONFIG_t * channel = get_channel(channel_config);
	if (channel == NULL) /* should never happen */
	{
		return false;
	}
	if (software_trigger)
	{
		channel->CCR |= 1 << DMA_CCR_SOFT_TRIG;
	}
	else
	{
		channel->CCR &= ~(1 << DMA_CCR_SOFT_TRIG);
	}
	return true;
}

bool DMA_channel_get_flag(DMA_CH_PERIPHERALS_t channel_config, DMA_FLAG_t flag)
{
	uint32_t flag_mask = 0;
	DMA_CHANNELS_t ch_num = DMA_CH_DECODE(channel_config);
	if (ch_num > DMA_CH_COUNT)
	{
		return false;
	}
	flag_mask = flag << (ch_num * DMA_ISR_FLAG_PER_CHANNEL);
	return s_DMA1->ISR & flag_mask;
}

bool DMA_channel_clear_flags(DMA_CH_PERIPHERALS_t channel_config)
{
	DMA_CHANNELS_t ch_num = DMA_CH_DECODE(channel_config);
	if (ch_num > DMA_CH_COUNT)
	{
		return false;
	}
	s_DMA1->IFCR |= DMA_FLAG_ALL << (ch_num * DMA_ISR_FLAG_PER_CHANNEL);
	return true;
}

void DMA_startup()
{
	for (size_t i = 0; i < DMA_CH_COUNT; i++)
	{
		s_reserved_channels[i] = DMA_CH_INVALID;
	}
	
	RCC_peripheral_set_clock(RCC_DMA1, true);
}

/*void DMA1_Channel1_IRQHandler()
{
	switch (s_reserved_channels[0])
	{

	}
}*/
void DMA1_Channel2_IRQHandler()
{
	switch (s_reserved_channels[1])
	{
		case DMA_CH2_USART3_TX:
			USART_DMA_handler(USART_NO_3, true);		
			break;
		default:
		break;
	}
}
void DMA1_Channel3_IRQHandler()
{
	switch (s_reserved_channels[2])
	{
		case DMA_CH3_USART3_RX:
			USART_DMA_handler(USART_NO_3, false);		
			break;
		default:
		break;
	}
}
void DMA1_Channel4_IRQHandler()
{
	switch (s_reserved_channels[3])
	{
		case DMA_CH4_USART1_TX:
			USART_DMA_handler(USART_NO_1, true);		
			break;
		default:
		break;
	}
}
void DMA1_Channel5_IRQHandler()
{
	switch (s_reserved_channels[4])
	{
		case DMA_CH5_USART1_RX:
			USART_DMA_handler(USART_NO_1, false);		
			break;
		default:
		break;
	}
}
void DMA1_Channel6_IRQHandler()
{
	switch (s_reserved_channels[5])
	{
		case DMA_CH6_USART2_RX:
			USART_DMA_handler(USART_NO_2, false);		
			break;
		default:
		break;
	}
}
void DMA1_Channel7_IRQHandler()
{
	switch (s_reserved_channels[6])
	{
		case DMA_CH7_USART2_TX:
			USART_DMA_handler(USART_NO_2, true);		
			break;
		default:
		break;
	}
}
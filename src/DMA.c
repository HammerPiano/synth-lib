#include "DMA.h"
#include "RCC.h"

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

#define DMA_ISR_FLAG_PER_CHANNEL (3)

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
} DMA_CH_CONFIG_t;

#define s_DMA_CHANNELS ((DMA_CH_CONFIG_t *)DMA1_Channels_BASE)
#define s_DMA1		   ((DMA_COMMON_t *)DMA1_BASE)

static uint8_t s_reserved_channels = 0;

/*
 ? Channel usage monitoring functions
*/

/**
 * @brief This function will check if the given channel is reserved
 *
 * @param channel channel number
 * @return true channel is reserved
 * @return false channel is free
 */
static bool is_channel_reserved(DMA_CHANNELS_t channel)
{
	return s_reserved_channels & (1 << channel);
}

/**
 * @brief This function will try to reserve a channel, if it is free
 *
 * @param channel channel to reserve
 * @return true channel successfully reserved
 * @return false channel already reserved
 */
static bool reserve_channel(DMA_CHANNELS_t channel)
{
	if (is_channel_reserved(channel))
	{
		return false;
	}
	s_reserved_channels |= 1 << channel;
	return true;
}

/**
 * @brief This function will free the given channel
 *
 * @param channel channel to free
 */
static void free_channel(DMA_CHANNELS_t channel)
{
	s_reserved_channels &= ~(1 << channel);
}

/*
 ? Generic static functions
*/

/**
 * @brief Get the channel object
 *
 * @param dma_channel_number channel number
 * @return DMA_CH_CONFIG_t* address of channel, NULL incase of faliure
 */
static DMA_CH_CONFIG_t * get_channel(DMA_CHANNELS_t dma_channel_number)
{
	DMA_CH_CONFIG_t * channel = NULL;
	if (dma_channel_number >= DMA_CH_COUNT)
	{
		return NULL;
	}
	channel = s_DMA_CHANNELS + dma_channel_number;
	if (channel > DMA1_END) /* should never happen */
	{
		return NULL;
	}
	return channel;
}

bool DMA_init_channel(DMA_CHANNELS_t		dma_channel_number,
					  const DMA_address_t * peripheral,
					  const DMA_address_t * memory,
					  uint32_t				priority,
					  uint32_t				direction,
					  uint32_t				interrupt_mask)
{
	DMA_CH_CONFIG_t * channel = get_channel(dma_channel_number);
	if (channel == NULL) /* should never happen */
	{
		return false;
	}

	if (reserve_channel(dma_channel_number) == false)
	{
		return false;
	}
	/* initialize channel configuration register */
	channel->CCR = priority << DMA_CCR_PRIORITY;
	channel->CCR |= (memory->access_size << DMA_CCR_MEMORY_SIZE) | (memory->increament_address << DMA_CCR_MEMORY_INC);
	channel->CCR |= (peripheral->access_size << DMA_CCR_PERIPH_SIZE) | (peripheral->increament_address << DMA_CCR_PERIPH_INC);
	channel->CCR |= direction << DMA_CCR_DIRECTION;
	channel->CCR |= interrupt_mask << DMA_CCR_INTERRUPTS;

	/* initialize addresses */
	channel->CMAR = memory->address;
	channel->CPAR = peripheral->address;

	return true;
}

bool DMA_de_init_channel(DMA_CHANNELS_t dma_channel_number)
{
	DMA_CH_CONFIG_t * channel = get_channel(dma_channel_number);
	if (channel == NULL) /* should never happen */
	{
		return false;
	}

	free_channel(dma_channel_number);
	channel->CCR   = 0;
	channel->CMAR  = 0;
	channel->CPAR  = 0;
	channel->CNDTR = 0;
	return true;
}

bool DMA_start_channel(DMA_CHANNELS_t dma_channel_number, uint16_t data_count, bool blocking)
{
	DMA_CH_CONFIG_t * channel = get_channel(dma_channel_number);
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
		while (!DMA_channel_get_flag(dma_channel_number, DMA_FLAG_FINISHED))
		{
			asm("nop");
		}
	}

	/* ready for enabling! */
	channel->CCR |= 1 << DMA_CCR_ENABLE;
	return true;
}

bool DMA_stop_channel(DMA_CHANNELS_t dma_channel_number)
{
	DMA_CH_CONFIG_t * channel = get_channel(dma_channel_number);
	if (channel == NULL) /* should never happen */
	{
		return false;
	}
	channel->CCR &= ~(1 << DMA_CCR_ENABLE);
	return true;
}

bool DMA_set_software_trigger(DMA_CHANNELS_t dma_channel_number, bool software_trigger)
{
	DMA_CH_CONFIG_t * channel = get_channel(dma_channel_number);
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
}

bool DMA_channel_get_flag(DMA_CHANNELS_t dma_channel_number, DMA_FLAG_t flag)
{
	uint32_t flag_mask = 0;
	if (dma_channel_number > DMA_CH_COUNT)
	{
		return false;
	}
	flag_mask = flag << (dma_channel_number * DMA_ISR_FLAG_PER_CHANNEL + 1);
	return s_DMA1->ISR & flag_mask;
}

bool DMA_channel_clear_flags(DMA_CHANNELS_t dma_channel_number)
{
	if (dma_channel_number > DMA_CH_COUNT)
	{
		return false;
	}
	s_DMA1->IFCR |= DMA_FLAG_ALL << (dma_channel_number * DMA_ISR_FLAG_PER_CHANNEL);
	return true;
}

void DMA_startup()
{
	s_reserved_channels = 0;
	RCC_peripheral_set_clock(RCC_DMA1, true);
}
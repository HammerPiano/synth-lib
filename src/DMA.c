#include "DMA.h"

#define DMA1_BASE		   (AHBPERIPH_BASE + 0x00000000U)
#define DMA1_Channels_BASE (AHBPERIPH_BASE + 0x00000008U)

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

#define DMA_CHANNELS ((DMA_CH_CONFIG_t[7])DMA1_Channels_BASE)
#define DMA1		 ((DMA_TypeDef *)DMA1_BASE)

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
	if (is_channel_used())
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

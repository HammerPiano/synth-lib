#ifndef __DMA_H__
#define __DMA_H__

#include "common.h"

#define DMA_INTERRUPT_ERROR		(0x00000008)
#define DMA_INTERRUPT_HALF		(0x00000004)
#define DMA_INTERRUPT_COMPLETE	(0x00000002)


typedef enum
{
	DMA_CH1,
	DMA_CH2,
	DMA_CH3,
	DMA_CH4,	
	DMA_CH5,
	DMA_CH6,
	DMA_CH7

} DMA_CHANNELS_t;

typedef enum 
{
	/* DMA Channel 1 Sources */

	DMA_CH1_ADC1		= DMA_CH1,
	DMA_CH1_TIM2_CH3	= DMA_CH1,
	DMA_CH1_TIM4_CH1	= DMA_CH1,

	/* DMA Channel 2 Sources */

	DMA_CH2_SPI1_RX		= DMA_CH2,
	DMA_CH2_USART3_TX	= DMA_CH2,
	DMA_CH2_TIM1_CH1	= DMA_CH2,
	DMA_CH2_TIM2_UPDATE	= DMA_CH2,
	DMA_CH2_TIM3_CH3	= DMA_CH2,

	/* DMA Channel 3 Sources */
	DMA_CH3,
	DMA_CH3_SPI1_TX		= DMA_CH3,
	DMA_CH3_USART3_RX	= DMA_CH3,
	DMA_CH3_TIM1_CH2	= DMA_CH3,
	DMA_CH3_TIM3_CH4	= DMA_CH3, 
	DMA_CH3_TIM3_UPDATE	= DMA_CH3,

	/* DMA Channel 4 Sources */
	DMA_CH4_SPI2_RX		= DMA_CH4,
	DMA_CH4_USART1_TX	= DMA_CH4,
	DMA_CH4_I2C2_TX		= DMA_CH4,
	DMA_CH4_TIM1_CH4	= DMA_CH4,
	DMA_CH4_TIM1_TRIG	= DMA_CH4,
	DMA_CH4_TIM1_COM	= DMA_CH4,
	DMA_CH4_TIM4_CH2	= DMA_CH4,

	/* DMA Channel 5 Sources */
	DMA_CH5_SPI2_TX		= DMA_CH5,
	DMA_CH5_USART1_RX	= DMA_CH5,
	DMA_CH5_I2C2_RX		= DMA_CH5,
	DMA_CH5_TIM1_UPDATE	= DMA_CH5,
	DMA_CH5_TIM2_CH1	= DMA_CH5,
	DMA_CH5_TIM4_CH3	= DMA_CH5,

	/* DMA Channel 6 Sources */
	DMA_CH6_USART2_RX	= DMA_CH6,
	DMA_CH6_I2C1_TX		= DMA_CH6,
	DMA_CH6_TIM1_CH3	= DMA_CH6,
	DMA_CH6_TIM3_CH1	= DMA_CH6,
	DMA_CH6_TIM3_TRIG	= DMA_CH6,
	
	/* DMA Channel 7 Sources */
	DMA_CH7_USART2_TX	= DMA_CH7,
	DMA_CH7_I2C1_RX		= DMA_CH7,
	DMA_CH7_TIM2_CH2	= DMA_CH7,
	DMA_CH7_TIM2_CH4	= DMA_CH7,
	DMA_CH7_TIM4_UPDATE	= DMA_CH7
} DMA_CH_PERIPHERALS_t;

typedef enum
{
	DMA_CH_PRIORITY_LOW,
	DMA_CH_PRIORITY_MEDIUM,
	DMA_CH_PRIORITY_HIGH,
	DMA_CH_PRIORITY_VERY_HIGH	
} DMA_CH_PRIORITY_t;

typedef enum
{
	DMA_DIRECTION_PERIPH_TO_MEM,
	DMA_DIRECTION_MEM_TO_PERIPH
} DMA_DIRECTION;

typedef enum
{
	DMA_ACCESS_8BIT,
	DMA_ACCESS_16BIT,
	DMA_ACCESS_32BIT	
} DMA_ACCESS_TYPE_t;

typedef struct
{
	uint32_t address;
	uint8_t access_size:2;
	uint8_t increament_address:1;
}DMA_address_t, *pDMA_address_t;


/**
 * @brief This function will init the DMA channel
 * 
 * @param dma_channel which channel - can be specified as a channel, or which peripheral to trigger it
 * @param peripheral peripheral address
 * @param memory flash/ram address to read/write - flash is readonly!!!
 * @param priotiry which priority this gets - also lower channel number, high priority
 * @param direction from or to peripheral
 * @param interrupt_mask which interrupts to generate for the channel
 * @return true init successfull
 * @return false init not successfull
 */
bool DMA_init_channel(DMA_CHANNELS_t dma_channel, const DMA_address_t * peripheral, const DMA_address_t * memory, DMA_CH_PRIORITY_t priotiry, DMA_DIRECTION direction, uint8_t interrupt_mask);

/**
 * @brief This function reset the configuration for the given channel to the deafult ones
 * 
 * @param dma_channel which channel to reset
 * @return true reset successfull
 * @return false rest not successfull
 */
bool DMA_de_init_channel(DMA_CHANNELS_t dma_channel);

/**
 * @brief This function will start transphering data
 * 
 * @param dma_channel which channel to start
 * @param data_count how many bytes to transpher
 * @param blocking if this function should wait until transphere is complete or not
 * @return true transphere started successfully
 * @return false transphere not started successfully
 */
bool DMA_start_channel(DMA_CHANNELS_t dma_channel, uint16_t data_count, bool blocking);

/**
 * @brief This function will check if the transpher complete. It is not blocking
 * 
 * @param dma_channel which channel to check
 * @return true transphere complete
 * @return false transphere on-going
 * 
 * @remarks the function can be used for simulating multitasking
 */
bool DMA_channel_is_finished(DMA_CHANNELS_t dma_channel);

#endif /*__DMA_H__*/
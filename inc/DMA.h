#ifndef __DMA_H__
#define __DMA_H__

#include "common.h"

#define DMA_INTERRUPT_ERROR	   (0x00000008)
#define DMA_INTERRUPT_HALF	   (0x00000004)
#define DMA_INTERRUPT_COMPLETE (0x00000002)

#define DMA_CH_OFFSET		8
#define DMA_CH_ENCODE(ch)	(ch << DMA_CH_OFFSET)
#define DMA_CH_DECODE(ch)	(ch >> DMA_CH_OFFSET)
#define DMA_CH_SOURCE(ch)	(ch & 0xFF)

typedef enum
{
	DMA_CH1,
	DMA_CH2,
	DMA_CH3,
	DMA_CH4,
	DMA_CH5,
	DMA_CH6,
	DMA_CH7,
	DMA_CH_COUNT

} DMA_CHANNELS_t;

/**
 * @brief This enums encodes the channel with the source of it, meaning when a DMA interuppt has occured it is possile to now what has caused it
 * 
 * @note This implementation allows only one source at a time to access the DMA channels 
 */
typedef enum
{
	/* DMA Channel 1 Sources */
	DMA_CH1_ADC1	 = DMA_CH_ENCODE(DMA_CH1),
	DMA_CH1_TIM2_CH3,
	DMA_CH1_TIM4_CH1,

	/* DMA Channel 2 Sources */
	DMA_CH2_SPI1_RX		= DMA_CH_ENCODE(DMA_CH2),
	DMA_CH2_USART3_TX,
	DMA_CH2_TIM1_CH1,
	DMA_CH2_TIM2_UPDATE,
	DMA_CH2_TIM3_CH3,

	/* DMA Channel 3 Sources */
	DMA_CH3_SPI1_TX		= DMA_CH_ENCODE(DMA_CH3),
	DMA_CH3_USART3_RX,
	DMA_CH3_TIM1_CH2,
	DMA_CH3_TIM3_CH4,
	DMA_CH3_TIM3_UPDATE,

	/* DMA Channel 4 Sources */
	DMA_CH4_SPI2_RX	  = DMA_CH_ENCODE(DMA_CH4),
	DMA_CH4_USART1_TX,
	DMA_CH4_I2C2_TX,
	DMA_CH4_TIM1_CH4,
	DMA_CH4_TIM1_TRIG,
	DMA_CH4_TIM1_COM,
	DMA_CH4_TIM4_CH2,

	/* DMA Channel 5 Sources */
	DMA_CH5_SPI2_TX		= DMA_CH_ENCODE(DMA_CH5),
	DMA_CH5_USART1_RX,
	DMA_CH5_I2C2_RX,
	DMA_CH5_TIM1_UPDATE,
	DMA_CH5_TIM2_CH1,
	DMA_CH5_TIM4_CH3,

	/* DMA Channel 6 Sources */
	DMA_CH6_USART2_RX = DMA_CH_ENCODE(DMA_CH6),
	DMA_CH6_I2C1_TX,
	DMA_CH6_TIM1_CH3,
	DMA_CH6_TIM3_CH1,
	DMA_CH6_TIM3_TRIG,

	/* DMA Channel 7 Sources */
	DMA_CH7_USART2_TX	= DMA_CH_ENCODE(DMA_CH7),
	DMA_CH7_I2C1_RX,
	DMA_CH7_TIM2_CH2,
	DMA_CH7_TIM2_CH4,
	DMA_CH7_TIM4_UPDATE,

	DMA_CH_INVALID = 0xFFFF
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
	DMA_FLAG_FINISHED = 1,
	DMA_FLAG_HALF	  = 2,
	DMA_FLAG_ERROR	  = 4,
	DMA_FLAG_ALL	  = 7
} DMA_FLAG_t;

typedef enum
{
	DMA_ACCESS_8BIT,
	DMA_ACCESS_16BIT,
	DMA_ACCESS_32BIT
} DMA_ACCESS_TYPE_t;

typedef struct
{
	void *	address;
	uint8_t access_size : 2;
	uint8_t increament_address : 1;
} DMA_address_t, *pDMA_address_t;

/**
 * @brief This function will init the DMA channel
 *
 * @param dma_channel_number which channel - can be specified as a channel, or which peripheral to trigger it
 * @param peripheral peripheral address
 * @param memory flash/ram address to read/write - flash is readonly!!!
 * @param priority which priority this gets - also lower channel number, high priority
 * @param direction from or to peripheral
 * @param interrupt_mask which interrupts to generate for the channel
 * @return true init successfull
 * @return false init not successfull
 */
bool DMA_init_channel(DMA_CHANNELS_t		dma_channel_number,
					  const DMA_address_t * peripheral,
					  const DMA_address_t * memory,
					  uint32_t				priority,
					  uint32_t				direction,
					  uint32_t				interrupt_mask);

/**
 * @brief This function reset the configuration for the given channel to the deafult ones
 *
 * @param dma_channel_number which channel to reset
 * @return true reset successfull
 * @return false rest not successfull
 */
bool DMA_de_init_channel(DMA_CHANNELS_t dma_channel_number);

/**
 * @brief This function will start transfering data
 *
 * @param dma_channel_number which channel to start
 * @param data_count how many bytes to transfer
 * @param blocking if this function should wait until transfer is complete or not
 * @return true transfer started successfully
 * @return false transfer not started successfully
 */
bool DMA_start_channel(DMA_CHANNELS_t dma_channel_number, uint16_t data_count, bool blocking);

/**
 * @brief This function will stop the given DMA channel
 * 
 * @param dma_channel_number which channel to stop
 * @return true stop successfull
 * @return false stop not successfull
 */
bool DMA_stop_channel(DMA_CHANNELS_t dma_channel_number);

/**
 * @brief This function will enable or disable the automatic software trigger for data transfer
 *
 * @param dma_channel_number which DMA channel to change
 * @param software_trigger true of false
 * @return true no error
 * @return false error
 */
bool DMA_set_software_trigger(DMA_CHANNELS_t dma_channel_number, bool software_trigger);

/**
 * @brief This function will get the desired flag for the given channel
 *
 * @param dma_channel_number channel
 * @param flag flag to select
 * @return true flag is on
 * @return false flag is off
 *
 * @remarks to get if the channel is still tranfering, provide DMA_FLAG_FINISHED and check while it is zero
 */
bool DMA_channel_get_flag(DMA_CHANNELS_t dma_channel_number, DMA_FLAG_t flag);

/**
 * @brief This function will clear all flags for the given channel
 *
 * @param dma_channel_number channel to clear
 * @return true clear successfull
 * @return false clear failed
 */
bool DMA_channel_clear_flags(DMA_CHANNELS_t dma_channel_number);

/**
 * @brief This function is called on startup of the chip
 *
 */
void DMA_startup();

#endif /*__DMA_H__*/
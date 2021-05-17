#ifndef __USART_H__
#define __USART_H__
#include "common.h"
#include "DMA.h"

typedef enum
{
	USART_NO_1,
	USART_NO_2,
	USART_NO_3
} USART_NUMBER_t;

typedef enum
{
	USART_STOP_0_5,
	USART_STOP_1_0,
	USART_STOP_1_5,
	USART_STOP_2_0
}USART_STOP_BIT_t;

/*
 * Usart configuration flags
*/
#define USART_CONF_FLAG_RX_ON				(0x00000004)
#define USART_CONF_FLAG_TX_ON				(0x00000008)
#define USART_CONF_FLAG_PARITY_OFF			(0x00000000)
#define USART_CONF_FLAG_PARITY_EVEN			(0x00000200)
#define USART_CONF_FLAG_PARITY_ODD			(0x00000300)
#define USART_CONF_FLAG_PARITY_ON			(0x00000200)
#define USART_CONF_FLAG_WORD_8_BIT			(0x00000000)
#define USART_CONF_FLAG_WORD_9_BIT			(0x00001000)
#define USART_CONF_FLAG_WORD_MASK			(0x00001000)
#define USART_CONF_FLAG_STOP_BIT_MASK		(0x30000000)
#define USART_CONF_FLAG_STOP_BIT_CALC(x)	(x << 28)
/*
 * Usart status flags
*/
#define USART_STAT_FLAG_TX_EMPTY	(0x80)
#define USART_STAT_FLAG_TX_DONE		(0x40)
#define USART_STAT_FLAG_RX_FULL		(0x20)
#define USART_STAT_FLAG_PARITY_ERR	(0x01)

#define USART_BAUD_RATE_MAX			(2250000)
#define USART_BAUD_RATE_DEFAULT		(115200)

/**
 * @brief This function will initialize the given usart interface
 * 
 * @param usart_num which interface to init
 * @param baud_rate what baud rate to set
 * @param flags any flags set by the USART_CONF_FLAG_*
 * @return true initializaion complete
 * @return false initializaion failure
 * 
 * @remark This implementation supports either 8 bit no parity, or 9 bit with parity
 */
bool USART_init(USART_NUMBER_t usart_num, uint32_t baud_rate, uint32_t flags);

/**
 * @brief This function will write a byte to the USART peripheral
 * 
 * @param usart_num which usart to write
 * @param data what to send
 * 
 * @remark Non blocking, poll flag USART_STAT_FLAG_TX_DONE to see when done
 */
void USART_byte_write(USART_NUMBER_t usart_num, uint8_t data);

/**
 * @brief This function will write a buffer (in a blocking way) to the USART
 * 
 * @param usart_num which USART to write
 * @param data pointer to data
 * @param data_size how much data
 * 
 * @remark This function is blocking while it is writing to the USART
 */
void USART_data_write(USART_NUMBER_t usart_num, const void * data, uint32_t data_size);

/**
 * @brief This function will write a buffer using the DMA (without blocking) to the USART
 * 
 * @param usart_num which USART to write
 * @param data pointer to data
 * @param data_size how much data
 * @return true write successfull
 * @return false write failed (DMA is in use)
 */
bool USART_data_write_dma(USART_NUMBER_t usart_num, const void * data, uint32_t data_size);

/**
 * @brief This function will read from the USART
 * 
 * @param usart_num which USART to read from
 * @param blocking	should the usart wait for data or not
 * @return int16_t	data - 8 bit
 * 					error - -1
 */
int16_t USART_byte_read(USART_NUMBER_t usart_num, bool blocking);

/**
 * @brief This function will read to a buffer from the USART (it will block)
 * 
 * @param usart_num which USART to read from
 * @param data data output
 * @param data_size output size
 * @return true read was successfull
 * @return false read failed
 */
bool USART_data_read_dma(USART_NUMBER_t usart_num, void * data, uint32_t data_size);

/**
 * @brief This function will return the address of the data register of the USART
 * 
 * @param usart_num which USART to retrieve
 * @return periph_ptr_t address
 */
periph_ptr_t USART_get_data_register(USART_NUMBER_t usart_num);

/**
 * @brief This function will return a flag from the USART
 * 
 * @param usart_num which usart to check
 * @param flag any of the USART_STAT_FLAG_* defines
 * @return bool flag status (on/off)
 */
bool USART_get_flag(USART_NUMBER_t usart_num, uint32_t flag);

/**
 * @brief This function converts from usart number to the correct DMA channel (Depending on TX or RX)
 * 
 * @param usart_num which usart to check
 * @param tx tx channel (true) or rx channel (false)
 * @return DMA_CH_PERIPHERALS_t DMA channel to check
 */
DMA_CH_PERIPHERALS_t USART_to_dma_channel(USART_NUMBER_t usart_num, bool tx);

/**
 * @brief This function is the handler of all USART sourced DMA interrupts
 * 
 * @param usart_num which usart has caused a DMA interrupt
 * @param tx tx or not
 * 
 * ! This function is called automatically, do not call it directrly
 */
void USART_DMA_handler(USART_NUMBER_t usart_num, bool tx);


#endif // __USART_H__
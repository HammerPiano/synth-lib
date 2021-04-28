#ifndef __USART_H__
#define __USART_H__
#include "common.h"

typedef enum
{
	USART_NO_1,
	USART_NO_2,
	USART_NO_3
} USART_NUMBER_e;

typedef enum
{
	USART_STOP_0_5,
	USART_STOP_1_0,
	USART_STOP_1_5,
	USART_STOP_2_0
}USART_STOP_BIT_e;

/*
 * Usart configuration flags
*/
#define USART_CONF_FLAG_RX_ON		(0x04)
#define USART_CONF_FLAG_TX_ON		(0x08)
#define USART_CONF_FLAG_PARITY_OFF	(0x0)
#define USART_CONF_FLAG_PARITY_EVEN	(0x200)
#define USART_CONF_FLAG_PARITY_ODD	(0x300)
#define USART_CONF_FLAG_WORD_8_BIT	(0x0)
#define USART_CONF_FLAG_WORD_9_BIT	(0x1000)

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
 */
bool USART_init(USART_NUMBER_e usart_num, uint32_t baud_rate, uint32_t flags);

/**
 * @brief This function will write a byte to the USART peripheral
 * 
 * @param usart_num which usart to write
 * @param data what to send
 */
void USART_byte_write(USART_NUMBER_e usart_num, uint8_t data);

/**
 * @brief This function will write a buffer (in a blocking way) to the USART
 * 
 * @param usart_num which USART to write
 * @param data pointer to data
 * @param data_size how much data
 */
void USART_data_write(USART_NUMBER_e usart_num, const void * data, uint32_t data_size);

/**
 * @brief This function will write a buffer using the DMA (without blocking) to the USART
 * 
 * @param usart_num which USART to write
 * @param data pointer to data
 * @param data_size how much data
 * @return true write successfull
 * @return false write failed (DMA is in use)
 */
bool USART_data_write_dma(USART_NUMBER_e usart_num, const void * data, uint32_t data_size);

/**
 * @brief This function will read from the USART
 * 
 * @param usart_num which USART to read from
 * @return uint8_t data
 */
uint8_t USART_byte_read(USART_NUMBER_e usart_num);

/**
 * @brief This function will read to a buffer from the USART (it will block)
 * 
 * @param usart_num which USART to read from
 * @param data data output
 * @param data_size output size
 * @return true read was successfull
 * @return false read failed
 */
bool USART_data_read_dma(USART_NUMBER_e usart_num, void * data, uint32_t data_size);

/**
 * @brief This function will return the address of the data register of the USART
 * 
 * @param usart_num which USART to retrieve
 * @return periph_ptr_t address
 */
periph_ptr_t USART_get_data_register(USART_NUMBER_e usart_num);

/**
 * @brief This function will return a flag from the USART
 * 
 * @param usart_num which usart to check
 * @param flag any of the USART_STAT_FLAG_* defines
 * @return bool flag status (on/off)
 */
bool USART_get_flag(USART_NUMBER_e usart_num, uint32_t flag);
#endif // __USART_H__
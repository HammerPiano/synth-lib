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

typedef enum
{
	USART_FLAG_RX_ON = 0x04,
	USART_FLAG_TX_ON = 0x08,
	USART_FLAG_PARITY_OFF = 0x0,
	USART_FLAG_PARITY_EVEN = 0x200,
	USART_FLAG_PARITY_ODD = 0x300,
	USART_FLAG_8_BIT_WORD = 0,
	USART_FLAG_9_BIT_WORD = 0x1000
} USART_FLAG_e;

bool USART_init(USART_NUMBER_e usart_num, uint32_t baud_rate, uint32_t flags);

void USART_byte_write(USART_NUMBER_e usart_num, uint8_t data);

void USART_data_write(USART_NUMBER_e usart_num, void * data, uint32_t data_size);

uint8_t USART_byte_read(USART_NUMBER_e usart_num);

void USART_data_read(USART_NUMBER_e usart_num, void * data, uint32_t data_size);
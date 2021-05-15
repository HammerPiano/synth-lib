#include <string.h>
#include <stdio.h>

#include "ADC.h"
#include "DMA.h"
#include "GPIO.h"
#include "USART.h"

#define SPACE_LENGTH (720000)
#define DOT_LENGTH	 SPACE_LENGTH
#define DASH_LENGTH	 (3 * DOT_LENGTH)

#define LED_ON	(true)
#define LED_OFF (false)

#define LED_COUNT (7)
#define LED_X_START (0)
#define LED_X_END (LED_X_START + LED_COUNT - 1)
#define LED_X_PORT (GPIO_PORT_A)

#define LED_Y_START (5)
#define LED_Y_END (LED_Y_START + LED_COUNT - 1)
#define LED_Y_PORT (GPIO_PORT_B)
// 12-bit ADC
#define ADC_MIDDLE (2048)
// Seperate the 0-4095 range to 7 parts, we need 6 thresholds (TH)
#define ADC_LED_RANGE (4096 / LED_COUNT)



void delay(uint32_t ms)
{
	for (uint32_t i = 0; i < ms; i++)
	{
		// F_CPU ~= 72MHz, delay in MS
		for (size_t j = 0; j < 72000; j++)
		{
			asm("nop");
		}
	}
}
/*
The segments are marked from the top, clockwise a-f and center is g
a to A5, B to A6 etc.
Digits is common anode
									 0	   1	 2	   3	 4	   5	 6	   7	 8	  9		A	  B		C     D     E
*/
const uint16_t LED_7SEG_VALUES[] = { 0x40, 0x79, 0x24, 0x30, 0x19, 0x12, 0x02, 0x78, 0x0, 0x10, 0x08, 0x03, 0x46, 0x21, 0x06, 0x0E };

int main()
{
	char buffer[100] = {0};
	uint8_t	 adc_pins[] = {8, 9};// pins B0, B1 are inputs 8,9 of the ADC1
	uint16_t 		 adc_data[2] = {0}, counter = 0; // X and Y values of potentiometer
	GPIO_PIN_ARRAY_t x_bargraph	= { 0 };
	GPIO_PIN_ARRAY_t y_bargraph	= { 0 };
	GPIO_PIN_ARRAY_t adc_inputs = { 0 };
	GPIO_PIN_ARRAY_t usart_pins = { 0 };
	GPIO_array_init(&x_bargraph, LED_X_PORT, LED_X_START, LED_X_END, GPIO_MODE_OUTPUT, GPIO_CONFIG_OUTPUT_PUSH_PULL);
	GPIO_array_init(&y_bargraph, LED_Y_PORT, LED_Y_START, LED_Y_END, GPIO_MODE_OUTPUT, GPIO_CONFIG_OUTPUT_PUSH_PULL);
	GPIO_array_init(&adc_inputs, GPIO_PORT_B, 0, 1, GPIO_MODE_INPUT, GPIO_CONFIG_INPUT_ANALOG);
	GPIO_array_init(&usart_pins, GPIO_PORT_A, 9, 10, GPIO_MODE_OUTPUT, GPIO_CONFIG_OUTPUT_PUSH_PULL_ALT);
	ADC_init(adc_pins, sizeof(adc_pins) / sizeof(adc_pins[0]), adc_data);
	USART_init(USART_NO_1, USART_BAUD_RATE_DEFAULT, USART_CONF_FLAG_TX_ON);
	GPIO_array_write_all(&y_bargraph, 0);
	GPIO_array_write_all(&x_bargraph, 0);
	while (1)
	{
		// each button press increase the index one time only
		ADC_start(ADC_mode_single, 2);
		// The EOF flag gets reset by the ADC everytime the DMA reads it, a kind of race condition
		// Read the DMA finished flag instead
		while (DMA_channel_get_flag(DMA_CH1_ADC1, DMA_FLAG_FINISHED) == 0)
		{
			asm ("nop");
		}
		DMA_stop_channel(DMA_CH1_ADC1);
		
		GPIO_array_write_value(&x_bargraph, 1 << (adc_data[0] / ADC_LED_RANGE));
		GPIO_array_write_value(&y_bargraph, 1 << (adc_data[1] / ADC_LED_RANGE));
		// wait for release
		if ((++counter) >= 60000)
		{
			counter = 0;
			//snprintf(buffer, sizeof(buffer), "%d %d\r\n", adc_data[0], adc_data[1]);
			USART_data_write_dma(USART_NO_1, "aaa", 3);
			WAIT(USART_get_flag(USART_NO_1, USART_STAT_FLAG_TX_DONE) == 0)
		}

	}
}
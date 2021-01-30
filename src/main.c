#include "ADC.h"
#include "DMA.h"
#include "GPIO.h"

#define SPACE_LENGTH (720000)
#define DOT_LENGTH	 SPACE_LENGTH
#define DASH_LENGTH	 (3 * DOT_LENGTH)

#define LED_ON	(true)
#define LED_OFF (false)

void delay(uint32_t length)
{
	for (uint32_t i = 0; i < length; i++)
	{
		asm("nop");
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
	uint16_t		 temp = 0, pins[] = { 8 };
	GPIO_PIN_ARRAY_t led	= { 0 };
	GPIO_PIN_ARRAY_t button = { 0 };
	GPIO_array_init(&button, GPIO_PORT_B, 0, 0, GPIO_MODE_INPUT, GPIO_CONFIG_INPUT_ANALOG);
	GPIO_array_init(&led, GPIO_PORT_A, 0, 6, GPIO_MODE_OUTPUT, GPIO_CONFIG_OUTPUT_OPEN_DRAIN);
	ADC_init(pins, 1, &temp);

	// Clear display
	GPIO_array_write_value(&led, 0x7F);
	while (1)
	{
		// each button press increase the index one time only
		ADC_start(ADC_mode_single, 1);
		delay(1000);
		DMA_stop_channel(DMA_CH1_ADC1);
		temp /= 256;
		GPIO_array_write_value(&led, LED_7SEG_VALUES[temp]);
		// wait for release
	}
}
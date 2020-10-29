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
*/
const uint16_t LED_7SEG_VALUES[] = { 0x800, 0xf20, 0x480, 0x600, 0x320, 0x240, 0x40, 0xf00, 0x0, 0x200 };

int main()
{

	uint8_t			 i		= 0;
	GPIO_PIN_ARRAY_t led	= { 0 };
	GPIO_PIN_ARRAY_t button = { 0 };
	GPIO_array_init(&button, GPIO_PORT_B, 9, 9, GPIO_MODE_INPUT, GPIO_CONFIG_INPUT_PULL_UP);
	GPIO_array_init(&led, GPIO_PORT_A, 5, 11, GPIO_MODE_OUTPUT, GPIO_CONFIG_OUTPUT_OPEN_DRAIN);
	// Clear display
	GPIO_array_write_value(&led, 0xFE0);
	while (1)
	{
		// each button press increase the index one time only
		while (GPIO_array_read_all(&button) != 0)
		{
			delay(1000);
		}
		GPIO_array_write_value(&led, LED_7SEG_VALUES[i++]);
		if (i >= 10)
		{
			i = 0;
		}
		// wait for release
		while (GPIO_array_read_all(&button) == 0)
		{
			delay(1000);
		}
	}
}
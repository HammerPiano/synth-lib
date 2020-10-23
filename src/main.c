#include "GPIO.h"

int main()
{
	bool			 state = false;
	GPIO_PIN_ARRAY_t led   = { 0 };
	GPIO_array_init(&led, GPIO_PORT_C, 13, 13, GPIO_MODE_OUTPUT, GPIO_CONFIG_OUTPUT_PUSH_PULL);
	while (1)
	{
		GPIO_array_write_all(&led, state);
		for (uint32_t i = 0; i < 500000; i++)
		{
			asm("nop");
		}
		state = !state;
	}
}
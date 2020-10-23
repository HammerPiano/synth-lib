#include "GPIO.h"

#define SPACE_LENGTH (720000)
#define DOT_LENGTH	 SPACE_LENGTH
#define DASH_LENGTH	 (3 * DOT_LENGTH)

void delay(uint32_t length)
{
	for (uint32_t i = 0; i < length; i++)
	{
		asm("nop");
	}
}

void letter_space(pGPIO_PIN_ARRAY_t led)
{
	GPIO_array_write_all(led, true);
	delay(DASH_LENGTH);
}

void space(pGPIO_PIN_ARRAY_t led)
{
	GPIO_array_write_all(led, true);
	delay(SPACE_LENGTH);
}

void dot(pGPIO_PIN_ARRAY_t led)
{
	GPIO_array_write_all(led, false);
	delay(DOT_LENGTH);
	space(led);
}

void dash(pGPIO_PIN_ARRAY_t led)
{
	GPIO_array_write_all(led, false);
	delay(DASH_LENGTH);
	space(led);
}

int main()
{
	GPIO_PIN_ARRAY_t led = { 0 };
	GPIO_array_init(&led, GPIO_PORT_C, 13, 13, GPIO_MODE_OUTPUT, GPIO_CONFIG_OUTPUT_PUSH_PULL);
	while (1)
	{
		dot(&led);
		dash(&led);
		letter_space(&led);
		dot(&led);
		dash(&led);
		dot(&led);
		letter_space(&led);
		dot(&led);
		dash(&led);
		letter_space(&led);
		dash(&led);
		dash(&led);
		delay(10 * DASH_LENGTH);
	}
}
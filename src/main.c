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

void letter_space(pGPIO_PIN_ARRAY_t led)
{
	GPIO_array_write_all(led, LED_OFF);
	delay(DASH_LENGTH);
}

void space(pGPIO_PIN_ARRAY_t led)
{
	GPIO_array_write_all(led, LED_OFF);
	delay(SPACE_LENGTH);
}

void dot(pGPIO_PIN_ARRAY_t led)
{
	GPIO_array_write_all(led, LED_ON);
	delay(DOT_LENGTH);
	space(led);
}

void dash(pGPIO_PIN_ARRAY_t led)
{
	GPIO_array_write_all(led, LED_ON);
	delay(DASH_LENGTH);
	space(led);
}

int main()
{
	GPIO_PIN_ARRAY_t led	= { 0 };
	GPIO_PIN_ARRAY_t button = { 0 };
	GPIO_array_init(&led, GPIO_PORT_B, 9, 9, GPIO_MODE_OUTPUT, GPIO_CONFIG_OUTPUT_PUSH_PULL);
	GPIO_array_init(&button, GPIO_PORT_A, 4, 4, GPIO_MODE_INPUT, GPIO_CONFIG_INPUT_PULL_UP);
	while (1)
	{
		while (GPIO_array_read_all(&button) != 0)
		{
			asm("nop");
			delay(1000);
		}
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
	}
}
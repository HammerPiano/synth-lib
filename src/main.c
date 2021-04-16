#include "ADC.h"
#include "DMA.h"
#include "GPIO.h"

#define SPACE_LENGTH (720000)
#define DOT_LENGTH	 SPACE_LENGTH
#define DASH_LENGTH	 (3 * DOT_LENGTH)

#define LED_ON	(true)
#define LED_OFF (false)

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
	const uint8_t	 adc_pins = {8,9};// pins B0, B1 are inputs 8,9 of the ADC1
	uint16_t 		 adc_data[2] = {0}; // X and Y values of potentiometer
	GPIO_PIN_ARRAY_t x_bargraph	= { 0 };
	GPIO_PIN_ARRAY_t y_bargraph	= { 0 };
	GPIO_PIN_ARRAY_t err_led  = { 0 };
	GPIO_array_init(&x_bargraph, GPIO_PORT_A, 0, 6, GPIO_MODE_OUTPUT, GPIO_CONFIG_OUTPUT_PUSH_PULL);

	GPIO_array_init(&err_led, GPIO_PORT_C, 13, 13, GPIO_MODE_OUTPUT, GPIO_CONFIG_OUTPUT_OPEN_DRAIN);
	// ADC_init(adc_pins, sizeof(adc_pins) / sizeof(adc_pins[0]), adc_data);
	if (GPIO_array_init(&y_bargraph, GPIO_PORT_B, 5, 11, GPIO_MODE_OUTPUT, GPIO_CONFIG_OUTPUT_PUSH_PULL) == GPIO_NO_ERR)
	{
		GPIO_array_write_value(&err_led, 1);
	}
	else
	{
		GPIO_array_write_value(&err_led, 1);
	}

	GPIO_array_write_all(&y_bargraph, 1);
	GPIO_array_write_all(&x_bargraph, 1);
	delay(100);
	GPIO_array_write_all(&y_bargraph, 0);
	GPIO_array_write_all(&x_bargraph, 0);
	while (1)
	{
		for (size_t i = 0; i < 7; i++)
		{
			GPIO_array_write_value(&x_bargraph, 1 << i);
			delay(10);
		}
		GPIO_array_write_all(&x_bargraph, 0);
		for (size_t i = 0; i < 7; i++)
		{
			GPIO_array_write_value(&y_bargraph, 1 << i);
			delay(10);
		}
		GPIO_array_write_all(&y_bargraph, 0);

		/*
		// each button press increase the index one time only
		ADC_start(ADC_mode_single, 1);
		while(ADC_get_flag(ADC_end_of_conversion) == 0)
		{
			asm ("nop");
		}
		DMA_stop_channel(DMA_CH1_ADC1);
		temp /= 256;
		GPIO_array_write_value(&x_bargraph, LED_7SEG_VALUES[temp]);
		// wait for release
		*/
	}
}
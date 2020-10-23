#include "GPIO.h"

struct _GPIO_PIN_ARRAY
{
	uint32_t start_pin : 4;
	uint32_t end_pin : 4;
	uint32_t mode : 4;
	uint32_t config : 4;
	uint32_t port : 4;
	uint32_t num_of_pins : 4;
};

typedef struct
{
	__IO uint32_t CRL;
	__IO uint32_t CRH;
	__IO uint32_t IDR;
	__IO uint32_t ODR;
	__IO uint32_t BSRR;
	__IO uint32_t BRR;
	__IO uint32_t LCKR;
} GPIO_TypeDef;

#define GPIOA_BASE (APB2PERIPH_BASE + 0x00000800U)
#define GPIOB_BASE (APB2PERIPH_BASE + 0x00000C00U)
#define GPIOC_BASE (APB2PERIPH_BASE + 0x00001000U)

#define GPIOA ((GPIO_TypeDef *)GPIOA_BASE)
#define GPIOB ((GPIO_TypeDef *)GPIOB_BASE)
#define GPIOC ((GPIO_TypeDef *)GPIOC_BASE)

#define GPIO_MIN_CR	 (0)
#define GPIO_MAX_CR	 (7)
#define GPIO_MIN_CRL (0)
#define GPIO_MAX_CRL (7)
#define GPIO_MIN_CRH (8)
#define GPIO_MAX_CRH (15)

/*
 ? static functions
*/

/**
 * @brief This function will return the port structure for the port
 *
 * @param portport num
 * @return GPIO_TypeDef* port struct
 */
static GPIO_TypeDef * _get_port(GPIO_PORT_t port)
{
	if (port == GPIO_PORT_A)
	{
		return GPIOA;
	}
	else if (port == GPIO_PORT_B)
	{
		return GPIOB;
	}
	else if (port == GPIO_PORT_C)
	{
		return GPIOC;
	}
	return NULL;
}

uint16_t static calculate_pin_mask(uint8_t start_pin, uint8_t end_pin)
{
	/*
	we want to generate a value, with pin_count 1's
	to do it, do pow(2, pin_count) and subtract 1
	*/
	uint8_t pin_count = end_pin - start_pin;
	return ((1 << pin_count) - 1) << start_pin;
}

/**
 * @brief This function configs the C register from start_pin to end_pin in the gpio cr (config register)
 *
 * @param gpio_cr the register to config
 * @param start_pin start pin
 * @param end_pin end pin
 * @param mode gpio mode
 * @param config gpio config
 * @return true config successfull
 * @return false config failed
 *
 * @remarks the function handles configurations of pins 8-15, not beyond that
 */
bool static config_cr_register(periph_ptr_t gpio_cr, uint8_t start_pin, uint8_t end_pin, GPIO_MODE_t mode, GPIO_CONFIG_t config)
{
	uint16_t pin_init_value = mode | ((config & 2) << 2), pin_array_init_value = 0, pin_mask = 0;

	if (gpio_cr == NULL)
	{
		return false;
	}
	if (start_pin > GPIO_MAX_CRH || end_pin > GPIO_MAX_CRH)
	{
		return false;
	}
	// calculate the pins to range from 0-7, in both CRL and CRH
	start_pin &= GPIO_MAX_CR;
	end_pin &= GPIO_MAX_CR;
	for (int i = start_pin; i <= end_pin; i++)
	{
		pin_array_init_value |= pin_init_value << i;
	}
	pin_mask = calculate_pin_mask(start_pin, end_pin);
	// set relevant bits to their values
	*gpio_cr &= ~pin_mask;
	*gpio_cr |= pin_array_init_value;
	return true;
}

/**
 * @brief This function will activate the pull up/down for the input
 *
 * @param pin_array pin array object
 * @param pull_up true to pull up, false other wise
 * @return true config successfull
 * @return false config failed
 */
bool static activate_input_pull(pGPIO_PIN_ARRAY_t pin_array, bool pull_up)
{
	GPIO_TypeDef * port_struct = NULL;
	uint16_t	   pin_mask	   = 0;
	if (pin_array == NULL)
	{
		return false;
	}
	port_struct = _get_port(pin_array->port);
	if (port_struct == NULL)
	{
		return false;
	}
	pin_mask = calculate_pin_mask(pin_array->start_pin, pin_array->end_pin);
	// to activate PULL UP write 1 for each input pin in ODR, to activate PULL DOWN write 0
	if (pull_up)
	{
		port_struct->ODR |= pin_mask;
	}
	else
	{
		port_struct->ODR &= ~pin_mask;
	}
}

bool static config_pins(const pGPIO_PIN_ARRAY_t pin_array)
{
	GPIO_TypeDef * port_struct = _get_port(pin_array->port);
	// each CRx is responsible for different pins in the port, CRL for first 8 pins, CRH for the others
	uint8_t crl_max = pin_array->end_pin > GPIO_MAX_CRL ? GPIO_MAX_CRL : pin_array->end_pin;
	uint8_t crh_min = pin_array->start_pin < GPIO_MIN_CRH ? GPIO_MIN_CRH : pin_array->start_pin;

	if (port_struct == NULL)
	{
		return NULL;
	}

	if (pin_array->start_pin <= GPIO_MAX_CRL)
	{
		config_cr_register(&(port_struct->CRL), pin_array->start_pin, crl_max, pin_array->mode, pin_array->config);
	}

	if (pin_array->end_pin > GPIO_MIN_CRL)
	{
		config_cr_register(&(port_struct->CRH), crh_min, crl_max, pin_array->mode, pin_array->config);
	}
}

bool GPIO_array_init(pGPIO_PIN_ARRAY_t pin_array, GPIO_PORT_t port, uint8_t start_pin, uint8_t end_pin, GPIO_MODE_t mode, GPIO_CONFIG_t config)
{
	// Validate parameters
	if (pin_array == NULL)
	{
		return false;
	}
	if (start_pin > GPIO_MAX_PIN || end_pin > GPIO_MAX_PIN)
	{
		return false;
	}
	// init the struct
	pin_array->start_pin   = start_pin;
	pin_array->end_pin	   = end_pin;
	pin_array->mode		   = mode;
	pin_array->config	   = config;
	pin_array->port		   = port;
	pin_array->num_of_pins = end_pin - start_pin;

	config_pins(pin_array);

	if (mode == GPIO_MODE_INPUT && (config == GPIO_CONFIG_INPUT_PULL_DOWN || config == GPIO_CONFIG_INPUT_PULL_UP))
	{
		activate_input_pull(pin_array, config == GPIO_CONFIG_INPUT_PULL_UP);
	}
}

void GPIO_array_write_pins(const pGPIO_PIN_ARRAY_t pin_array, uint16_t pin_mask, bool state)
{
}

void GPIO_array_write(const pGPIO_PIN_ARRAY_t pin_array, uint16_t value)
{
}

void GPIO_array_write_all(const pGPIO_PIN_ARRAY_t pin_array, bool state)
{
}
#ifndef __GPIO_H__
#define __GPIO_H__

#include "common.h"

/* pin masks for any number of adjacent GPIO pins */
#define GPIO_PIN_MASK_1  (0x0001)
#define GPIO_PIN_MASK_2  (0x0003)
#define GPIO_PIN_MASK_3  (0x0007)
#define GPIO_PIN_MASK_4  (0x000f)
#define GPIO_PIN_MASK_5  (0x001f)
#define GPIO_PIN_MASK_6  (0x003f)
#define GPIO_PIN_MASK_7  (0x007f)
#define GPIO_PIN_MASK_8  (0x00ff)
#define GPIO_PIN_MASK_9  (0x01ff)
#define GPIO_PIN_MASK_10 (0x03ff)
#define GPIO_PIN_MASK_11 (0x07ff)
#define GPIO_PIN_MASK_12 (0x0fff)
#define GPIO_PIN_MASK_13 (0x1fff)
#define GPIO_PIN_MASK_14 (0x3fff)
#define GPIO_PIN_MASK_15 (0x7fff)
#define GPIO_PIN_MASK_16 (0xffff)

/* range - including */
#define GPIO_MAX_PIN (15)
#define GPIO_MIN_PIN (0)

typedef enum
{
	GPIO_PORT_A,
	GPIO_PORT_B,
	GPIO_PORT_C,
	GPIO_PORT_COUNT
} GPIO_PORT_t;

typedef enum
{
	GPIO_MODE_INPUT,
	GPIO_MODE_OUTPUT,
	GPIO_MODE_OUTPUT_2MHz,
	GPIO_MODE_OUTPUT_50Mhz
} GPIO_MODE_t;

typedef enum
{
	GPIO_CONFIG_INPUT_ANALOG,
	GPIO_CONFIG_INPUT_FLOATING,
	/* pull up/down uses the same value in configuration register */
	GPIO_CONFIG_INPUT_PULL_DOWN,
	GPIO_CONFIG_INPUT_PULL_UP = GPIO_CONFIG_INPUT_PULL_DOWN,
	GPIO_CONFIG_INPUT_RESERVED,
	GPIO_CONFIG_OUTPUT_PUSH_PULL,
	GPIO_CONFIG_OUTPUT_OPEN_DRAIN,
	/* Alt as alternate function */
	GPIO_CONFIG_OUTPUT_PUSH_PULL_ALT,
	GPIO_CONFIG_OUTPUT_OPEN_DRAIN_ALT,
} GPIO_CONFIG_t;

typedef enum
{
	GPIO_NO_ERR,
	GPIO_NULL,
	GPIO_PINS_RESERVED,
	GPIO_INVALID_PIN,
	GPIO_INVALID_PORT

} GPIO_ERR_t;

typedef struct _GPIO_PIN_ARRAY
{
	uint32_t start_pin : 4;
	uint32_t end_pin : 4;
	uint32_t mode : 4;
	uint32_t config : 4;
	uint32_t port : 4;
	uint32_t num_of_pins : 4;
} GPIO_PIN_ARRAY_t, *pGPIO_PIN_ARRAY_t;

/**
 * @brief This function inits a range of gpio pins in the give port
 * 
 * @param pin_array pin_array object
 * @param port which port to select
 * @param start_pin first pin in group (included)
 * @param end_pin last pin in group (included)
 * @param mode input/output
 * @param config specific configuration
 * @return GPIO_ERR_t errors if any
 */
GPIO_ERR_t GPIO_array_init(pGPIO_PIN_ARRAY_t pin_array, GPIO_PORT_t port, uint8_t start_pin, uint8_t end_pin, GPIO_MODE_t mode, GPIO_CONFIG_t config);

/*
 ? Output functions
*/

/**
 * @brief This function is the same as the function GPIO_array_write_pins, just setting for all of the pins
 * 
 * @param pin_array pin_array object
 * @param state on/off for all pins
 * 
 * @remark this function uses the BSRR/BRR (atomic write)
 */
void GPIO_array_write_all(const GPIO_PIN_ARRAY_t * pin_array, bool state);

/**
 * @brief This function will set the given pins to the state
 * 
 * @param pin_array pin_array object
 * @param pin_mask which pins to change
 * @param state on/off for the pins
 * 
 * @remarks the function ignore any pins outside the bounds of the array
 * @remark this function uses the BSRR/BRR (atomic write)
 */
void GPIO_array_write_pins(const GPIO_PIN_ARRAY_t * pin_array, uint16_t pin_mask, bool state);

/**
 * @brief This function will write the given value and override all pins
 * 
 * @param pin_array pin_array object
 * @param value value to write

 */
void GPIO_array_write_value(const GPIO_PIN_ARRAY_t * pin_array, uint16_t value);

/*
 ? Input functions
*/

/**
 * @brief This function will read all pins of the given array
 * 
 * @param pin_array pin_array object
 * @return uint16_t read value
 */
uint16_t GPIO_array_read_all(const GPIO_PIN_ARRAY_t * pin_array);

/**
 * @brief This function will read some of the pins, given by the pin mask
 * 
 * @param pin_array pin_array object
 * @param pin_mask which pins to read
 * @return uint16_t read value
 * 
 * @remarks The value will be right shifted, so that the first pin (given in initializaion) will be bit 0
 */
uint16_t GPIO_array_read_pins(const GPIO_PIN_ARRAY_t * pin_array, uint16_t pin_mask);

/**
 * @brief This function is called on the startup of the chip
 * 
 */
void GPIO_startup();

#endif /* __GPIO_H__ */
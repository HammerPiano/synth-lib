#ifndef __RCC_H__
#define __RCC_H__

#include "common.h"
#include <stdint.h>
#include <stddef.h>
typedef enum
{
	/* AHB peripherals */
	RCC_DMA1,
	RCC_DMA2,


	/* APB1 peripherals */
	RCC_TIM2 = 32,
	RCC_TIM3,
	RCC_TIM4,
	RCC_WWDG = RCC_TIM2 + 11,
	RCC_SPI2 = RCC_TIM2 + 14,
	RCC_UART2= RCC_TIM2 + 17,
	RCC_UART3,
	RCC_I2C1 = RCC_TIM2 + 21,
	RCC_I2C2,
	RCC_USB,
	RCC_BKP =  RCC_TIM2 + 27,
	RCC_PWR,

	/* APB2 peripherals */
	RCC_AFIO = 64,
	RCC_GPIOA = RCC_AFIO + 2,
	RCC_GPIOB,
	RCC_GPIOC,
	RCC_ADC1 = RCC_AFIO + 9,
	RCC_ADC2,
	RCC_TIM1,
	RCC_SPI1,
	RCC_UART1 = RCC_AFIO + 14

} RCC_Peripherals_t;

/**
 * @brief This function will enable or disable the clock for the given peripheral
 * 
 * @param periph - peripheral ID
 * @param enable - set true to enable the clock, false to disable
 */
void RCC_peripheral_set_clock(RCC_Peripherals_t periph, bool enable);

/**
 * @brief This function will resetart the given peripheral
 * 
 * @param periph - peripheral ID
 */
void RCC_peripheral_reset(RCC_Peripherals_t periph);

/**
 * @brief This function will reset the clock configuration registers to thier default value
 * 
 */
void RCC_reset_clock();

/**
 * @brief This function will init the clock to run @ 72MHz
 * 
 */
void RCC_init_clock();

/**
 * @brief This function will send a software restart signal to the system
 * 
 */
void RCC_reset_system();

#endif /* __RCC_H__ */
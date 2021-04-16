#ifndef __ADC_H__
#define __ADC_H__

#include "common.h"

#define MAX_CHANNEL	 (16)
#define MAX_SEQUENCE (16)

/*
how many clock cycles to sample a channel, in adc clock cycles
! the converstion takes 12.5 adc clock cycles
*/
typedef enum
{
	ADC_SAMPLING_1_5,
	ADC_SAMPLING_7_5,
	ADC_SAMPLING_13_5,
	ADC_SAMPLING_28_5,
	ADC_SAMPLING_41_5,
	ADC_SAMPLING_55_5,
	ADC_SAMPLING_71_5,
	ADC_SAMPLING_239_5
} ADC_SAMPLING_TIME_t;

/*
ADC flags we want to checkout, might add some DMA flags as well
From the reference manual page 237
*/
typedef enum
{
	ADC_end_of_conversion = 0x02,
	ADC_regular_start	  = 0x10,
} ADC_flag_t;

typedef enum
{
	ADC_mode_single, // ? This mode means just sampling all channels once, and waiting for this function be called again
	ADC_mode_loop	 // ? This mode means the ADC will loop on all channels untill stopped
} ADC_mode_t;

/* it is possible to provide ADC_DEFAULT_SAMPLING_TIME which is one of the sampling values */
#ifndef ADC_DEFAULT_SAMPLING_TIME
#define ADC_DEFAULT_SAMPLING_TIME (ADC_SAMPLING_13_5)
#endif /*ADC_DEFAULT_SAMPLING_TIME*/

/**
 * @brief This function will initialize the ADC
 *
 * @param channels which channels to sample, array of upto 16 channels, channels can be repeated
 * @param count_channels number of channels to sample
 * @param output where to store the incoming ADC data
 * @return true init successfull
 * @return false init not successfull
 *
 * @remark The extented version of this function includes an array for individually setting the sampling time, default is in ADC_DEFAULT_SAMPLING_TIME
 */
bool ADC_init(uint8_t * channels, uint8_t count_channels, uint16_t * output);

/**
 * @brief This function inits the adc
 *
 * @param channels which channels to sample, array of upto 16 channels, channels can be repeated
 * @param channels_sampling_time array of values to select the sampling time needed for the ADC
 * @param count_channels number of channels to sample
 * @param output where to store the incoming ADC data
 * @return true init successfull
 * @return false init not successfull
 */
bool ADC_init_ex(uint8_t * channels, ADC_SAMPLING_TIME_t * channels_sampling_time, uint8_t count_channels, uint16_t * output);

/**
 * @brief This function returns the flag value for the selected flag
 *
 * @param flag which flag to check
 * @return bool flag state
 * 
 * @remark This function is to only retrieve the flags related to the ADC,
 * 		   to get the flags of the DMA of the ADC, use the appropiate DMA function
 */
bool ADC_get_flag(ADC_flag_t flag);

/**
 * @brief This function will return the address of the data regsiter of the ADC
 * @details This function is needed for using the DMA to read data from the ADC, if we want to call it independently
 *
 * @return periph_ptr_t the address of the register
 */
periph_ptr_t ADC_get_data_register();

/**
 * @brief This function will start the ADC
 *
 * @param mode which mode to use
 */
void ADC_start(ADC_mode_t mode, uint8_t count_channels);

/**
 * @brief This function will stop the ADC
 *
 */
void ADC_stop();

/**
 * @brief This function inits global variables in the ADC module
 *
 */
void ADC_startup();
#endif /*__ADC_H__*/
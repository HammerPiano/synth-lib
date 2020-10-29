#include "utils.h"

uint32_t utils_generate_mask(uint8_t start_bit, uint8_t end_bit)
{
	uint8_t	 bit_count = end_bit - start_bit + 1;
	uint32_t bitmask   = 0;
	/* if the mask is for the whole word range, the CPU is limited for 32bit long words */
	if (bit_count == 32)
	{
		bitmask = 0xFFFFFFFF;
	}
	else
	{
		bitmask = (1 << bit_count) << start_bit;
	}
	return bitmask;
}
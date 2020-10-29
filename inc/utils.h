#ifndef __UTILS_H__
#define __UTILS_H__

#include <stdint.h>

/**
 * @brief This function will generate a bit mask, covering all bits from start_bit to end_bit
 * 
 * @param start_bit bit to start from
 * @param end_bit bit to end at
 * @return uint32_t resulting mask
 * 
 * @remarks The start and and are included in the range
 */
uint32_t utils_generate_mask(uint8_t start_bit, uint8_t end_bit);

#endif /* __UTILS_H__ */
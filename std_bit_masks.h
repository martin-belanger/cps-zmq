/*
 * Copyright (c) 2016 Dell Inc.
 *
 * Licensed under the Apache License, Version 2.0 (the "License"); you may
 * not use this file except in compliance with the License. You may obtain
 * a copy of the License at http://www.apache.org/licenses/LICENSE-2.0
 *
 * THIS CODE IS PROVIDED ON AN *AS IS* BASIS, WITHOUT WARRANTIES OR
 * CONDITIONS OF ANY KIND, EITHER EXPRESS OR IMPLIED, INCLUDING WITHOUT
 * LIMITATION ANY IMPLIED WARRANTIES OR CONDITIONS OF TITLE, FITNESS
 * FOR A PARTICULAR PURPOSE, MERCHANTABLITY OR NON-INFRINGEMENT.
 *
 * See the Apache Version 2.0 License for specific language governing
 * permissions and limitations under the License.
 */

/*
 * filename: std_bit_masks.h
 */

/**
 *       @file  std_bit_masks.h
 *      @brief  bit manipulation tools
 *
 *
 *   @internal
 *     Created  05/11/2014
 *     Company  DELL
 *   Copyright  Copyright (c) 2014,
 *
 * =====================================================================================
 */

#ifndef __STD_BIT_MASKS_H
#define __STD_BIT_MASKS_H

#include "std_type_defs.h"
#include <stddef.h>

#ifdef __cplusplus
extern "C" {
#endif

#define BITS_PER_BYTE  8
#define BITS_PER_INT (BITS_PER_BYTE*sizeof(uint_t))

/**
 * Determine the offset in bytes of a bit
 */
#define STD_BIT_ARRAY_BYTE_OFFSET(numbits) \
    (((numbits)/BITS_PER_BYTE))

/**
 * Determine the offset in bits of a bit within a byte
 */
#define STD_BIT_ARRAY_BIT_OFFSET(numbits) \
    (((numbits)%BITS_PER_BYTE))

/**
 * Determine the number of bytes needed to support a bit array
 */
#define STD_BYTES_FOR_BITS(numbits) \
    ((STD_BIT_ARRAY_BIT_OFFSET(numbits)==0 ? 0 : 1) + STD_BIT_ARRAY_BYTE_OFFSET(numbits))

/**
 * @brief  create a bit mask for a specific type using the number of bits
 *
 * @param type the type of the mask (int, unsigned int, etc..)
 * @param bits the numnber of bits to mask
 *
 * example... STD_BIT_MASK_MAKE(int,8) = 0xff
 */
#define STD_BIT_MASK_MAKE(type, bits) \
    (~((((type)~0) << ((bits)) & ((type)~0))))

/**
 * @brief  create a bit mask for a specific type using the number of bits
 *          and move it to a specific location
 *
 * @param type the type of the mask (int, unsigned int, etc..)
 * @param bits the numnber of bits to mask
 * @param bit_offset how many bits to shift
 *
 * example... STD_BIT_MASK(int,8,8) = 0xff00
 */
#define STD_BIT_MASK(type, bits, bit_offset) \
    (STD_BIT_MASK_MAKE(type,bits) << (bit_offset))

/**
 * A macro to create a bit array (numbits is the number of bits needed in the array at minimum)
 */
#define STD_BIT_ARRAY_CREATE(name, numbits) \
    uint8_t name[STD_BYTES_FOR_BITS(numbits)]

/**
 * A macro to set a bit in a bit array.  Bitoffset is a bit offset (ie 301)
 * in the range 0 .. (numbits-1) inclusive.
 * Eg: For bit array created with numbbits as 8, valid bitoffsets are from
 * 0 to 7 inclusive
 */
#define STD_BIT_ARRAY_SET(name,bitoffset) \
    (((uint8_t*)(name))[STD_BIT_ARRAY_BYTE_OFFSET(bitoffset)] |= 1<<STD_BIT_ARRAY_BIT_OFFSET(bitoffset))

/**
 * A macro to clear a bit in a bit array.  Bitoffset is a bit offset (ie 301)
 * in the range 0 .. (numbits-1) inclusive.
 * Eg: For bit array created with numbbits as 8, valid bitoffsets are from
 * 0 to 7 inclusive
 */
#define STD_BIT_ARRAY_CLR(name,bitoffset) \
    (((uint8_t*)(name))[STD_BIT_ARRAY_BYTE_OFFSET(bitoffset)] &= ~((uint8_t)(1<<STD_BIT_ARRAY_BIT_OFFSET(bitoffset))))

/**
 * A macro to check a bit in a bit array
 */
#define STD_BIT_ARRAY_TEST(name,bitoffset) \
    ((((uint8_t*)(name))[STD_BIT_ARRAY_BYTE_OFFSET(bitoffset)] & (1<<STD_BIT_ARRAY_BIT_OFFSET(bitoffset)))!=0)

/**
 * Free the a created bitmap - only bitmaps created with std_bitmap_create_array
 * are supported
 * @param bitmap is the bitmap to free
 */
void std_bitmaparray_free_data(void *bitmap);

/**
 * Create an array where all bits are set to 1
 * @param the length in bits (eg... 32 for an uint32_t sized array)
 */
void *std_bitmap_create_array(unsigned long len);

/**
 * This is the method to create an array that is all set to 0 by default
 * @param len this is the length in bits
 */
void *std_bitmap_create_array_clear_bits(unsigned long len);


/**
 * Find the first bit set to 1 in the varray of bits.
 * @param array the array of bits
 * @param len the length of array in bits
 * @param from starting from a specific bit position in the range 0 to len-1
 * @return -1 for no found bits otherwise the bit position starting at the begin
 */
int std_find_first_bit(void *array, size_t len, size_t from) ;
/**
 * This API finds the last bit set in the varray of bits
 * @param varray the bit array
 * @param len the length of the bit array (in bits..)
 * @param from starting at this bit position and working back - range 0 to len-1
 * @return -1 for not found, or otherwise the bit position starting at the end
 */
int std_find_last_bit(void *varray, size_t len, size_t from);


#ifdef __cplusplus
}
#endif

#endif

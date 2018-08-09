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


#ifndef __STD_TLV_H
#define __STD_TLV_H

#include <stdint.h>
#include <stdbool.h>
#include <string.h>

#include "endian.h"

/** @defgroup STDTLV "Standard TLV API"
 * This is a standard TLV API that will encode both the tag, and value in
 * an endian neutral format.
 * The general APIs from this library are:
 *   std_tlv_h_tag - get the tag from a void pointer
 *   std_tlv_h_len - get the length from a void pointer
 *   std_tlv_add - append a TLV to the current tlv
 *   std_tlv_find - find the next instance of a tag by walking TLVs
 *   std_tlv_next - go to the next tlv
 @{
*/

/**
 * The type of the TLV tag
 */
typedef uint64_t std_tlv_tag_t;

/**
 * The type of the TLV length
 */
typedef uint64_t std_tlv_len_t;

#define STD_TLV_HDR_LEN (sizeof(std_tlv_tag_t)+ sizeof(std_tlv_len_t))

#include "std_tlv_internal.h"

/**
 * Set the tag into the current TLV in a endian neutral format
 * @param data the pointer to the TLV
 * @param tag the tag to set
 */
static inline void std_tlv_set_tag(register void *data, std_tlv_tag_t tag) {
    *(std_tlv_tag_t*)std_tlv_offset(data, STD_TLV_TAG_POS) = std_tlv_h_to_tag(tag);
}

/**
 * Set the length into the TLV in an endian neutral format
 * @param data the TLV to update
 * @param len the new length of the TLV
 */
static inline void std_tlv_set_len(register void *data, std_tlv_len_t len) {
    *(std_tlv_len_t*)std_tlv_offset(data, STD_TLV_LEN_POS) = std_tlv_h_to_len(len);
}

/**
 * Get the length field from the TLV in an endian neutral way
 * @param data the TLV to query
 * @return the length field of the TLV
 */
static inline std_tlv_len_t std_tlv_len(register void *data) {
    return std_tlv_len_to_h(*std_tlv_ilen(data));
}

/**
 * Get the tag field of the current TLV
 * @param data is the TLV to get the field from
 * @return the tag field of the TLV
 */
static inline std_tlv_tag_t std_tlv_tag(register void *data) {
    return std_tlv_tag_to_h(*std_tlv_itag(data));
}

/**
 * Get the total length occupied from the TLV including all headers and
 * the data used by the TLV
 * @param data the TLV to get the total size on
 * @return the TLV length plus the header length
 */
static inline std_tlv_len_t std_tlv_total_len(register void *data) {
    return std_tlv_len(data) + STD_TLV_HDR_LEN;
}

/**
 * Get the TLV data pointer as a void pointer.
 * @param data the TLV who's data to get
 */
static inline void * std_tlv_data(register void *data) {
    return std_tlv_offset(data, STD_TLV_DATA_POS);
}

/**
 * Get the data of the TLV as a uint16_t in a endian neutral way (must use
 * std_tlv_add_u16)
 * @param data the TLV to query
 * @return the data as a uint16_t
 */
static inline uint16_t std_tlv_data_u16(register void *data) {
    return le16toh(*(uint16_t*)std_tlv_data(data));
}

/**
 * Get the data of the TLV as a uint32_t in a endian neutral way (must use
 * std_tlv_add_u32)
 * @param data the TLV to query
 * @return the data as a uint32_t
 */
static inline uint32_t std_tlv_data_u32(register void *data) {
    return le32toh(*(uint32_t*)std_tlv_data(data));
}

/**
 * Get the data of the TLV as a uint64_t in a endian neutral way (must use
 * std_tlv_add_u64)
 * @param data the TLV to query
 * @return the data as a uint64_t
 */
static inline uint64_t std_tlv_data_u64(register void *data) {
    return le64toh(*(uint64_t*)std_tlv_data(data));
}

/**
 * Create a TLV at the "data" location passed as long as the data_len
 * has enough space to hold the contents including header.
 * @param data the spot to place the TLV
 * @param data_len the total reserved space
 * @param tag the tag of the data
 * @param len the length of the data
 * @param content the content to copy
 * @return a pointer to the address directly after the newly created TLV or
 *    NULL if there is not enough space
 */
static inline void * std_tlv_add(void *data, size_t *data_len, std_tlv_tag_t tag, std_tlv_len_t len, const void *content) {
    if (*data_len < (len+STD_TLV_HDR_LEN)) return NULL;
    std_tlv_set_tag(data, tag);
    std_tlv_set_len(data, len);
    memcpy(((uint8_t*)data) + STD_TLV_HDR_LEN, content, (size_t)len);
    *data_len -= (size_t)(len+STD_TLV_HDR_LEN);
    return std_tlv_offset(data, (size_t)len + STD_TLV_HDR_LEN);
}

/**
 * Create a uint16_t at the current "data" pointer and return a pointer to the memory
 * directly after the newly created TLV
 * @param data the location at which to create the TLV
 * @param data_len the total amount of space for this TLV (or remaining buffer space)
 * @param tag the tag of the TLV
 * @param content the uint16_t to add in an endian neutral way
 * @return a pointer to the memory directly after the newly created TLV or NULL if
 *     not enough space
 */
static inline void * std_tlv_add_u16(void *data, size_t *data_len, std_tlv_tag_t tag, uint16_t content) {
    content = htole16(content);
    return std_tlv_add(data, data_len, tag, sizeof(content),&content);
}

/**
 * Create a uint32_t at the current "data" pointer and return a pointer to the memory
 * directly after the newly created TLV
 * @param data the location at which to create the TLV
 * @param data_len the total amount of space for this TLV (or remaining buffer space)
 * @param tag the tag of the TLV
 * @param content the uint32_t to add in an endian neutral way
 * @return a pointer to the memory directly after the newly created TLV or NULL if
 *     not enough space
 */
static inline void * std_tlv_add_u32(void *data, size_t *data_len, std_tlv_tag_t tag, uint32_t content) {
    content = htole32(content);
    return std_tlv_add(data, data_len, tag, sizeof(content), &content);
}

/**
 * Create a uint64_t at the current "data" pointer and return a pointer to the memory
 * directly after the newly created TLV
 * @param data the location at which to create the TLV
 * @param data_len the total amount of space for this TLV (or remaining buffer space)
 * @param tag the tag of the TLV
 * @param content the uint64_t to add in an endian neutral way
 * @return a pointer to the memory directly after the newly created TLV or NULL if
 *     not enough space
 */
static inline void * std_tlv_add_u64(void *data, size_t *data_len, std_tlv_tag_t tag, uint64_t content) {
    content = htole64(content);
    return std_tlv_add(data, data_len, tag, sizeof(content), &content);
}

/**
 * Check to see if the current TLV pointer is valid taking in the pointer and a size of
 * buffer that is pointed at by data
 * @param data the TLV
 * @param len the length of memory pointed to by "data"
 * @return true if the TLV is valid otherwise false
 */
static inline bool std_tlv_valid(register void *data, register  size_t len) {
    if ((data == NULL) || (len == 0)) return false;
    return (len >= (size_t)std_tlv_total_len(data));
}

/**
 * Return a pointer to the next TLV or NULL if at the end
 * @param data the pointer to the current TLV
 * @param len the length of space that "data" contains
 * @return a pointer to the next TLV or NULL if no next
 */
static inline void * std_tlv_next(register void *data, register size_t *len) {
    if (data == NULL) return NULL;
    size_t tlen = (size_t)std_tlv_total_len(data);
    if (*len < tlen) return NULL;
    *len -= tlen;
    return std_tlv_offset(data, tlen);
}

/**
 * Find the next instance of a tag given a TLV or return NULL if not found
 * @param data the TLV to search through
 * @param dlen the length of space that "data" contains (will be updated)
 * @param tag the tag to find
 * @return the pointer to the found TLV or NULL
 */
static inline void * std_tlv_find_next(register void *data, size_t *dlen, std_tlv_tag_t tag) {
    do {
        if (!std_tlv_valid(data,*dlen)) return NULL;
        if (std_tlv_tag(data) == tag) return data;
    } while ((data = std_tlv_next(data, dlen))!= NULL);
    return NULL;
}

/**
 * Find (the first match) an internal attribute with the following algorithm.
 *     while (tlvs)
 *         find(tag) and if not found return error
 *         if no more tags in list return it
 *         otherwise enter the attribute and search
 *
 * @param data the TLV starting position
 * @param dlen[out] the length of the current TLV
 * @param tags the array of attribute IDs
 * @param tlen the lenght of the tag ID array
 * @return a pointer to the TLV if found or NULL
 */
static inline void * std_tlv_efind(register void * data, size_t *dlen, std_tlv_tag_t *tags, size_t tlen) {
    size_t ix = 0;
    size_t len = *dlen;
    for ( ; ix < tlen; ++ix) {
        data = std_tlv_find_next(data,&len,tags[ix]);
        if (data==NULL) return NULL;
        if (ix+1 == tlen) break;
        len = std_tlv_len(data);
        data = std_tlv_data(data);
    }
    *dlen = len;
    return data;
}

/** @} */

#endif

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
 * std_tlv_internal.h
 */

#ifndef STD_TLV_INTERNAL_H_
#define STD_TLV_INTERNAL_H_

#define STD_TLV_DATA_POS (STD_TLV_HDR_LEN)
#define STD_TLV_LEN_POS	 (sizeof(std_tlv_tag_t))
#define STD_TLV_TAG_POS  (0)

/**
 * The following are conversion functions to and from tag to embedded format
 * The Endian neutral format of the TLV API is le
 */
#define std_tlv_tag_to_h le64toh
#define std_tlv_len_to_h le64toh

#define std_tlv_h_to_tag htole64
#define std_tlv_h_to_len htole64

/**
 * Advance the pointer to the offset specified
 * @param data the pointer used as a base
 * @param offset the offset to add to the base
 */
static void * std_tlv_offset(register void *data, register size_t offset) {
	return ((uint8_t*)data) + offset;
}

/**
 * Get the internal length pointer
 * @param data the TLV
 * @return pointer to the length field
 */
static std_tlv_len_t * std_tlv_ilen(register void *data) {
	return (std_tlv_len_t*)std_tlv_offset(data, STD_TLV_LEN_POS);
}

/**
 * Get the pointer to the internal tag field of the tlv
 * @param data the TLV which is being referenced
 * @return the pointer to the tag field
 */
static std_tlv_tag_t * std_tlv_itag(register void *data) {
	return (std_tlv_tag_t*)std_tlv_offset(data, STD_TLV_TAG_POS);
}


#endif /* STD_TLV_INTERNAL_H_ */

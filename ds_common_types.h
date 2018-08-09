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
 * filename: db_common_types.h
 */

/*
 * db_common_types.h
 */

#ifndef DS_COMMON_TYPES_H_
#define DS_COMMON_TYPES_H_

#include <stdint.h>
#include <netinet/in.h>

/**
 * The length in octets of an V6 IP address.
 */
#define HAL_INET6_LEN (16)

/**
 * Defined by the standard as 0000:0000:0000:0000:0000:0000:127.127.127.127.
 * tools.ietf.org/html/rfc4291, And this is 46 chars (including trailing null)
 * Alternate for INET6_ADDRSTRLEN (linux)
 */
#define HAL_INET6_TEXT_LEN (46)

/**
 * The length in octets of an V4 IP address.
 */
#define HAL_INET4_LEN (4)

/**
 * The IPv4 address family.
 */
#define HAL_INET4_FAMILY (AF_INET)

/**
 * The IPv6 address family.
 */
#define HAL_INET6_FAMILY (AF_INET6)

/**
 * This the size of an ethernet address.  There are OS defines for this field but to make
 * The HAL OS independant it has been redefined
 */
#define HAL_MAC_ADDR_LEN (6)

/**
 * Represents the IFNAMSIZ define from if.h in linux.  Currently kept separate to maintain a difference between
 * the OS and Dell software
 */
#define HAL_IF_NAME_SZ (16)

/** Type used to refer to a physical NPU in the lower HAL API */
typedef int32_t npu_id_t;

/** Type used to refer to a generic port NPU or not */
typedef uint32_t port_t;

/** Type used to refer to a physical NPU port in the lower HAL API */
typedef port_t npu_port_t;

/** The type of an Ethernet address */
typedef uint8_t hal_mac_addr_t[HAL_MAC_ADDR_LEN];

/**
 * Interface index
 */
typedef int hal_ifindex_t;

/**
 * The VLAN ID type used for all nas/ndi operations
 */
//Changing it to uint16_t to match with sai_vlan_id_t which is uint16_t
typedef uint16_t hal_vlan_id_t;

// Type to refer VxLAN Identifier
typedef uint32_t vxlan_id_t;
/**
 * HAL VRF ID type
 */
typedef unsigned int hal_vrf_id_t;

/**
 * The LAG ID type used for all NAS operations
 */
typedef unsigned int nas_lag_id_t;

typedef struct in_addr  dn_ipv4_addr_t;
typedef struct in6_addr dn_ipv6_addr_t;

/**
 * The IP address structure used by all the HAL components. Address family
 * identifies the v4 or v6 address types.
 */
typedef struct _hal_ip_addr_t {
    uint32_t     af_index; /* HAL_INET4_FAMILY or HAL_INET6_FAMILY */
    union {
        struct in_addr ipv4;
        struct in6_addr ipv6;
/* Following defines are for backward compatibility */
#define v4_addr ipv4.s_addr
#define v6_addr ipv6.s6_addr
    } u;
} hal_ip_addr_t;

typedef hal_ip_addr_t std_ip_addr_t;

#endif /* DB_COMMON_TYPES_H_ */

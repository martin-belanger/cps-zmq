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
 * filename: std_error_ids.h
 */


/**
 *       @file  std_error_ids.h
 *      @brief  definition of error subsystems
 *
 *   @internal
 *     Created  05/11/2014
 *     Company  DELL
 *   Copyright  Copyright (c) 2014, Cliff Wichmann
 *
 * =====================================================================================
 */

#ifndef __STD_ERR_IDS__
#define __STD_ERR_IDS__

enum e_std_error_subsystems {
    e_std_err_NULL=0,
    e_std_err_NPU=1,
    e_std_err_BOARD=2,
    e_std_err_TEST=3,
    e_std_err_INTERFACE=4,
    e_std_err_DIAG=5,
    e_std_err_COM=6,
    e_std_err_HALCOM=7,
    e_std_err_QOS=8,
    e_std_err_ROUTE=9,
    e_std_err_ACL=10,
    e_std_err_CPSNAS=11,
    e_std_err_MGMT=12,
    e_std_err_MAC=13,
    e_std_err_STG=14,
    e_std_err_MIRROR=15,
    e_std_err_SFLOW=16,
    e_std_err_CPSAPI=17,
    e_std_err_LACP=18,
    e_std_err_XSTP=19,
    e_std_err_LLDP=20,
    e_std_err_DOT1X=21,
    e_std_err_L2MAC=22,
    e_std_err_L2CMN=23,
    e_std_err_OSPFV2=24,
    e_std_err_OSPFV3=25,
    e_std_err_BGP=26,
    e_std_err_RTM=27,
    e_std_err_POLICY=28,
    e_std_err_NAS_OS=29,
    e_std_err_PAS=30,
    e_std_err_ENV_TMPCTL=31,
    e_std_err_L3SERVICES=32,
    e_std_err_IPM=33,
    e_std_err_SWUPDATE=34,
    e_std_err_DATASTORE=35,
    e_std_err_PORT_SEC=36,
    e_std_err_MGMT_INTF=37,
    e_std_err_CHM=38,
    e_std_err_ETL=39,
    e_std_err_IFM=40,
    e_std_err_ISSU=41,
    e_std_err_NDM=42,
    e_std_err_PM=43,
    e_std_err_PPM=44,
    e_std_err_SA=45,
    e_std_err_SWITCH_RES_MGR=46,
    e_std_err_L3_DEBUG=47,
    e_std_err_OPENFLOW=48,
    e_std_err_AFS_UTILS=49,
    e_std_err_VRRP=50,
    e_std_err_INFRA_COMMON=51,
    e_std_err_VLT=52,
    e_std_err_ISCSI_OPT=53,
    e_std_err_DCBX=54,
    e_std_err_UDF=55,
    e_std_err_DNV_ONBOARDING=56,
    e_std_err_DNV_NETWORK=57,
    e_std_err_APP_FC=58,
    e_std_err_MCAST=59,
    e_std_err_APP_LFD=60,
    e_std_err_APP_VN=61,
    e_std_err_BFD=62,
};

enum e_std_error_codes {
    e_std_err_code_FAIL=1,
    e_std_err_code_CFG=2,
    e_std_err_code_PARAM=3,
    e_std_err_code_TOOBIG=4,
    e_std_err_code_CLOSED=5,
    e_std_err_code_NOMEM=6,
    e_std_err_code_NEXIST=7,
    e_std_err_code_NORESOURCE=8,
    e_std_err_code_NOTSUPPORTED=9,
};

#endif

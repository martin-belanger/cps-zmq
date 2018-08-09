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
 * filename: std_error_codes.h
 */


/**
 *       @file  std_error_codes.h
 *      @brief  error code definition
 *
 *   @internal
 *     Created  05/11/2014
 *     Company  DELL
 *   Copyright  Copyright (c) 2014, Cliff Wichmann
 *
 * =====================================================================================
 */

#ifndef __STD_ERROR_CODES_H
#define __STD_ERROR_CODES_H


#include "std_bit_masks.h"
#include "std_error_ids.h"

#include <errno.h>
#include <stdbool.h>

/**************************************************************************/
/* The following lines are internal and not meant to be used by themselves*/
#define STD_MK_ERR_FLAG_POS (31)
#define STD_MK_ERR_FLAG (1 << (STD_MK_ERR_FLAG_POS))

#define STD_MK_ERR_SUB_POS  (25)
#define STD_MK_ERR_SUB_MASK (STD_BIT_MASK(unsigned int,\
                            STD_MK_ERR_FLAG_POS-STD_MK_ERR_SUB_POS,0))

#define STD_MK_ERR_ERRID_POS  (15)
#define STD_MK_ERR_ERRID_MASK (STD_BIT_MASK(unsigned int,\
                            STD_MK_ERR_SUB_POS-STD_MK_ERR_ERRID_POS,0))

#define STD_MK_ERR_PRIV_POS  (0)
#define STD_MK_ERR_PRIV_MASK (STD_BIT_MASK(unsigned int,\
                            STD_MK_ERR_ERRID_POS-STD_MK_ERR_PRIV_POS,0))

/**************************************************************************/

/*
  The type of the error code
*/
typedef int t_std_error;


#define STD_ERR_OK (0)

/**
 * @brief is the passed parameter an error or not
 *
 * @param x the value to check to see if it is an error
 */
#define STD_IS_ERR(x) ((x)!=STD_ERR_OK)

/**
 * @brief Create an error number from a 3 touples subsyste, id, private data
 *
 * @param SUB the subsystem from std_error_ids.h
 * @param ERRID the actual generic error number also from std_error_ids.h
 * @param PRIV the private error details - i.e. errno or etc..
 */
#define STD_ERR_MK(SUB, ERRID, PRIV) \
    ((1 << STD_MK_ERR_FLAG_POS ) | \
    (((SUB) & STD_MK_ERR_SUB_MASK) << STD_MK_ERR_SUB_POS )| \
    (((ERRID) & STD_MK_ERR_ERRID_MASK) << STD_MK_ERR_ERRID_POS ) | \
    (((PRIV) & STD_MK_ERR_PRIV_MASK) << STD_MK_ERR_PRIV_POS ))

/**
 * @brief A simple wrapper for std_err_mk that will default the beginning of the
 * types to simplify typing
 * @param sub subsystem from std_error_ids.h
 * @param clas generic failure type from std_error_ids.h
 * @return standard error code
 */
#define STD_ERR(SUB,ERRID,PRIV) \
    STD_ERR_MK(e_std_err_ ## SUB,e_std_err_code_ ## ERRID, PRIV)


/**
 * @brief   remove the error flag part of the error code
 * @param   x the error code to update
 * @return  error code less the flag
 */
#define STD_ERR_RM_FLAG(x) ( (x) & (~STD_MK_ERR_FLAG) )


/**
 * @brief extract the subsystem part of the error code
 * @param   x the erorr code to edit
 * @return  the subsystem part of the error code
 */
#define STD_ERR_EXT_SUB(x) \
    ((STD_ERR_RM_FLAG(x) >> STD_MK_ERR_SUB_POS) & STD_MK_ERR_SUB_MASK)

/**
 * @brief   Extract the error id part of the error code
 * @param   x the error code to edit
 * @return  just the error ide of the error code
 */
#define STD_ERR_EXT_ERRID(x) \
    ((STD_ERR_RM_FLAG(x) >> STD_MK_ERR_ERRID_POS) & STD_MK_ERR_ERRID_MASK)


/**
 * @brief   Extract the private portion of the error code
 * @param   x the error code to edit
 * @return  the private part of the error code
 */
#define STD_ERR_EXT_PRIV(x) \
    ((STD_ERR_RM_FLAG(x) >> STD_MK_ERR_PRIV_POS) & STD_MK_ERR_PRIV_MASK)


/**
* @note Example is shown in std_error_codes.h (below)
* @verbatim
 #include <unistd.h>
 #include <errno.h>

 t_std_error open_file(const char *f, int *fd) {

     *fd = open(f,O_RDONLY);
     if (*fd==-1) {
         return STD_ERR_MK(e_std_err_TEST,e_std_err_code_PARAM,errno);
     }
     return STD_ERR_OK;
 }
 @endverbatim
*/

/**
 * @brief Create an error based on subsystem, class and errno as
 *              the private data
 * @param sub the subsystem from std_error_ids.h
 * @param clas the actual generic error number also from std_error_ids.h
 */
static inline t_std_error STD_ERR_FROM_ERRNO(enum e_std_error_subsystems sub,
    enum e_std_error_codes clas) {
    return STD_ERR_MK(sub,clas,errno);
}

/**
 * Check the condition and return STD_ERR_OK if the condition is true.  Just a short
 * cut to always typing the a check on the return code
 *
 * @param test_condition a boolean statement (ie. socket!=-1)
 * @param if_condition_false a standard error code that is return if it is false
 * @return the return code that is either STD_ERR_OK if test_condition is true or
 *           if_condition_false value
 */
static inline t_std_error STD_ERR_OK_IF_TRUE(bool test_condition, t_std_error if_condition_false) {
    return (test_condition) ? STD_ERR_OK : if_condition_false;
}

#endif

/*!
 * \file abi.h
 * \date 2014/04/29 14:39
 *
 * \author yayan
 * Contact: user@company.com
 *
 * \brief 
 *
 * TODO: long description
 *
 * \note
*/

#ifndef LEMON_ABI_H
#define LEMON_ABI_H

#include <lemon/configure.h>

#define LEMON_MILLISECONDS_OF_ONE_TICK                          100

typedef struct{
    const char*                                                 startfile;
    const char**                                                loadpaths;
    const char**                                                loadcpaths;
    const char**                                                args;
}                                                               lemon_conf;

LEMON_API int start_lemon_service(const lemon_conf * conf);

#endif // LEMON_ABI_H
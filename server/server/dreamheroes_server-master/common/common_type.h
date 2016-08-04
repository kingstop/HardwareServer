#ifndef __common_type_h__
#define __common_type_h__
#include "stdint.h"
#ifndef WIN32
typedef unsigned char           u8;
typedef signed char             s8;
typedef unsigned short          u16;
typedef signed short            s16;
typedef unsigned int            u32;
typedef signed int              s32;
typedef unsigned long long      u64;
typedef signed long long        s64;
#else
typedef uint8_t                 u8;
typedef int8_t                  s8;
typedef uint16_t                u16;
typedef int16_t                 s16;
typedef uint32_t                u32;
typedef int32_t                 s32;
typedef uint64_t                u64;
typedef int64_t                 s64;
#endif // WIN32
#endif
#ifndef __common_helper_h__
#define __common_helper_h__

#include "common_type.h"

struct CheckFlag
{
    static u32  addFlag(u32& v, u32 f){ v |=  (1 << f); return v;}
    static u32  removeFlag(u32& v, u32 f){ v &= ~(1 << f) ; return v;}
    static u32  addFlags(u32&v , u32 flag){ v |= flag; return v;}
    static u32  removeFlags(u32& v, u32 flag){ v &= ~flag ; return v;}
    static bool hasFlag(const u32& v, u32 f){ return (v & (1 << f)) > 0 ;}
};

#endif
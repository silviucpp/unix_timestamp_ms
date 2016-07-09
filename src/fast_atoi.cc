//
//  fast_atoi.cpp
//
//  Created by Silviu Caragea on 7/8/16.
//  Copyright © 2016 silviu.cpp@gmail.com. All rights reserved.
//

#include "fast_atoi.h"

//we don't need the whitespace skipping for our use case.

#if defined(ATOI_SKIP_WHITESPACE)
    #define is_space(x) ((x) == ' ' || (x) == '\t' || (x) == '\n' || (x) == '\v' || (x) == '\f' || (x) == '\r')
#endif

int32 fast_atoi(const char *str)
{
    int32 res = 0;
    
#if defined(ATOI_SKIP_WHITESPACE)
    while (is_space(*str))
        ++str;
#endif

    if (*str == '-')
    {
        ++str;

        do
        {
            uint8 d = static_cast<uint8>(*str) - '0';
            if (d > 9)
                return res;

            res = res * 10 - d;
        }
        while(++str);
    }
    else if (*str == '+')
    {
        ++str;
    }
    
    do
    {
        uint8 d = static_cast<uint8>(*str) - '0';
        if (d > 9)
            return res;

        res = res * 10 + d;
    }
    while(++str);
    
    return res;
}

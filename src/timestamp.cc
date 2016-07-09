//
//  timestamp.cpp
//
//  Created by Silviu Caragea on 7/8/16.
//  Copyright © 2016 silviu.cpp@gmail.com. All rights reserved.
//

#include "timestamp.h"

#if defined(__WIN32__)
#ifndef _WINSOCKAPI_
#define _WINSOCKAPI_
#endif
#include <Windows.h>
#elif defined(__APPLE__) && defined(__MACH__)
#include <sys/time.h>
#endif

inline bool is_leap_year(uint16 y)
{
    return ((y & 3) == 0 && (y % 100 != 0 || y % 400 == 0));
}

static uint8 days_per_month(uint16 y, uint16 m)
{
    static const uint8 kDays[2][13] =
    {
        {0, 31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31},
        {0, 31, 29, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31}
    };
    
    return kDays[m == 2 && is_leap_year(y)][m];
}

static const uint16 kDayOffset[13] =
{
    0, 306, 337, 0, 31, 61, 92, 122, 153, 184, 214, 245, 275
};

int64 tm2timestamp_ms(const struct time_ms& tm)
{
    uint16 year = tm.year;
    
    if (tm.day > 28 && tm.day > days_per_month(tm.year, tm.month))
        return -1;
    
    if (tm.month < 3)
        year--;
    
    uint32 rdn = (1461 * year)/4 - year/100 + year/400 + kDayOffset[tm.month] + tm.day - 306;
    uint32 sod = tm.hour * 3600 + tm.minute * 60 + tm.second;
    
    return (((int64)rdn - 719163) * 86400 + sod)* 1000 + tm.millisecond;
}

#if defined(_WIN32)

uint64 get_time_since_epoch_ms()
{
    _FILETIME ft;
    GetSystemTimeAsFileTime(&ft);
    uint64 ns100 = (static_cast<uint64>(ft.dwHighDateTime) << 32 | static_cast<uint64>(ft.dwLowDateTime)) - 116444736000000000LL;
    return ns100 / 10000;
}

#elif defined(__APPLE__) && defined(__MACH__)

uint64 get_time_since_epoch_ms()
{
    struct timeval tv;
    gettimeofday(&tv, NULL);
    return static_cast<uint64>(tv.tv_sec)  * 1000 + static_cast<uint64>(tv.tv_usec) / 1000;
}

#else

uint64 get_time_since_epoch_ms()
{
    struct timespec ts;
    clock_gettime(CLOCK_REALTIME, &ts);
    return static_cast<uint64>(ts.tv_sec) * 1000 + static_cast<uint64>(ts.tv_nsec) / 1000000;
}

#endif

int32 gmt_offset_seconds()
{
	time_t t = time(NULL);
	struct tm lt;
	localtime_r(&t, &lt);
	return lt.tm_gmtoff;
}


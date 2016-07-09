//
//  timestamp.h
//
//  Created by Silviu Caragea on 7/8/16.
//  Copyright © 2016 silviu.cpp@gmail.com. All rights reserved.
//

#ifndef UNIX_TIMESTAMP_MS_TIMESTAMP_H_
#define UNIX_TIMESTAMP_MS_TIMESTAMP_H_

#include "basictypes.h"
#include <time.h>

struct time_ms
{
    uint16 year;
    uint8 month;
    uint8 day;
    uint8 hour;
    uint8 minute;
    uint8 second;
    uint16 millisecond;
};

int64 tm2timestamp_ms(const struct time_ms& tm);
uint64 get_time_since_epoch_ms();
int32 gmt_offset_seconds();

#endif

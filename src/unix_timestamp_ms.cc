//
//  unix_timestamp_ms.cc
//
//  Created by Silviu Caragea on 7/8/16.
//  Copyright © 2016 silviu.cpp@gmail.com. All rights reserved.
//

#include "timestamp.h"
#include "fast_atoi.h"

#include <mysql/mysql.h>
#include <string.h>

#ifdef HAVE_DLOPEN

#define MAX_BUFFER_SIZE 24
#define MAX_DATE_TIME_LENGTH 23
#define DATE_TIME_LENGTH 19

#define INVALID_YEAR(X) (X < 1)
#define INVALID_MONTH(X) (X < 1 || X > 12)
#define INVALID_DAY(X) (X < 1 || X > 31)
#define INVALID_HOUR(X) (X > 23)
#define INVALID_MINUTE(X) (X > 59)
#define INVALID_SECOND(X) (X > 59)

static const int32 kGmtOffsetMs = gmt_offset_seconds()*1000;

extern "C" {
    my_bool unix_timestamp_ms_init(UDF_INIT* initid, UDF_ARGS* args, char* message);
    void unix_timestamp_ms_deinit(UDF_INIT* initid);
    uint64 unix_timestamp_ms(UDF_INIT* initid, UDF_ARGS* args, char* is_null, char* error);
}

bool parse2d(const char* p, uint32 i, uint8 *rs)
{
    int8 d0, d1;

    if (((d0 = p[i + 0] - '0') > 9) || ((d1 = p[i + 1] - '0') > 9))
        return false;

    *rs = d0 * 10 + d1;
    return true;
}

bool parse4d(const char* p, uint32 i, uint16 *rs)
{
    char d0, d1, d2, d3;

    if (((d0 = p[i + 0] - '0') > 9) || ((d1 = p[i + 1] - '0') > 9) || ((d2 = p[i + 2] - '0') > 9) || ((d3 = p[i + 3] - '0') > 9))
        return false;

    *rs = d0 * 1000 + d1 * 100 + d2 * 10 + d3;
    return true;
}

bool parse_date_time(const char* buffer, uint64 length, struct time_ms* tm)
{
    if(length >= DATE_TIME_LENGTH)
    {
        if(buffer[4] != '-' || buffer[7]  != '-' || buffer[13] != ':' || buffer[16] != ':')
            return false;

        if(!parse4d(buffer, 0, &tm->year) || INVALID_YEAR(tm->year))
            return false;

        if(!parse2d(buffer, 5, &tm->month) || INVALID_MONTH(tm->month))
            return false;

        if(!parse2d(buffer, 8, &tm->day) || INVALID_DAY(tm->day))
            return false;

        if(!parse2d(buffer, 11, &tm->hour) || INVALID_HOUR(tm->hour))
            return false;

        if(!parse2d(buffer, 14, &tm->minute) || INVALID_MINUTE(tm->minute))
            return false;

        if(!parse2d(buffer, 17, &tm->second) || INVALID_SECOND(tm->second))
            return false;

        uint64 rest = length - DATE_TIME_LENGTH;
        tm->millisecond = (rest > 1 && buffer[DATE_TIME_LENGTH] == '.' ? fast_atoi(buffer+20) : 0);
    }
    else
    {
        bool is_time = (buffer[2] == ':');

        if(is_time && length >= 8)
        {
            if(buffer[2] != ':' || buffer[5]  != ':')
                return false;

            if(!parse2d(buffer, 0, &tm->hour) || INVALID_HOUR(tm->hour))
                return false;

            if(!parse2d(buffer, 3, &tm->minute) || INVALID_MINUTE(tm->minute))
                return false;

            if(!parse2d(buffer, 6, &tm->second) || INVALID_SECOND(tm->second))
                return false;

            uint64 rest = length - 8;
            time_t rawtime = time (0);
            struct tm* now = gmtime(&rawtime);

            tm->year = now->tm_year + 1900;
            tm->month = now->tm_mon + 1;
            tm->day = now->tm_mday;
            tm->millisecond = (rest > 1 && buffer[8] == '.' ? fast_atoi(buffer+9) : 0);
        }
        else if(length == 10)
        {
            memset(tm, 0, sizeof(struct time_ms));

            if (buffer[4] != '-' || buffer[7]  != '-')
                return false;

            if(!parse4d(buffer, 0, &tm->year) || INVALID_YEAR(tm->year))
                return false;

            if(!parse2d(buffer, 5, &tm->month) || INVALID_MONTH(tm->month))
                return false;

            if(!parse2d(buffer, 8, &tm->day) || INVALID_DAY(tm->day))
                return false;
        }
    }

    return true;
}

my_bool unix_timestamp_ms_init(UDF_INIT* initid, UDF_ARGS* args, char* message)
{
    UNUSED(initid);

    if (args->arg_count > 1)
    {
        strcpy(message,"UNIX_TIMESTAMP_MS requires no more than one argument");
        return 1;
    }

    return 0;
}

void unix_timestamp_ms_deinit(UDF_INIT* initid)
{
    UNUSED(initid);
}

uint64 unix_timestamp_ms(UDF_INIT* initid, UDF_ARGS* args, char* is_null, char* error)
{
    UNUSED(initid);
    UNUSED(is_null);

    if(args->arg_count == 0)
        return get_time_since_epoch_ms();

    if(args->args[0] == NULL)
    {
    	*is_null = 1;
    	return 0;
    }

    uint32 length = args->lengths[0];

    if(length <= MAX_DATE_TIME_LENGTH)
    {
    	char buff[MAX_BUFFER_SIZE];
		memcpy(buff, args->args[0], length);
		buff[length] = '\0';

		struct time_ms tm;
		int64 return_value = -1;

		if(parse_date_time(buff, length, &tm))
			return_value = tm2timestamp_ms(tm);

		if(return_value != -1)
			return return_value - kGmtOffsetMs;
    }

	*error = 1;
	return 0;
}

#endif

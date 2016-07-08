#ifdef STANDARD
/* STANDARD is defined, don't use any mysql functions */
#include <string.h>

#ifdef __WIN__
typedef unsigned __int64 ulonglong; /* Microsofts 64 bit types */
typedef __int64 longlong;
#else
typedef unsigned long long ulonglong;
typedef long long longlong;
#endif

#else

#include <my_global.h>
#include <my_sys.h>
#if defined(MYSQL_SERVER)
#include <m_string.h>
#else
/* when compiled as standalone */
#include <string.h>
#endif
#endif

#include <mysql.h>
#include <ctype.h>

#if defined(__WIN__)
#ifndef _WINSOCKAPI_
#define _WINSOCKAPI_
#endif
#include <Windows.h>
#elif defined(__APPLE__) && defined(__MACH__)
#include <sys/time.h>
#else
#include <time.h>
#endif

#ifdef HAVE_DLOPEN

#define UNUSED(expr) do { (void)(expr); } while (0)

#define DATE_TIME_LENGTH 19
#define	DATE_LENGTH 10
#define TIME_LENGTH 8

const char kDateTimeFormat[] = "%Y-%m-%d %H:%M:%S";
const char kDateFormat[] = "%Y-%m-%d";
const char kTimeFormat[] = "%H:%M:%S";

extern "C" {
    my_bool unix_timestamp_ms_init(UDF_INIT* initid, UDF_ARGS* args, char* message);
    void unix_timestamp_ms_deinit(UDF_INIT* initid);
    ulonglong unix_timestamp_ms(UDF_INIT* initid, UDF_ARGS* args, char* is_null, char* error);
}

#if defined(_WIN32)

ulonglong get_time_since_epoch_ms()
{
    _FILETIME ft;
    GetSystemTimeAsFileTime(&ft);
    ulonglong ns100 = (static_cast<ulonglong>(ft.dwHighDateTime) << 32 | static_cast<ulonglong>(ft.dwLowDateTime)) - 116444736000000000LL;
    return ns100 / 10000;
}

#elif defined(__APPLE__) && defined(__MACH__)

ulonglong get_time_since_epoch_ms()
{
    struct timeval tv;
    gettimeofday(&tv, NULL);
    return static_cast<ulonglong>(tv.tv_sec)  * 1000 + static_cast<ulonglong>(tv.tv_usec) / 1000;
}

#else

ulonglong get_time_since_epoch_ms()
{
    struct timespec ts;
    clock_gettime(CLOCK_REALTIME, &ts);
    return static_cast<ulonglong>(ts.tv_sec) * 1000 + static_cast<ulonglong>(ts.tv_nsec) / 1000000;
}

#endif

my_bool unix_timestamp_ms_init(UDF_INIT* initid, UDF_ARGS* args, char* message)
{
    UNUSED(initid);
    
    if (args->arg_count > 1)
    {
        strcpy(message,"UNIX_TIMESTAMP_MS requires no more than one argument");
        return 1;
    }
    
    if(args->arg_count == 1 && args->lengths[0] > 19)
    {
        strcpy(message,"UNIX_TIMESTAMP_MS received invalid date time as argument");
        return 1;
    }
    
    return 0;
}

void unix_timestamp_ms_deinit(UDF_INIT* initid)
{
    UNUSED(initid);
}

ulonglong unix_timestamp_ms(UDF_INIT* initid, UDF_ARGS* args, char* is_null, char* error)
{
    UNUSED(initid);
    UNUSED(is_null);
    
    if(args->arg_count == 0)
        return get_time_since_epoch_ms();
    
    struct tm tm;
    char buff[20];
    unsigned long length =  args->lengths[0];
    memcpy(buff, args->args[0], length);
    buff[length] = '\0';
    
    if(length == DATE_TIME_LENGTH)
    {
        if (strptime(buff, kDateTimeFormat, &tm) != NULL)
            return mktime(&tm)*1000;
    }
    else if (length == DATE_LENGTH)
    {
        tm.tm_hour = 0;
        tm.tm_min = 0;
        tm.tm_sec = 0;
        tm.tm_isdst = -1;
        
        if (strptime(buff, kDateFormat, &tm) != NULL)
            return mktime(&tm)*1000;
    }
    else if (length == TIME_LENGTH)
    {
        if (strptime(buff, kTimeFormat, &tm) != NULL)
        {
            time_t rawtime;
            time (&rawtime);
            struct tm * local_tm = localtime (&rawtime);
            local_tm->tm_hour = tm.tm_hour;
            local_tm->tm_min = tm.tm_min;
            local_tm->tm_sec = tm.tm_sec;
            return mktime(local_tm)*1000;
        }
    }
    
    *error = 1;
    return 0;
}

#endif

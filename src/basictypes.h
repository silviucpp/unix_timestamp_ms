//
//  basictypes.h
//
//  Created by Silviu Caragea on 7/8/16.
//  Copyright © 2016 silviu.cpp@gmail.com. All rights reserved.
//

#ifndef UNIX_TIMESTAMP_MS_BASICTYPES_H_
#define UNIX_TIMESTAMP_MS_BASICTYPES_H_

#define UNUSED(expr) do { (void)(expr); } while (0)

#ifdef STANDARD
	typedef unsigned char uint8;
	typedef unsigned short uint16;
	typedef unsigned int uint32;
	typedef unsigned long long uint64;
	typedef signed char int8;
	typedef short int16;
	typedef int int32;
	typedef long long int64;
#else
	#include <mysql/my_global.h>
#endif

#endif

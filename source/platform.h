#pragma once

#include <assert.h>

typedef unsigned char		byte8;

typedef unsigned char		uint8;
typedef unsigned short		uint16;
typedef unsigned int		uint32;
typedef unsigned long long	uint64;

typedef signed char			int8;
typedef short				int16;
typedef int					int32;
typedef long long			int64;

void check_gl_error(const char* file_name, int line_number);

#define GELATO_HASH_SEED 0xCAFEBABE
#define GELATO_CONTENT_PACKAGE_MAGIC 0xCAFEBABE
#define __DEBUG__

#ifdef __DEBUG__
    #define GL_CHECK(function)\
    function;\
    check_gl_error(__FILE__, __LINE__);
#else
    #define GL_CHECK(function) function
#endif

#define ARRAY_SIZE(array) \
    sizeof(array) / sizeof(array[0]) 
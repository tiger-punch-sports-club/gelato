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

#define GELATO_HASH_SEED 0xCAFEBABE
#define GELATO_CONTENT_PACKAGE_MAGIC 0xCAFEBABE

#define GL_CHECK(function) 

#define ARRAY_SIZE(array) \
    sizeof(array) / sizeof(array[0]) 
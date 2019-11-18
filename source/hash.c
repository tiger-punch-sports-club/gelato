#include "hash.h"
#include <string.h>
#include <xxHash/xxhash.h>

Hash hash_data(void* data, uint64 size) 
{
    Hash hash =
    {
        ._value = XXH64(data, size, GELATO_HASH_SEED)
    };
    return hash;
}

Hash hash_string(const char* str)
{
	uint64 size = strlen(str);
	return hash_data((void*) str, size);
}

uint64 murmur_hash3(uint64 k)
{
	k ^= k >> 33;
	k *= 0xff51afd7ed558ccd;
	k ^= k >> 33;
	k *= 0xc4ceb9fe1a85ec53;
	k ^= k >> 33;
	return k;
}
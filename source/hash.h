#pragma once
#include "platform.h"

typedef struct Hash 
{
    uint64 _value;
} Hash;

Hash hash_data(void* data, uint64 size);
Hash hash_string(const char* str);
uint64 murmur_hash3(uint64 k);
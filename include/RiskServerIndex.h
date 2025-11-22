#pragma once

#include <cstddef>
#include <cstdint>
#include <cstring>

using u64 = unsigned long long;


template<typename T>
inline void GetKeyImpl(uint64_t &key, T value)
{
    if (value < 0) value = 0;

    if(value >= 10000)
    {
        key *= 100000;
    } else if(value >= 1000)
    {
        key *= 10000;
    } else if(value >= 100)
    {
        key *= 1000;
    } else if(value >= 10)
    {
        key *= 100;
    } else {
        key *= 10;
    }

    key += value;
}

template<>
inline void GetKeyImpl(uint64_t &key, const char* value)
{
    uint64_t tmp = 0u;
    size_t len = strlen(value);
    len = len > 20 ? 20 : len;
    for(int i = 0; i < len; i++)
    {
        // option 特化处理 合约代码过于长
        if(value[i] == '-') continue;

        tmp = value[i] - '0';
        if(tmp > 10) key *= 100;
        else key *= 10;
        key += tmp;
    }
}

template<>
inline void GetKeyImpl(uint64_t &key, char value)
{
    if (value < 0) value = 0;

    if (value > 100) key *= 1000;
    else key *= 100;

    key += value;
}

template<typename T, typename ...Args>
inline void GetKeyImpl(uint64_t &key, T value, Args ...args)
{
    GetKeyImpl(key, value);
    GetKeyImpl(key, args...);
}


template<unsigned N = 0, typename ...Args>
class CommonKeyWithInit {
public:
    static uint64_t GetKey(Args ...args)
    {
        uint64_t key = N;
        ::GetKeyImpl(key, args...);
        return key;
    }
};


using SecCodeKey = CommonKeyWithInit<1, const char *, const char *>;
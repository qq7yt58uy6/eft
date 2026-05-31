#pragma once
#include <string>
#include <unordered_map>

class Hash
{
    inline static std::unordered_map<const char*, size_t> cached_hashes_;

    static size_t fnv_1(const std::string& text)
    {
        std::size_t hash = 14695981039346656037u;
        for (const char it : text)
        {
            hash *= 1099511628211u;
            hash ^= it;
        }

        return hash;
    }

public:
    static size_t get_cached(const char* text)
    {
        if (cached_hashes_.contains(text))
        {
            return cached_hashes_.at(text);
        }
        size_t hash = fnv_1(text);
        cached_hashes_.insert_or_assign(text, hash);
        return hash;
    }

    static size_t get_const(const char* text)
    {
        return fnv_1(text);
    }

    static size_t get(const char* text)
    {
        return fnv_1(text);
    }
};

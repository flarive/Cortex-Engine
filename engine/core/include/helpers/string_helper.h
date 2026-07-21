#pragma once

#include <string>
#include <algorithm>
#include <cctype>

namespace engine
{
    static bool containsInsensitive(const std::string& haystack, const std::string& needle)
    {
        if (needle.empty())
            return true;

        auto it = std::search(
            haystack.begin(), haystack.end(),
            needle.begin(), needle.end(),
            [](char a, char b)
            {
                return std::tolower(static_cast<unsigned char>(a)) == std::tolower(static_cast<unsigned char>(b));
            }
        );

        return it != haystack.end();
    }
}

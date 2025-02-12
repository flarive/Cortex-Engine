#pragma once

class NonCopyable
{
    NonCopyable(const NonCopyable&) = delete;
    NonCopyable& operator= (const NonCopyable&) = delete;
    NonCopyable(NonCopyable&&) = delete;
    NonCopyable& operator= (const NonCopyable&&) = delete;
protected:
    NonCopyable() = default;
};

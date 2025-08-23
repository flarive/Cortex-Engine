#pragma once

/**
 * @class NonCopyable
 * @brief A utility class to prevent copying and moving of derived classes.
 *
 * This class is designed to be inherited from. By deleting the copy constructor,
 * copy assignment operator, move constructor, and move assignment operator,
 * it ensures that any class inheriting from `NonCopyable` cannot be copied or moved.
 * This is useful for classes that manage non-copyable resources (e.g., file handles,
 * mutexes, or unique pointers) or for enforcing singleton patterns.
 *
 * @note The constructor is protected to allow inheritance but prevent direct instantiation.
 *       The destructor is not explicitly declared, so it defaults to public and virtual
 *       if needed in polymorphic hierarchies.
 */

class NonCopyable
{
    // Delete the copy constructor to prevent copying.
    NonCopyable(const NonCopyable&) = delete;

    // Delete the copy assignment operator to prevent assignment.
    NonCopyable& operator=(const NonCopyable&) = delete;

    // Delete the move constructor to prevent moving.
    NonCopyable(NonCopyable&&) = delete;

    // Delete the move assignment operator to prevent move assignment.
    NonCopyable& operator=(NonCopyable&&) = delete;

protected:
    // Allow construction of derived classes.
    NonCopyable() = default;
};

class NonCopyableButMovable
{
    protected:
        NonCopyableButMovable() = default;
        NonCopyableButMovable(NonCopyableButMovable&&) noexcept = default;
        NonCopyableButMovable& operator=(NonCopyableButMovable&&) noexcept = default;

    public:
        NonCopyableButMovable(const NonCopyableButMovable&) = delete;
        NonCopyableButMovable& operator=(const NonCopyableButMovable&) = delete;
};

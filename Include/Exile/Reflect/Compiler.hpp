#pragma once

#include <cstddef>
#include <array>
#include <concepts>
#include <algorithm>

/**
 * Compile-time functionality to support reflection
 */

namespace Exi::Reflect
{

    /* Helper struct to pass string literals as template parameters */
    template <std::size_t N>
    struct TemplateString
    {
        consteval TemplateString(const char(&str)[N])
        {
            std::copy_n(str, N, m_Data.begin());
        }
        [[nodiscard]] consteval const char* Data() const { return m_Data.data(); }
        std::array<char, N> m_Data;
    };

    /**
     * Compile-time hash function, courtesy of https://simoncoenen.com/blog/programming/StaticReflection
     * We use it to hash class/field/type names into IDs
     * @param str
     * @param value
     * @return
     */
    static constexpr std::size_t Hash(const char* const str, std::size_t value = 0) noexcept
    {
        return (str[0] == '\0') ? value : Hash(&str[1], (value ^ ((std::size_t)str[0])) * 0x100000001b3);
    }

    template <typename T>
    concept Integer8  = (std::integral<T> && (sizeof(T) == 1));
    template <typename T>
    concept Integer16 = (std::integral<T> && (sizeof(T) == 2));
    template <typename T>
    concept Integer32 = (std::integral<T> && (sizeof(T) == 4));
    template <typename T>
    concept Integer64 = (std::integral<T> && (sizeof(T) == 8));

    /**
     * Helper template for getting information about a function
     * @tparam T
     */
    template <class T>
    struct FunctionTraits : FunctionTraits<decltype(&T::operator())> { };

    /**
     * FunctionTraits specialization for T = function
     * @tparam ReturnType
     * @tparam Args
     */
    template <class ReturnType, class... Args>
    struct FunctionTraits<ReturnType(Args...)>
    {
        using Return = ReturnType;
        using FnType = ReturnType(Args...);
        static inline constexpr int ArgCount = sizeof...(Args);

        template <size_t Index>
        struct Arg {
            using Type = typename std::tuple_element<Index, std::tuple<Args...>>::type;
        };
    };

    /**
     * FunctionTraits specialization for T = function pointer
     * @tparam ReturnType
     * @tparam Args
     */
    template <typename ReturnType, typename... Args>
    struct FunctionTraits<ReturnType (*)(Args...)> : public FunctionTraits<ReturnType(Args...)> { };

    /**
     * FunctionTraits specialization for T = member function pointer
     * @tparam Class
     * @tparam ReturnType
     * @tparam Args
     */
    template <typename Class, typename ReturnType, typename... Args>
    struct FunctionTraits<ReturnType (Class::*)(Args...)> : public FunctionTraits<ReturnType(Args...)>
    {
        using Owner = Class&;
    };

    /**
     * FunctionTraits specialization for T = const member function pointer
     * @tparam Class
     * @tparam ReturnType
     * @tparam Args
     */
    template <typename Class, typename ReturnType, typename... Args>
    struct FunctionTraits<ReturnType (Class::*)(Args...) const> : public FunctionTraits<ReturnType(Args...)>
    {
        using Owner = const Class&;
    };

}

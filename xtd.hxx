/**
 * @file xtd.hxx
 * @author Felixaszx (felixaszx@outlook.com)
 * @brief Core defines and help tempaltes for xtd
 * @version 0.1
 * @date 2025-07-12
 * 
 * @copyright MIT License Copyright (c) 2025
 * 
 */

#ifndef XTD_HXX
#define XTD_HXX

#include <atomic>
#include <chrono>
#include <concepts>
#include <expected>
#include <filesystem>
#include <fstream>
#include <functional>
#include <iostream>
#include <memory_resource>
#include <print>
#include <ranges>
#include <set>
#include <source_location>
#include <stdexcept>
#include <string>
#include <string_view>
#include <thread>
#include <unordered_set>
#include <variant>
#include <vector>

#ifndef XTD_EXT_HPP_NAMESPACE
    #define XTD_EXT_HPP_NAMESPACE xtd
#endif

#ifndef XTD_EXT_HPP_NAMESPACE_CAPITAL
    #define XTD_EXT_HPP_NAMESPACE_CAPITAL XTD
#endif

namespace XTD_EXT_HPP_NAMESPACE::i_ // internal namespace
{
    template <class... F>
    struct overload_call_t : F...
    {
        using F::operator()...;
    };
}; // namespace XTD_EXT_HPP_NAMESPACE::i_

namespace XTD_EXT_HPP_NAMESPACE_CAPITAL
{
    using i8 = std::int8_t;
    using u8 = std::uint8_t;
    using i16 = std::int16_t;
    using u16 = std::uint16_t;
    using i32 = std::int32_t;
    using u32 = std::uint32_t;
    using i64 = std::int64_t;
    using u64 = std::uint64_t;
    using imax = std::intmax_t;
    using umax = std::uintmax_t;
    using sz = std::size_t;
    using ssz = ssize_t;
    using ptr_diff = ptrdiff_t;

    using char8 = char;
    using uchar8 = unsigned char;
    using char16 = char16_t;
    using char32 = char32_t;
    using wchar = wchar_t;

    using f32 = float;
    using f64 = double;

    // atomic variants
    using atomic_bool = std::atomic_bool;
    using atomic_i8 = std::atomic_int8_t;
    using atomic_u8 = std::atomic_uint8_t;
    using atomic_i16 = std::atomic_int16_t;
    using atomic_u16 = std::atomic_uint16_t;
    using atomic_i32 = std::atomic_int32_t;
    using atomic_u32 = std::atomic_uint32_t;
    using atomic_i64 = std::atomic_int64_t;
    using atomic_u64 = std::atomic_uint64_t;
    using atomic_imax = std::atomic_intmax_t;
    using atomic_umax = std::atomic_uintmax_t;
    using atomic_sz = std::atomic_size_t;
    using atomic_ssz = std::atomic<ssize_t>;
    using atomic_ptr_diff = std::atomic_ptrdiff_t;

    using atomic_char8 = std::atomic_char;
    using atomic_uchar8 = std::atomic_uchar;
    using atomic_char16 = std::atomic_char16_t;
    using atomic_char32 = std::atomic_char32_t;
    using atomic_wchar = std::atomic_wchar_t;

    inline const size_t DEFAULT_ALITNMENT = 16;
}; // namespace XTD_EXT_HPP_NAMESPACE_CAPITAL

namespace XTD_EXT_HPP_NAMESPACE
{
    using namespace XTD_EXT_HPP_NAMESPACE_CAPITAL;

    template <typename T>
    inline constexpr auto //
    aligned [[nodiscard]] (const T& size, std::size_t alignment = DEFAULT_ALITNMENT) noexcept
        requires std::is_arithmetic_v<T>
    {
        return alignment * ((size - 1) / alignment) + alignment;
    }

    template <typename T>
    inline constexpr auto //
    align(T& size, std::size_t alignment = DEFAULT_ALITNMENT) noexcept
        requires std::is_arithmetic_v<T>
    {
        size = aligned(size, alignment);
        return size;
    }

    // retuns number of elements in a given array
    template <typename T>
        requires std::is_bounded_array_v<T>
    inline consteval auto //
    elmsof [[nodiscard]] (const T& arr) noexcept
    {
        return sizeof(T) / sizeof(arr[0]);
    }

    template <typename T>
    concept sizeof2_compatible = requires(T container) //
    {
        { container.size() } -> std::convertible_to<std::size_t>;
        { sizeof(typename std::remove_cvref_t<decltype(container)>::value_type) } -> std::convertible_to<std::size_t>;
    };

    // returns number of bytes in a given array base container
    inline constexpr auto //
    sizeof2 [[nodiscard]] (const sizeof2_compatible auto& container)
    {
        using T = std::remove_cvref_t<decltype(container)>;
        return sizeof(typename T::value_type) * container.size();
    }

    template <typename T>
        requires std::is_arithmetic_v<T>
    inline constexpr T max_v = std::numeric_limits<T>::max();

    template <typename T>
        requires std::is_arithmetic_v<T>
    inline constexpr T min_v = std::numeric_limits<T>::min();

    template <typename T>
        requires std::is_floating_point_v<T>
    inline constexpr T inf_v = std::numeric_limits<T>::infinity();

    inline constexpr void //
    logln(const auto& msg = {})
    {
        std::println("LOG: {}", msg);
    }

    inline constexpr void //
    errln(const auto& msg, std::source_location src = std::source_location::current())
    {
        std::println(stderr,
                     "\n{}:{}:{}:\nERROR: {}", //
                     src.file_name(), src.line(),
                     src.column(), //
                     msg);
    }

    inline constexpr void //
    warnln(const auto& msg)
    {
        std::println("WARNNING: {}", msg);
    }

    template <typename... F>
    inline constexpr i_::overload_call_t<F...> //
    overload(F&&... funcs)
    {
        return i_::overload_call_t{std::forward<F>(funcs)...};
    }

}; // namespace XTD_EXT_HPP_NAMESPACE

namespace XTD_EXT_HPP_NAMESPACE::literals
{
    using namespace std::chrono_literals;
    using namespace std::string_literals;
    using namespace std::string_view_literals;

    inline consteval std::size_t //
    operator""_b(unsigned long long size)
    {
        return size;
    }

    inline consteval std::size_t //
    operator""_kb(unsigned long long size)
    {
        return 1024_b * size;
    }

    inline consteval std::size_t //
    operator""_mb(unsigned long long size)
    {
        return 1024_kb * size;
    }

    inline consteval std::size_t //
    operator""_gb(unsigned long long size)
    {
        return 1024_mb * size;
    }

    inline consteval std::size_t //
    operator""_kb(long double sizef)
    {
        return 1024_b * sizef;
    }

    inline consteval std::size_t //
    operator""_mb(long double sizef)
    {
        return 1024_kb * sizef;
    }

    inline consteval std::size_t //
    operator""_gb(long double sizef)
    {
        return 1024_mb * sizef;
    }

    inline consteval std::int8_t //
    operator""_i8(unsigned long long i)
    {
        return static_cast<std::int8_t>(i);
    }

    inline consteval std::uint8_t //
    operator""_u8(unsigned long long i)
    {
        return static_cast<std::uint8_t>(i);
    }

    inline consteval std::int16_t //
    operator""_i16(unsigned long long i)
    {
        return static_cast<std::int16_t>(i);
    }

    inline consteval std::uint16_t //
    operator""_u16(unsigned long long i)
    {
        return static_cast<std::uint16_t>(i);
    }

    inline consteval std::int32_t //
    operator""_i32(unsigned long long i)
    {
        return static_cast<std::int32_t>(i);
    }

    inline consteval std::uint32_t //
    operator""_u32(unsigned long long i)
    {
        return static_cast<std::uint32_t>(i);
    }

    inline consteval std::int64_t //
    operator""_i64(unsigned long long i)
    {
        return static_cast<std::int64_t>(i);
    }

    inline consteval std::uint64_t //
    operator""_u64(unsigned long long i)
    {
        return static_cast<std::uint64_t>(i);
    }

    inline consteval std::intmax_t //
    operator""_imax(unsigned long long i)
    {
        return static_cast<std::intmax_t>(i);
    }

    inline consteval std::uintmax_t //
    operator""_umax(unsigned long long i)
    {
        return static_cast<std::uintmax_t>(i);
    }

}; // namespace XTD_EXT_HPP_NAMESPACE::literals

#ifndef XTD_NO_MACROS
    #define castc(to, from) const_cast<to>(from)
    #define castd(to, from) dynamic_cast<to>(from)
    #define castr(to, from) reinterpret_cast<to>(from)
    #define casts(to, from) static_cast<to>(from)
    #define castf(to, from) ((to)(from))
#endif

#ifndef XTD_NO_DATA_STRUCTURES
    #include "xtd_ds.hxx"
#endif

#ifndef XTD_NO_NAMESPACE
using namespace XTD_EXT_HPP_NAMESPACE;
using namespace XTD_EXT_HPP_NAMESPACE::literals;
#endif

#endif // XTD_HXX

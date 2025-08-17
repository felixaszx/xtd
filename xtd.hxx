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

#include <print>
#include <atomic>
#include <chrono>
#include <type_traits>
#include <source_location>

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
    inline const std::size_t DEFAULT_ALITNMENT = 16;

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
    using usz = std::size_t;
    using ssz = ssize_t;
    using ptrdiff = std::ptrdiff_t;

    using uchar = unsigned char;
    using char8 = char8_t;
    using char16 = char16_t;
    using char32 = char32_t;
    using wchar = wchar_t;

    using f32 = float;
    using f64 = double;

    // atomic variants
    using atomic_bool = std::atomic<bool>;
    using atomic_i8 = std::atomic<i8>;
    using atomic_u8 = std::atomic<u8>;
    using atomic_i16 = std::atomic<i16>;
    using atomic_u16 = std::atomic<u16>;
    using atomic_i32 = std::atomic<i32>;
    using atomic_u32 = std::atomic<u32>;
    using atomic_i64 = std::atomic<i64>;
    using atomic_u64 = std::atomic<u64>;
    using atomic_imax = std::atomic<imax>;
    using atomic_umax = std::atomic<umax>;
    using atomic_usz = std::atomic<usz>;
    using atomic_ssz = std::atomic<ssz>;
    using atomic_ptrdiff = std::atomic<ptrdiff>;

    using atomic_char = std::atomic<char>;
    using atomic_uchar = std::atomic<uchar>;
    using atomic_char8 = std::atomic<char8>;
    using atomic_char16 = std::atomic<char16>;
    using atomic_char32 = std::atomic<char32>;
    using atomic_wchar = std::atomic<wchar>;

    using atomic_f32 = std::atomic<f32>;
    using atomic_f64 = std::atomic<f64>;

    template <auto C = std::numeric_limits<std::size_t>::max(),
              typename Idx = std::size_t,
              typename Accessor = void,
              typename T = void>
        requires std::unsigned_integral<Idx>
    class ts_idx
    {
        friend Accessor;

      public:
        inline static const ts_idx null_idx = static_cast<Idx>(-1);
        inline static const ts_idx zero_idx = static_cast<Idx>(0);

      private:
        Idx idx_ = null_idx;
        inline constexpr ts_idx(Idx i) { idx_ = i; }

      public:
        inline constexpr operator bool() const noexcept { return null_idx.idx_ != idx_; }
        inline constexpr operator Idx() const noexcept { return idx_; }
        inline constexpr bool operator==(const ts_idx& x) const noexcept { return idx_ == x.idx_; }
        inline static consteval decltype(C) idx_class() noexcept { return C; };
        inline const ts_idx off_by(Idx off) const noexcept { return *this ? ts_idx(idx_ + off) : null_idx; };

        ts_idx() = default;
    };

}; // namespace XTD_EXT_HPP_NAMESPACE_CAPITAL

namespace XTD_EXT_HPP_NAMESPACE
{
    using namespace XTD_EXT_HPP_NAMESPACE_CAPITAL;

    template <typename T>
    inline constexpr auto //
    aligned [[nodiscard]] (const T& size, std::size_t alignment = DEFAULT_ALITNMENT) noexcept
        requires std::is_arithmetic_v<T>
    {
        std::u32string a;
        return alignment * ((size - 1) / alignment) + alignment;
    }

    inline constexpr auto //
    ceili [[nodiscard]] (const std::integral auto& dividend, const std::integral auto& divisor) noexcept
    {
        return dividend == 0 ? 0 : (dividend - 1) / divisor + 1;
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
    inline constexpr auto //
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
    sizeof2 [[nodiscard]] (const sizeof2_compatible auto&& container)
    {
        using T = std::remove_cvref_t<decltype(container)>;
        return sizeof(typename T::value_type) * std::forward<T>(container).size();
    }

    // returns number of bytes in a given array base container
    template <typename T>
        requires(std::is_bounded_array_v<T>)
    inline constexpr auto //
    sizeof2 [[nodiscard]] (T&& container)
    {
        return sizeof(std::forward<T>(container));
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

    template <typename... Args>
    struct logln
    {
        inline constexpr //
            logln(std::string_view format, Args&&... msg, std::source_location src = std::source_location::current())
        {
            std::println("[{}] {} ({}:{}:{})", "LOG",                         //
                         std::vformat(format, std::make_format_args(msg...)), //
                         src.file_name(), src.line(), src.column());
        }
    };

    template <typename... Args>
    struct errln
    {
        inline constexpr //
            errln(std::string_view format, Args&&... msg, std::source_location src = std::source_location::current())
        {
            std::println(stderr, "[{}] {} ({}:{}:{})", "ERROR",               //
                         std::vformat(format, std::make_format_args(msg...)), //
                         src.file_name(), src.line(), src.column());
        }
    };

    template <typename... Args>
    struct warnln
    {
        inline constexpr //
            warnln(std::string_view format, Args&&... msg, std::source_location src = std::source_location::current())
        {
            std::println("[{}] {} ({}:{}:{})", "WARNING",                     //
                         std::vformat(format, std::make_format_args(msg...)), //
                         src.file_name(), src.line(), src.column());
        }
    };

    template <typename... Args> //
    logln(std::string_view, Args&&...) -> logln<Args...>;

    template <typename... Args> //
    errln(std::string_view, Args&&...) -> errln<Args...>;

    template <typename... Args> //
    warnln(std::string_view, Args&&...) -> warnln<Args...>;

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

    inline consteval float //
    operator""_f32(long double i)
    {
        return static_cast<f32>(i);
    }

    inline consteval float //
    operator""_f64(long double i)
    {
        return static_cast<f64>(i);
    }

    template <typename To, typename From>
    inline constexpr To //
    castc(From&& from)
    {
        return const_cast<To>(std::forward<From>(from));
    }

    template <typename To, typename From>
    inline constexpr To //
    castd(From&& from)
    {
        return dynamic_cast<To>(std::forward<From>(from));
    }

    template <typename To, typename From>
    inline constexpr To //
    castr(From&& from)
    {
        return reinterpret_cast<To>(std::forward<From>(from));
    }

    template <typename To, typename From>
    inline constexpr To //
    casts(From&& from)
    {
        return static_cast<To>(std::forward<From>(from));
    }

    template <typename To, typename From>
    inline constexpr To //
    castf(From&& from)
    {
        return ((To)(std::forward<From>(from)));
    }
}; // namespace XTD_EXT_HPP_NAMESPACE::literals

#if !defined(XTD_NO_NAMESPACE)
using namespace XTD_EXT_HPP_NAMESPACE_CAPITAL;
using namespace XTD_EXT_HPP_NAMESPACE;
using namespace XTD_EXT_HPP_NAMESPACE::literals;
#endif

#endif // XTD_HXX

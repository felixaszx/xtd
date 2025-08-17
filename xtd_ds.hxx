/**
 * @file xtd_ds.hxx
 * @author Felixaszx (felixaszx@outlook.com)
 * @brief Data structures and adaptors for xtd
 * @version 0.1
 * @date 2025-07-12
 *
 * @copyright MIT License Copyright (c) 2025
 *
 */

#ifndef XTD_DS_HXX
#define XTD_DS_HXX

#include <vector>
#include <deque>
#include <concepts>
#include <numbers>
#include <span>
#include <ranges>
#include <flat_map>
#include <type_traits>
#include <unordered_set>

// Helper data structure classes
namespace XTD_EXT_HPP_NAMESPACE
{
    template <typename T, typename Idx>
        requires std::unsigned_integral<Idx> && //
                 (sizeof(T) >= sizeof(Idx))
    union jump_array_elm
    {
        Idx next_ = std::numeric_limits<Idx>::max();
        std::byte elm_[sizeof(T)];
    };

    template <typename T,
              typename Idx = std::size_t,
              typename C = std::vector<jump_array_elm<T, Idx>>,
              bool checking = false>
        requires std::unsigned_integral<Idx> && //
                 (sizeof(T) >= sizeof(Idx))
    class jump_array
    {
        // enable check will have high overhead
      public:
        using value_type = T;
        using size_type = Idx;
        using difference_type = std::ptrdiff_t;
        using pointer = T*;
        using const_pointer = const T*;
        using reference = T&;
        using const_reference = const T&;

      private:
        Idx next_ = 0; // empty
        Idx size_ = 0;
        Idx last_ = std::numeric_limits<Idx>::max(); // empty
        C c_ = {};

      public:
        constexpr bool //
        contains [[nodiscard]] (Idx idx) const;

        template <typename S>
        constexpr auto& // enable check will have high overhead
        at(this S&& self, Idx idx);

        template <typename S>
        constexpr auto& //
        operator[](this S&& self, Idx idx) noexcept;

        template <typename S>
        constexpr auto //
        data(this S&& self) noexcept;

        constexpr size_type //
        size() const noexcept;

        constexpr size_type //
        effective_size() const noexcept;

        constexpr size_type //
        capacity() const noexcept;

        constexpr void //
        clear();

        constexpr bool //
        erase(Idx idx);

        constexpr void //
        expand();

        constexpr void //
        expand_to(Idx size);

        template <class... Args>
        constexpr Idx //
        emplace [[nodiscard]] (Args&&... args);

        template <typename... CArgs>
        constexpr jump_array(CArgs... container_args);

        constexpr ~jump_array();
    };

    constexpr void //
    replacing_erase(auto& container, size_t idx);


#include "xtd_ds.ixx"
}; // namespace XTD_EXT_HPP_NAMESPACE

#endif // XTD_DS_HXX

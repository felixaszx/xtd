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
              typename C = std::pmr::vector<jump_array_elm<T, Idx>>,
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
        Idx next_ = 0;
        Idx size_ = 0;
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
    replacing_erase(auto& container, std::size_t idx);

    template <typename T, typename Idx = std::size_t>
        requires std::is_default_constructible_v<T> && //
                 std::unsigned_integral<Idx> && (sizeof(T) >= sizeof(Idx))
    struct tree_array
    {
      public:
        struct node
        {
            friend tree_array;

          private:
            Idx parent_ = std::numeric_limits<Idx>::max();
            Idx children_ = std::numeric_limits<Idx>::max();

          public:
            T data_ = {};

            template <typename... Args>
            node(Args&&... args)
                : data_(std::forward<Args>(args)...)
            {
            }
        };

      protected:
        jump_array<node, Idx> nodes_ = {};
        jump_array<std::pmr::deque<Idx>, Idx> children_ = {};
        mutable std::pmr::memory_resource* mem_res_ = nullptr;

        constexpr void //
        clear_parent(Idx node);

        constexpr void //
        reset_children(Idx node);

      public:
        constexpr //
            tree_array(std::pmr::memory_resource* mem_res = std::pmr::new_delete_resource());

        template <typename... Args>
        constexpr Idx // return node index
        emplace [[nodiscard]] (Args&&... args);

        constexpr bool // very expensive when node is not std::numeric_limits<Idx>::max()!
        containes(Idx node) const;

        constexpr Idx //
        size() const;

        template <typename S>
        constexpr auto& // mostly used internally
        get(this S&& self, Idx node);

        constexpr void //
        erase(Idx node);

        constexpr Idx //
        has_parent(Idx node) const;

        constexpr Idx //
        get_parent(Idx node) const;

        constexpr bool //
        has_children(Idx node) const;

        constexpr const std::pmr::deque<Idx>& //
        get_children(Idx node) const;

        constexpr void //
        add_child(Idx at, Idx child);

        constexpr void //
        reset_parent(Idx node);

        constexpr Idx // linear search, return index in children array
        find_child(Idx at, Idx child) const;

        constexpr Idx //
        expand_to(Idx to);

        constexpr std::vector<Idx> //
        sort [[nodiscard]] (Idx at) const;

        template <typename S, typename F>
            requires std::invocable<F, Idx, tree_array&>
        constexpr void //
        traverse(this S&& self, Idx at, F&& callback);

        constexpr void //
        clear();

        constexpr void // very expensive!
        cut(Idx at);

        constexpr Idx // very expensive!, node 0 will always be root, return the index of new root
        insert(Idx to, const tree_array& tree, Idx from)
            requires std::is_copy_constructible_v<T>;

        constexpr Idx // node 0 will always be root, return the index of new root
        take(Idx to, tree_array& tree, Idx from)
            requires std::is_move_constructible_v<T>;
    };

#include "xtd_ds.ixx"
}; // namespace XTD_EXT_HPP_NAMESPACE

#endif // XTD_DS_HXX

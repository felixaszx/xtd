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
#include <concepts>
#include <numbers>
#include <span>
#include <type_traits>
#include <unordered_set>

// Helper data structure classes
namespace XTD_EXT_HPP_NAMESPACE
{
    template <typename T, typename Idx = std::size_t, bool checking = true>
        requires std::unsigned_integral<Idx> && //
                 (sizeof(T) >= sizeof(Idx))
    class jump_span
    {
        // enable check will have high overhead
      public:
        using value_type = T;
        using size_type = std::size_t;
        using difference_type = std::ptrdiff_t;
        using pointer = T*;
        using const_pointer = const T*;
        using reference = T&;
        using const_reference = const T&;

      private:
        struct jump_span_elm
        {
            Idx next_ = std::numeric_limits<Idx>::max();
            T elm_;
        };

        Idx next_ = 0;
        std::span<jump_span_elm> span_ = nullptr;

      public:
        inline constexpr bool //
        occupied [[nodiscard]] (Idx idx) const
        {
            if (idx >= span_.size())
            {
                return false;
            }

            Idx next = next_;
            while (next != std::numeric_limits<Idx>::max())
            {
                if (next == idx)
                {
                    return false;
                }
                next = span_[next].next_;
            }
            return true;
        }

        inline constexpr reference // enable check will have high overhead
        at(Idx idx) const
        {
            if constexpr (checking)
            {
                if (!occupied(idx))
                {
                    throw std::invalid_argument("This index is refereing to an unoccupied position");
                }
            }
            return span_[idx].elm_;
        }

        inline constexpr reference //
        operator[](Idx idx) const noexcept
        {
            return span_[idx].elm_;
        }

        inline constexpr pointer //
        data() const noexcept
        {
            return reinterpret_cast<pointer>(span_.data());
        }

        inline constexpr size_type //
        size() const noexcept
        {
            return span_.size();
        }

        inline constexpr size_type //
        remain_slots() const noexcept
        {
            size_type remaining = 0;
            Idx next = next_;
            while (next != std::numeric_limits<Idx>::max())
            {
                remaining++;
                next = span_[next].next_;
            }

            return remaining;
        }

        inline constexpr bool //
        erase(Idx idx)
        {
            if constexpr (checking)
            {
                if (!occupied(idx))
                {
                    return false;
                }
            }

            if constexpr (std::is_compound_v<T> && !std::is_pointer_v<T>)
            {
                std::destroy_at(&span_[idx].elm_);
            }

            span_[idx].next_ = next_;
            next_ = idx;
            return true;
        }

        template <class... Args>
        inline constexpr Idx //
        emplace [[nodiscard]] (Args&&... args)
        {
            if (next_ == std::numeric_limits<Idx>::max())
            {
                return std::numeric_limits<Idx>::max();
            }

            Idx curr = next_;
            next_ = span_[curr].next_;
            std::construct_at(&span_[curr].elm_, std::forward<Args>(args)...);
            return curr;
        }

        inline constexpr jump_span(std::byte* data, Idx elm_size)
            : next_(0),
              span_((jump_span_elm*)(data), elm_size)
        {
            for (Idx i = 0; i < elm_size; i++)
            {
                span_[i].next_ = i + 1;
            }
            span_[elm_size - 1].next_ = std::numeric_limits<Idx>::max();
        }

        inline constexpr ~jump_span()
        {
            if constexpr (std::is_compound_v<T> && !std::is_pointer_v<T>)
            {
                std::unordered_set<Idx> empty_set = {};
                Idx next = next_;
                while (next != std::numeric_limits<Idx>::max())
                {
                    empty_set.insert(next);
                    next = span_[next].next_;
                }

                if constexpr (!std::is_trivially_destructible_v<T>)
                {
                    for (Idx i = 0; i < span_.size(); i++)
                    {
                        if (!empty_set.contains(i))
                        {
                            std::destroy_at(&span_[i].elm_);
                        }
                    }
                }
            }
        }
    };

    inline constexpr void //
    replacing_erase(auto& container, size_t idx)
    {
        std::swap(container[idx], container.back());
        container.pop_back();
    }
}; // namespace XTD_EXT_HPP_NAMESPACE

#endif // XTD_DS_HXX

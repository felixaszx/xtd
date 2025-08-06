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
        Idx next_ = 0;
        Idx size_ = 0;
        C c_ = nullptr;

      public:
        inline constexpr bool //
        occupied [[nodiscard]] (Idx idx) const
        {
            if (idx >= c_.size())
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
                next = c_[next].next_;
            }
            return true;
        }

        template <typename S>
        inline constexpr auto& // enable check will have high overhead
        at(this S&& self, Idx idx)
        {
            if constexpr (checking)
            {
                if (!self.occupied(idx))
                {
                    throw std::invalid_argument("This index is refereing to an unoccupied position");
                }
            }
            return *(T*)(std::forward<S>(self).c_[idx].elm_);
        }

        template <typename S>
        inline constexpr auto& //
        operator[](this S&& self, Idx idx) noexcept
        {
            return *(T*)(std::forward<S>(self).c_[idx].elm_);
        }

        inline constexpr pointer //
        data() const noexcept
        {
            return reinterpret_cast<pointer>(c_.data());
        }

        inline constexpr size_type //
        size() const noexcept
        {
            return size_;
        }

        inline constexpr size_type //
        capacity() const noexcept
        {
            return c_.size();
        }

        inline constexpr size_type //
        remain_slots() const noexcept
        {
            size_type remaining = 0;
            Idx next = next_;
            while (next != std::numeric_limits<Idx>::max())
            {
                remaining++;
                next = c_[next].next_;
            }

            return remaining;
        }

        inline constexpr void //
        clear()
        {
            if constexpr (!std::is_trivially_destructible_v<T>)
            {
                std::unordered_set<Idx> empty_set = {};
                Idx next = next_;
                while (next != std::numeric_limits<Idx>::max())
                {
                    empty_set.insert(next);
                    next = c_[next].next_;
                }

                for (Idx i = 0; i < c_.size(); i++)
                {
                    if (!empty_set.contains(i))
                    {
                        std::destroy_at(reinterpret_cast<T*>(c_[i].elm_));
                    }
                }
            }

            size_ = 0;
            next_ = std::numeric_limits<Idx>::max();
            c_.clear();
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

            if constexpr (!std::is_trivially_destructible_v<T>)
            {
                std::destroy_at(reinterpret_cast<T*>(c_[idx].elm_));
            }

            c_[idx].next_ = next_;
            next_ = idx;
            size_--;
            return true;
        }

        inline constexpr void //
        expand()
        {
            expand_to(c_.size() == 0 ? 1 : c_.size() * 2);
        }

        inline constexpr void //
        expand_to(Idx size)
        {
            if (size <= c_.size())
            {
                return;
            }

            Idx old_size = c_.size();
            c_.resize(size);

            for (Idx i = old_size; i < c_.size(); i++)
            {
                c_[i].next_ = i + 1;
            }
            c_[c_.size() - 1].next_ = std::numeric_limits<Idx>::max();

            Idx next = next_;
            while (next != std::numeric_limits<Idx>::max() && c_[next].next_ != std::numeric_limits<Idx>::max())
            {
                next = c_[next].next_;
            }

            if (next == std::numeric_limits<Idx>::max())
            {
                next_ = old_size;
                return;
            }
            c_[next].next_ = old_size;
        }

        template <class... Args>
        inline constexpr Idx //
        emplace [[nodiscard]] (Args&&... args)
        {
            if (next_ == std::numeric_limits<Idx>::max())
            {
                expand();
            }

            Idx curr = next_;
            next_ = c_[curr].next_;
            std::construct_at(reinterpret_cast<T*>(c_[curr].elm_), std::forward<Args>(args)...);

            size_++;
            return curr;
        }

        inline constexpr jump_array(Idx elm_size = 4)
            : next_(0),
              c_(elm_size)
        {
            for (Idx i = 0; i < elm_size; i++)
            {
                c_[i].next_ = i + 1;
            }

            if (elm_size > 0)
            {
                c_[elm_size - 1].next_ = std::numeric_limits<Idx>::max();
            }
            else
            {
                next_ = std::numeric_limits<Idx>::max();
            }
        }

        inline constexpr ~jump_array() { clear(); }
    };

    inline constexpr void //
    replacing_erase(auto& container, size_t idx)
    {
        std::byte tmp[sizeof(container[0])];
        memcpy(tmp, &container[idx], sizeof(tmp));
        memcpy(&container[idx], &container.back(), sizeof(tmp));
        memcpy(&container.back(), tmp, sizeof(tmp));

        container.pop_back();
    }
}; // namespace XTD_EXT_HPP_NAMESPACE

#endif // XTD_DS_HXX

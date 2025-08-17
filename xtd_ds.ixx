
template <typename T, typename Idx, typename C, bool checking>
    requires std::unsigned_integral<Idx> && (sizeof(T) >= sizeof(Idx))
inline constexpr jump_array<T, Idx, C, checking>::~jump_array()
{
    clear();
}

template <typename T, typename Idx, typename C, bool checking>
    requires std::unsigned_integral<Idx> && (sizeof(T) >= sizeof(Idx))
template <typename... CArgs>
inline constexpr jump_array<T, Idx, C, checking>::jump_array(CArgs... container_args)
    : next_(0),
      c_(std::forward<CArgs>(container_args)...)
{
    c_.resize(1);
    c_[0].next_ = std::numeric_limits<Idx>::max();
    next_ = 0;
}

template <typename T, typename Idx, typename C, bool checking>
    requires std::unsigned_integral<Idx> && (sizeof(T) >= sizeof(Idx))
template <class... Args>
inline constexpr Idx //
    jump_array<T, Idx, C, checking>::emplace [[nodiscard]] (Args&&... args)
{
    if (next_ == std::numeric_limits<Idx>::max())
    {
        expand();
    }

    Idx curr = next_;
    next_ = c_[curr].next_;
    std::construct_at(reinterpret_cast<T*>(c_[curr].elm_), std::forward<Args>(args)...);

    if (curr + 1 > effective_size())
    {
        last_ = curr;
    }

    size_++;
    return curr;
}

template <typename T, typename Idx, typename C, bool checking>
    requires std::unsigned_integral<Idx> && (sizeof(T) >= sizeof(Idx))
inline constexpr void //
    jump_array<T, Idx, C, checking>::expand_to(Idx size)
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

template <typename T, typename Idx, typename C, bool checking>
    requires std::unsigned_integral<Idx> && (sizeof(T) >= sizeof(Idx))
inline constexpr void //
    jump_array<T, Idx, C, checking>::expand()
{
    expand_to(c_.size() == 0 ? 1 : c_.size() * 2);
}

template <typename T, typename Idx, typename C, bool checking>
    requires std::unsigned_integral<Idx> && (sizeof(T) >= sizeof(Idx))
inline constexpr bool //
    jump_array<T, Idx, C, checking>::erase(Idx idx)
{
    if (idx == std::numeric_limits<Idx>::max())
    {
        return false;
    }

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

    if (idx == last_)
    {
        last_--;
    }

    c_[idx].next_ = next_;
    next_ = idx;
    size_--;
    return true;
}

template <typename T, typename Idx, typename C, bool checking>
    requires std::unsigned_integral<Idx> && (sizeof(T) >= sizeof(Idx))
inline constexpr void //
    jump_array<T, Idx, C, checking>::clear()
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
    last_ = std::numeric_limits<Idx>::max();
    next_ = std::numeric_limits<Idx>::max();
    c_.clear();
}

template <typename T, typename Idx, typename C, bool checking>
    requires std::unsigned_integral<Idx> && (sizeof(T) >= sizeof(Idx))
inline constexpr typename jump_array<T, Idx, C, checking>::size_type //
    jump_array<T, Idx, C, checking>::capacity() const noexcept
{
    return c_.size();
}

template <typename T, typename Idx, typename C, bool checking>
    requires std::unsigned_integral<Idx> && (sizeof(T) >= sizeof(Idx))
inline constexpr typename jump_array<T, Idx, C, checking>::size_type //
    jump_array<T, Idx, C, checking>::effective_size() const noexcept
{
    return last_ + 1;
}

template <typename T, typename Idx, typename C, bool checking>
    requires std::unsigned_integral<Idx> && (sizeof(T) >= sizeof(Idx))
inline constexpr typename jump_array<T, Idx, C, checking>::size_type //
    jump_array<T, Idx, C, checking>::size() const noexcept
{
    return size_;
}

template <typename T, typename Idx, typename C, bool checking>
    requires std::unsigned_integral<Idx> && (sizeof(T) >= sizeof(Idx))
template <typename S>
inline constexpr auto //
    jump_array<T, Idx, C, checking>::data(this S&& self) noexcept
{
    return *(T*)(std::forward<S>(self).c_.data());
}

template <typename T, typename Idx, typename C, bool checking>
    requires std::unsigned_integral<Idx> && (sizeof(T) >= sizeof(Idx))
template <typename S>
inline constexpr auto& //
    jump_array<T, Idx, C, checking>::
    operator[](this S&& self, Idx idx) noexcept
{
    return *(T*)(std::forward<S>(self).c_[idx].elm_);
}

template <typename T, typename Idx, typename C, bool checking>
    requires std::unsigned_integral<Idx> && (sizeof(T) >= sizeof(Idx))
template <typename S>
inline constexpr auto& // enable check will have high overhead
    jump_array<T, Idx, C, checking>::at(this S&& self, Idx idx)
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

template <typename T, typename Idx, typename C, bool checking>
    requires std::unsigned_integral<Idx> && (sizeof(T) >= sizeof(Idx))
inline constexpr bool //
    jump_array<T, Idx, C, checking>::contains [[nodiscard]] (Idx idx) const
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

inline constexpr void //
replacing_erase(auto& container, size_t idx)
{
    std::byte tmp[sizeof(container[0])];
    memcpy(tmp, &container[idx], sizeof(tmp));
    memcpy(&container[idx], &container.back(), sizeof(tmp));
    memcpy(&container.back(), tmp, sizeof(tmp));

    container.pop_back();
}
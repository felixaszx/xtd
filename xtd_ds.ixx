
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
replacing_erase(auto& container, std::size_t idx)
{
    std::byte tmp[sizeof(container[0])];
    memcpy(tmp, &container[idx], sizeof(tmp));
    memcpy(&container[idx], &container.back(), sizeof(tmp));
    memcpy(&container.back(), tmp, sizeof(tmp));

    container.pop_back();
}

template <typename T, typename Idx>
    requires std::is_default_constructible_v<T> && //
             std::unsigned_integral<Idx> &&
             (sizeof(T) >= sizeof(Idx))
             inline constexpr Idx // node 0 will always be root, return the index of new root
             tree_array<T, Idx>::take(Idx to, tree_array& tree, Idx from)
                 requires std::is_move_constructible_v<T>
{
    std::flat_map<Idx, Idx> mapping;

    auto func = [&](Idx node, tree_array<T>&) //
    {
        for (auto& child : tree.get_children(node))
        {
            Idx cp = mapping[tree.get_parent(child)];
            Idx cn = tree_array<T, Idx>::emplace(std::move(tree.get(child).data_));
            mapping.emplace(child, cn);
            this->add_child(cp, cn);
        }
    };

    Idx new_from = tree_array<T, Idx>::emplace(std::move(tree.get(from).data_));
    mapping.emplace(from, new_from);
    tree.traverse(from, func);
    this->add_child(to, mapping[from]);

    tree.cut(from);
    return new_from;
}

template <typename T, typename Idx>
    requires std::is_default_constructible_v<T> && //
             std::unsigned_integral<Idx> &&
             (sizeof(T) >= sizeof(Idx))
             inline constexpr Idx // very expensive!, node 0 will always be root, return the index of new root
             tree_array<T, Idx>::insert(Idx to, const tree_array& tree, Idx from)
                 requires std::is_copy_constructible_v<T>
{
    std::flat_map<Idx, Idx> mapping;

    auto func = [&](Idx node, const tree_array<T>&) //
    {
        for (auto& child : tree.get_children(node))
        {
            Idx cp = mapping[tree.get_parent(child)];
            Idx cn = tree_array<T, Idx>::emplace(tree.get(child).data_);
            mapping.emplace(child, cn);
            this->add_child(cp, cn);
        }
    };

    Idx new_from = tree_array<T, Idx>::emplace(tree.get(from).data_);
    mapping.emplace(from, new_from);
    tree.traverse(from, func);
    this->add_child(to, mapping[from]);
    return new_from;
}

template <typename T, typename Idx>
    requires std::is_default_constructible_v<T> && //
             std::unsigned_integral<Idx> && (sizeof(T) >= sizeof(Idx))
inline constexpr void // very expensive!
    tree_array<T, Idx>::cut(Idx at)
{
    auto func = [this](Idx node, const tree_array<T>&) //
    {
        this->reset_children(node);
        this->nodes_.erase(node);
    };

    for (auto child : this->get_children(at))
    {
        this->traverse(child, func);
    }

    // all children are destroyed now
    children_.erase(get(at).children_);
    this->reset_parent(at);
    this->nodes_.erase(at);
}

template <typename T, typename Idx>
    requires std::is_default_constructible_v<T> && //
             std::unsigned_integral<Idx> && (sizeof(T) >= sizeof(Idx))
inline constexpr void //
    tree_array<T, Idx>::clear()
{
    nodes_.clear();
    children_.clear();
}

template <typename T, typename Idx>
    requires std::is_default_constructible_v<T> && std::unsigned_integral<Idx> &&
             (sizeof(T) >= sizeof(Idx))
             template <typename S, typename F>
                 requires std::invocable<F, Idx, tree_array<T, Idx>&>
inline constexpr void //
    tree_array<T, Idx>::traverse(this S&& self, Idx at, F&& callback)
{
    std::deque<Idx> queue;
    queue.push_back(at);

    while (!queue.empty())
    {
        Idx curr = queue.front();

        if (std::forward<S>(self).has_children(curr))
        {
            auto& children = std::forward<S>(self).get_children(curr);
            queue.insert(queue.end(), children.begin(), children.end());
        }

        callback(curr, std::forward<S>(self));
        queue.pop_front();
    }
}

template <typename T, typename Idx>
    requires std::is_default_constructible_v<T> && //
             std::unsigned_integral<Idx> && (sizeof(T) >= sizeof(Idx))
inline constexpr std::vector<Idx> //
    tree_array<T, Idx>::sort [[nodiscard]] (Idx at) const
{
    std::vector<Idx> idx;
    idx.reserve(size());
    auto func = [&idx](Idx node, const tree_array<T>&) { idx.push_back(node); };
    traverse(at, func);
    return idx;
}

template <typename T, typename Idx>
    requires std::is_default_constructible_v<T> && //
             std::unsigned_integral<Idx> && (sizeof(T) >= sizeof(Idx))
inline constexpr Idx //
    tree_array<T, Idx>::expand_to(Idx to)
{
    nodes_.expand_to(to);
}

template <typename T, typename Idx>
    requires std::is_default_constructible_v<T> && //
             std::unsigned_integral<Idx> && (sizeof(T) >= sizeof(Idx))
inline constexpr Idx // linear search, return index in children array
    tree_array<T, Idx>::find_child(Idx at, Idx child) const
{
    auto& children = get_children(at);
    auto result = std::ranges::find(children, child);

    if (result != children.end())
    {
        return std::distance(children.begin(), result);
    }
    return std::numeric_limits<Idx>::max();
}

template <typename T, typename Idx>
    requires std::is_default_constructible_v<T> && //
             std::unsigned_integral<Idx> && (sizeof(T) >= sizeof(Idx))
inline constexpr void //
    tree_array<T, Idx>::reset_parent(Idx node)
{
    if (!has_parent(node))
    {
        return;
    }

    auto& curr = get(node);
    auto& parent = get(curr.parent_);
    auto& children = children_[parent.children_];
    auto result = std::distance(children.begin(), std::ranges::find(children, node));
    replacing_erase(children, result);

    if (children.empty())
    {
        children_.erase(parent.children_);
        parent.children_ = std::numeric_limits<Idx>::max();
    }

    clear_parent(node);
}

template <typename T, typename Idx>
    requires std::is_default_constructible_v<T> && //
             std::unsigned_integral<Idx> && (sizeof(T) >= sizeof(Idx))
inline constexpr void //
    tree_array<T, Idx>::add_child(Idx at, Idx child)
{
    if (get_parent(child) == at || at == child)
    {
        return;
    }

    auto& curr = get(at);
    if (get_children(at).empty())
    {
        curr.children_ = children_.emplace(mem_res_);
    }

    children_[curr.children_].push_back(child);

    reset_parent(child);
    get(child).parent_ = at;
}

template <typename T, typename Idx>
    requires std::is_default_constructible_v<T> && //
             std::unsigned_integral<Idx> && (sizeof(T) >= sizeof(Idx))
inline constexpr const std::pmr::deque<Idx>& //
    tree_array<T, Idx>::get_children(Idx node) const
{
    static const std::pmr::deque<Idx> empty(0);
    if (!has_children(node))
    {
        return empty;
    }
    return children_[get(node).children_];
}

template <typename T, typename Idx>
    requires std::is_default_constructible_v<T> && //
             std::unsigned_integral<Idx> && (sizeof(T) >= sizeof(Idx))
inline constexpr bool //
    tree_array<T, Idx>::has_children(Idx node) const
{
    return get(node).children_ != std::numeric_limits<Idx>::max();
}

template <typename T, typename Idx>
    requires std::is_default_constructible_v<T> && //
             std::unsigned_integral<Idx> && (sizeof(T) >= sizeof(Idx))
inline constexpr Idx //
    tree_array<T, Idx>::get_parent(Idx node) const
{
    return get(node).parent_;
}

template <typename T, typename Idx>
    requires std::is_default_constructible_v<T> && //
             std::unsigned_integral<Idx> && (sizeof(T) >= sizeof(Idx))
inline constexpr Idx //
    tree_array<T, Idx>::has_parent(Idx node) const
{
    return get(node).parent_ != std::numeric_limits<Idx>::max();
}

template <typename T, typename Idx>
    requires std::is_default_constructible_v<T> && //
             std::unsigned_integral<Idx> && (sizeof(T) >= sizeof(Idx))
inline constexpr void //
    tree_array<T, Idx>::erase(Idx node)
{
    auto& target = get(node);
    Idx parent = get_parent(node);

    if (!get_children(node).empty())
    {
        for (auto& child : children_[target.children_])
        {
            get(child).parent_ = parent;
        }
    }

    if (has_parent(node))
    {
        reset_parent(node);

        if (has_children(node))
        {
            auto& parent_node = get(parent);
            if (!has_children(parent))
            {
                parent_node.children_ = children_.emplace(mem_res_);
            }

            auto& p_children = children_[parent_node.children_];
            auto& t_children = children_[target.children_];
            p_children.insert(p_children.end(), t_children.begin(), t_children.end());
            children_.erase(target.children_);
        }
    }

    nodes_.erase(node);
}

template <typename T, typename Idx>
    requires std::is_default_constructible_v<T> && //
             std::unsigned_integral<Idx> && (sizeof(T) >= sizeof(Idx))
template <typename S>
inline constexpr auto& // mostly used internally
    tree_array<T, Idx>::get(this S&& self, Idx node)
{
    return std::forward<S>(self).nodes_[node];
}

template <typename T, typename Idx>
    requires std::is_default_constructible_v<T> && //
             std::unsigned_integral<Idx> && (sizeof(T) >= sizeof(Idx))
inline constexpr Idx //
    tree_array<T, Idx>::size() const
{
    return nodes_.size();
}

template <typename T, typename Idx>
    requires std::is_default_constructible_v<T> && //
             std::unsigned_integral<Idx> && (sizeof(T) >= sizeof(Idx))
inline constexpr bool // very expensive when node is not std::numeric_limits<Idx>::max()!
    tree_array<T, Idx>::containes(Idx node) const
{
    return nodes_.contains(node);
}

template <typename T, typename Idx>
    requires std::is_default_constructible_v<T> && //
             std::unsigned_integral<Idx> && (sizeof(T) >= sizeof(Idx))
template <typename... Args>
inline constexpr Idx // return node index
    tree_array<T, Idx>::emplace [[nodiscard]] (Args&&... args)
{
    return nodes_.emplace(std::forward<Args>(args)...);
}

template <typename T, typename Idx>
    requires std::is_default_constructible_v<T> && //
                 std::unsigned_integral<Idx> && (sizeof(T) >= sizeof(Idx))
inline constexpr //
    tree_array<T, Idx>::tree_array(std::pmr::memory_resource* mem_res)
    : nodes_(4, mem_res),
      mem_res_(mem_res)
{
}

template <typename T, typename Idx>
    requires std::is_default_constructible_v<T> && //
             std::unsigned_integral<Idx> && (sizeof(T) >= sizeof(Idx))
inline constexpr void //
    tree_array<T, Idx>::reset_children(Idx node)
{
    if (has_children(node))
    {
        for (auto& child : children_[get(node).children_])
        {
            clear_parent(child);
        }
        children_.erase(get(node).children_);
    }
}

template <typename T, typename Idx>
    requires std::is_default_constructible_v<T> && //
             std::unsigned_integral<Idx> && (sizeof(T) >= sizeof(Idx))
inline constexpr void //
    tree_array<T, Idx>::clear_parent(Idx node)
{
    get(node).parent_ = std::numeric_limits<Idx>::max();
}
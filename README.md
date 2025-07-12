# xtd
Some smaller hellper class and defines for mordern c++20+

## `xtd::jump_span`
`xtd::jump_span` is an `std::span`-like adaptor that focuses on very specific use cases:
- Only works on raw memory
- No iteration, but random access
- Need quick insertion in an array-like data structure
- Order does not matter and search is not needed
- Near zero memory overhead
- Frequent removal and insertion at random position
- The position of insertion does not matter
- Fixed array size (a.k.a. no dynamic memory allocation)

### Usage
`xtd::jump_span` only provides 2 APIs for storage manupilation, `emplace(...)` and `erase(Idx idx)`. 

#### `bool emplace(...)` 
Same as all other stl containers, except it will return a number indicating where the newly constructed element is plced. You **MUST** keep this number valid otherwise you will never be able to remove this element until the lifetime of `xtd::jump_span` is over.
##### Returns `bool`
- `false`: the array is full
- `true`: operation successfull


#### `bool erase(Idx idx)`
You **MUST** use the number obtained form `emplace(...)` as the index otherwise the behaviour is undefined and can potentially be catastrophic. You may set `checking = true` in the template argument to prevent unexpected destructor calls on invalid elements. ##### Returns `bool`
##### Returns `bool`
- `false`: the index is invalid (only when `checking = true`)
- `true`: operation successfull (always `true` when `checking = false`)


#### `reference operator[](Idx idx)` and `reference at(Idx idx)`
Same as all other stl containers, except `operator[]` never throw exceptions when accessing invalid elements and `at(Idx idx)` will when checking is enabled.
##### Exception `std::invalid_argument`

### Implementation
```c++
template <typename T, typename Idx = std::size_t, bool checking = true>
        requires std::unsigned_integral<Idx> && //
                 (sizeof(T) >= sizeof(Idx))
    class jump_span
{
    
    // ...
    struct jump_span_elm
    {
        Idx next_ = std::numeric_limits<Idx>::max();
        T elm_;
    };
    // ...
    Idx next_ = 0;
    std::span<jump_span_elm> span_ = nullptr;
    // ...
};
```
`next_` will always store the subscript of an unoccupied slot. Each unoccupied slot will always store the subcript of the next one in an union with `T`. Therefore a link list of unoccupied slots is maintained intrusively. When `next_` is the maximum number possible of `Idx`, the array is considered fully occupied. Hence `size()` function of `jump_span` should be interpreted as the one in `std::array` rather than `std::vector`.

`eamplce(...)` will always use the head of the link list as the target. When `erase(Idx idx)` is called, `next_` will be `idx`. This is to garuantee fast insertion and removal. Cache locality might be a problem in this case because all elements store in it is never continues. 

### Example usage cases
- Lookup table
- Some kind of caching mechanism

### Issues
- Performance of the destructor can be bad when there are lots of empty slots
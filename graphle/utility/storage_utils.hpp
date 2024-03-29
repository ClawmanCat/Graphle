#pragma once

#include <common.hpp>

#include <utility>


namespace graphle::util {
    /** Removes and returns the first item of the provided container. */
    template <typename C> requires requires (C c) { c.front(), c.pop_front(); }
    constexpr inline rng::range_value_t<C> take_front(C& container) {
        auto value = std::move(container.front());
        container.pop_front();

        return value;
    }


    /** Removes and returns the last item of the provided container. */
    template <typename C> requires requires (C c) { c.back(), c.pop_back(); }
    constexpr inline rng::range_value_t<C> take_back(C& container) {
        auto value = std::move(container.back());
        container.pop_back();

        return value;
    }


    /** Returns a view of the given range with no elements, i.e. the subrange (range.end(), range.end()). */
    template <typename R> constexpr inline auto empty_range_of(R&& range) {
        return rng::subrange(range.end(), range.end());
    }
}
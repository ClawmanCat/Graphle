#pragma once

#include <common.hpp>


namespace graphle::meta {
    /**
     * Wrapper around a constexpr-if statement to allow easily assigning different types to the same variable. E.g.:
     * ~~~
     * // decltype(value) is float if UseFloatingPoint or int otherwise.
     * auto value = if_constexpr<UseFloatingPoint>(1.0f, 1);
     * ~~~
     */
    template <bool Condition, typename T, typename F>
    constexpr inline decltype(auto) if_constexpr(T&& if_true, F&& if_false) {
        if constexpr (Condition) return GRAPHLE_FWD(if_true);
        else return GRAPHLE_FWD(if_false);
    }
}
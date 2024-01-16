#pragma once

#include <common.hpp>

#include <type_traits>


namespace graphle::meta {
    /**
     * Type trait that is false for any provided type T.
     * Prevents early evaluation in static_assert, allowing the equivalent of static_assert(false) in a constexpr-if branch.
     */
    template <typename T> class always_false {
    private:
        struct hidden_type {};
    public:
        constexpr static inline bool value = std::is_same_v<T, hidden_type>;
    };


    /** @copydoc always_false */
    template <typename T> constexpr inline bool always_false_v = always_false<T>::value;
}
#pragma once

#include <common.hpp>


/**
 * @def GRAPHLE_MAKE_NIEBLOID(Name, Function, ...)
 * Constructs a Niebloid function object to wrap the provided free function.
 * VA_ARGS may be used to pass template parameters for the function object (<b>NOT</b> its call operator!), e.g.:
 * ~~~
 * // Existing free function
 * template <typename T> auto my_transform(auto&&... args) { ... }
 *
 * // Niebloid definition
 * GRAPHLE_MAKE_NIEBLOID(some_transform, my_transform<T>, typename T);
 *
 * // Usage
 * using alias = some_transform_t<int>;
 * auto result = some_transform<int>(1, 2, 3, 4);
 * ~~~
 * @note: NTTPs are currently not supported.
 * @note: If you get weird errors here using MSVC, make sure you compile with /Zc:preprocessor (The legacy preprocessor does not support VA_OPT).
 */
#define GRAPHLE_MAKE_NIEBLOID(Name, Function, ...)              \
__VA_OPT__(template <) __VA_ARGS__ __VA_OPT__(>)                \
struct Name##_t {                                               \
    constexpr decltype(auto) operator()(auto&&... args) const { \
        return Function(GRAPHLE_FWD(args)...);                  \
    }                                                           \
};                                                              \
                                                                \
__VA_OPT__(template <typename... Args>)                         \
constexpr inline Name##_t __VA_OPT__(<Args...>) Name = {}


// Doxygen seems to struggle with VA_OPT so provide a simplified version of the above macro when running it.
#ifdef GRAPHLE_DOXYGEN
    /**
     * @def GRAPHLE_MAKE_NIEBLOID_IMPL(Name, Function, ...)
     * Simplified version of GRAPHLE_MAKE_NIEBLOID for Doxygen.
     */
    #define GRAPHLE_MAKE_NIEBLOID_IMPL(Name, Function, ...)                             \
    struct Name##_t { constexpr static decltype(auto) operator()(auto&&) const {} };    \
    constexpr inline Name##_t Name = {};
#else
    #define GRAPHLE_MAKE_NIEBLOID_IMPL(Name, Function, ...) GRAPHLE_MAKE_NIEBLOID(Name, Function, __VA_ARGS__)
#endif


namespace graphle::util {
    /**
     * @ingroup Utils
     * A function object that does nothing.
     */
    GRAPHLE_MAKE_NIEBLOID_IMPL(no_op, [] (auto&&...) {});

    /**
     * @ingroup Utils
     * A function object that returns the address of an object.
     */
    GRAPHLE_MAKE_NIEBLOID_IMPL(addressof, [](auto&& obj) { return std::addressof(obj); });

    /**
     * @ingroup Utils
     * A function object that dereferences a pointer-like object.
     */
    GRAPHLE_MAKE_NIEBLOID_IMPL(indirect, [] (auto&& pointer) -> decltype(auto) { return *pointer; });

    /**
     * @ingroup Utils
     * A function object that makes its argument const.
     */
    GRAPHLE_MAKE_NIEBLOID_IMPL(as_const, std::as_const);

    /**
     * @ingroup Utils
     * A function object that makes the object pointed to by its argument (raw pointer) const
     * (as opposed to std::add_const which adds constness to the pointer).
     */
    GRAPHLE_MAKE_NIEBLOID_IMPL(pointed_to_as_const, [] <typename T> (T* pointer) -> const T* { return pointer; });


    /**
     * @ingroup Utils
     * A function object that transposes an edge [A, B] to an edge [B, A].
     */
    GRAPHLE_MAKE_NIEBLOID_IMPL(transpose_edge, [] (auto edge) { std::swap(edge.first, edge.second); return edge; });


    /**
     * @ingroup Utils
     * Returns a function object that applies F to both vertices of an edge.
     * @todo: This would probably work better as a range view.
     */
    template <typename F> constexpr inline auto transform_edge(F&& fn) {
        // Edge is just a pair of pointers, no need to forward.
        return [f = GRAPHLE_FWD(fn)] (auto&& edge) {
            return std::pair { f(GRAPHLE_FWD(edge.first)), f(GRAPHLE_FWD(edge.second)) };
        };
    }
}
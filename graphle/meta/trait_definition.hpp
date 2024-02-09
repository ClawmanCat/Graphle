#pragma once

#include <common.hpp>


/**
 * @def GRAPHLE_EVAL_IF_VALID(Default, ...)
 * Evaluates and returns the expression __VA_ARGS__ if it is valid, or returns 'Default' otherwise.
 */
#define GRAPHLE_EVAL_IF_VALID(Default, ...)                                                         \
[] {                                                                                                \
    if constexpr (requires { __VA_ARGS__; }) return __VA_ARGS__;                                    \
    else return Default;                                                                            \
} ()


/**
 * @def GRAPHLE_BOOL_TRAIT_DEF(TraitName, ConceptName, Args, ...)
 * Defines a boolean-valued type trait, its associated templated global variable and the associated concept.
 * If the expression that evaluates the trait is not valid (as checked through a requires-expression) the trait will be false.
 *
 * Example:
 * ~~~
 * // Defines the following using the provided implementation:
 * // template <typename T> struct is_boolean { ... };
 * // template <typename T> constexpr inline bool is_boolean_v = ...;
 * // template <typename T> concept boolean_type = ...;
 * GRAPHLE_BOOL_TRAIT_DEF(is_boolean, boolean_type, typename T, std::is_same_v<T, bool>);
 * ~~~
 *
 * @param TraitName The name of the resulting trait. The associated variable will have the name TraitName_v.
 * @param ConceptName The name of the resulting concept.
 * @param Args The argument-sequence for the type trait, including typename/type specifiers.
 * @param __VA_ARGS__ A boolean expression for the given trait. The provided arguments from 'Args' may be used within this expression.
 */
#define GRAPHLE_BOOL_TRAIT_DEF(TraitName, ConceptName, Args, ...)                                   \
template <Args> struct TraitName {                                                                  \
    constexpr static inline bool value = GRAPHLE_EVAL_IF_VALID(false, __VA_ARGS__);                 \
};                                                                                                  \
                                                                                                    \
/** See associated type-trait. */                                                                   \
template <Args> constexpr inline bool TraitName##_v = GRAPHLE_EVAL_IF_VALID(false, __VA_ARGS__);    \
/** See associated type-trait. */                                                                   \
template <Args> concept ConceptName = __VA_ARGS__


/**
 * @def GRAPHLE_TYPE_TRAIT_DEF(TraitName, Args, ...)
 * Defines a type-valued type trait and its associated templated typedef.
 * Example:
 * ~~~
 * // Defines the following using the provided implementation:
 * // template <typename T> struct value_type { ... };
 * // template <typename T> using value_type_t = ...;
 * GRAPHLE_TYPE_TRAIT_DEF(value_type, typename T, typename T::value_type)
 * ~~~
 * @param TraitName The name of the resulting trait. The associated typedef will have the name TraitName_t.
 * @param Args The argument-sequence for the type trait, including typename/type specifiers.
 * @param __VA_ARGS__ A resulting type expression for the given trait. The provided arguments from 'Args' may be used within this expression.
 */
#define GRAPHLE_TYPE_TRAIT_DEF(TraitName, Args, ...)                                                \
template <Args> struct TraitName {                                                                  \
    using type = __VA_ARGS__;                                                                       \
};                                                                                                  \
                                                                                                    \
/** See associated type-trait. */                                                                   \
template <Args> using TraitName##_t = __VA_ARGS__
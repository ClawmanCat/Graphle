#pragma once

#include <common.hpp>

#include <type_traits>


namespace graphle::meta {
    /** A wrapper around a type. */
    template <typename T> using type  = std::type_identity<T>;
    /** A wrapper around a NTTP. */
    template <auto V>     using value = std::integral_constant<decltype(V), V>;


    /** Concept for a type that wraps some other type. */
    template <typename T> concept type_wrapper  = requires { typename T::type; };
    /** Concept for a type that wraps a value. */
    template <typename T> concept value_wrapper = requires { T::value; };
    /** Concept for a type that wraps a value of type T. */
    template <typename T, typename V> concept value_wrapper_of = std::is_same_v<std::remove_cvref_t<decltype(T::value)>, V>;


    /** Indicates the absence of a type. */
    struct none {};
    /** Indicates a type that will be replaced with another type later. */
    struct placeholder {};


    /** Used for type deduction through member initializers. */
    template <typename T> using deduce_as_t = type<T>;
    /** @copydoc deduce_as_t */
    template <typename T> constexpr inline deduce_as_t<T> deduce_as = {};

    /** Equivalent to deduce_as_t but always deduces as an lvalue. */
    template <typename T> using deduce_as_lvalue_t = deduce_as_t<std::remove_reference_t<T>>;
    /** @copydoc deduce_as_lvalue_t */
    template <typename T> constexpr inline deduce_as_lvalue_t<T> deduce_as_lvalue = {};


    /** Checks if a type is meta::none */
    template <typename T> struct is_none : std::bool_constant<std::is_same_v<T, none>> { };
    /** @copydoc is_none */
    template <typename T> constexpr inline bool is_none_v = is_none<T>::value;

    /** Checks if a type is meta::placeholder */
    template <typename T> struct is_placeholder : std::bool_constant<std::is_same_v<T, placeholder>> { };
    /** @copydoc is_placeholder */
    template <typename T> constexpr inline bool is_placeholder_v = is_placeholder<T>::value;
}
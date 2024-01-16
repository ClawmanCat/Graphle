#pragma once

#include <common.hpp>


namespace graphle::meta {
    /** Adds const to the pointed-to type of a pointer. */
    template <typename T> struct pointed_to_as_const {};
    /** @copydoc pointed_to_as_const */
    template <typename T> struct pointed_to_as_const<T*> { using type = const T*; };

    /** @copydoc pointed_to_as_const */
    template <typename T> using pointed_to_as_const_t = typename pointed_to_as_const<T>::type;


    /** Checks if two pointers have the same constness of their pointed-to type. */
    template <typename A, typename B> struct pointer_constness_match {};
    /** @copydoc pointer_constness_match */
    template <typename A, typename B> struct pointer_constness_match<A*, B*> : std::true_type {};
    /** @copydoc pointer_constness_match */
    template <typename A, typename B> struct pointer_constness_match<const A*, const B*> : std::true_type {};
    /** @copydoc pointer_constness_match */
    template <typename A, typename B> struct pointer_constness_match<const A*, B*> : std::false_type {};
    /** @copydoc pointer_constness_match */
    template <typename A, typename B> struct pointer_constness_match<A*, const B*> : std::false_type {};

    /** @copydoc pointer_constness_match */
    template <typename A, typename B> constexpr inline bool pointer_constness_match_v = pointer_constness_match<A, B>::value;
}
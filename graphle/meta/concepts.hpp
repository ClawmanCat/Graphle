#pragma once

#include <common.hpp>


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




    /**
     * Type trait to check if the type T is an instantiation of the template Tmpl.
     * This trait also returns true for classes deriving from a template instantiation and differing cv-qualifiers.
     */
    template <template <typename...> typename Tmpl, typename T> class is_template {
    private:
        template <typename... Xs> constexpr static std::true_type  test(Tmpl<Xs...>*);
        template <typename... Xs> constexpr static std::false_type test(...);
    public:
        constexpr static inline bool value = decltype(test(std::declval<T*>()))::value;
    };


    /** @copydoc is_template */
    template <template <typename...> typename Tmpl, typename T>
    constexpr static inline bool is_template_v = is_template<Tmpl, T>::value;


    /** Concept for @ref is_template */
    template <typename T, template <typename...> typename Tmpl>
    concept of_template = is_template<Tmpl, T>::value;




    /** Concept for std::is_invocable_r */
    template <typename F, typename R, typename... Args> concept invocable_r = std::is_invocable_r_v<F, R, Args...>;
}
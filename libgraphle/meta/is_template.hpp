#pragma once

#include <common.hpp>

#include <type_traits>


namespace graphle::meta {
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
}
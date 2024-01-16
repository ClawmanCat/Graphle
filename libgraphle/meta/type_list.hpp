#pragma once

#include <common.hpp>
#include <meta/is_template.hpp>


namespace graphle::meta {
    /** A list of types. */
    template <typename... Ts> struct type_list {
        constexpr static inline std::size_t size = sizeof...(Ts);
        constexpr static inline bool empty       = size == 0;


        template <template <typename... X> typename F> using map = type_list<F<Ts>...>;
        template <template <typename... X> typename F> using to  = type_list<F<Ts...>>;


        /** Invokes fn's call operator with the types in the type list as template arguments. */
        template <typename F> constexpr static decltype(auto) apply(F&& fn) {
            return fn.template operator()<Ts...>();
        }


        /** Invokes fn's call operator once for every type in the type list with that type as a template argument. */
        template <typename F> constexpr static inline void foreach(F&& fn) {
            (fn.template operator()<Ts>(), ...);
        }
    };


    /** Concept for template instantiations of @ref type_list */
    template <typename L> concept list_of_types = is_template_v<type_list, L>;
}
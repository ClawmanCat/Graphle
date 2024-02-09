#pragma once

#include <common.hpp>
#include <meta/concepts.hpp>
#include <meta/value.hpp>


namespace graphle::meta {
    namespace detail {
        /** Splits the given pack into a head and a Tmpl<Tail...>. */
        template <template <typename...> typename Tmpl, typename... Ts> struct splitter {
            using head = none;
            using tail = Tmpl<>;
        };

        /** @copydoc splitter */
        template <template <typename...> typename Tmpl, typename T0, typename... Ts> struct splitter<Tmpl, T0, Ts...> {
            using head = T0;
            using tail = Tmpl<Ts...>;
        };
    }


    /** A list of types. */
    template <typename... Ts> struct type_list {
        constexpr static inline std::size_t size = sizeof...(Ts);
        constexpr static inline bool empty       = size == 0;

        /** The first element of the list, or meta::none if the list is empty. */
        using head = typename detail::splitter<type_list, Ts...>::head;
        /** A type_list of all but the first element of this type_list. */
        using tail = typename detail::splitter<type_list, Ts...>::tail;

        /**
         * Invokes F on every element of this type list, and returns the result as a new type_list. E.g.:
         * ~~~
         * // type_list<const int*, const char*, const bool*>
         * using const_pointers = type_list<int, char, bool>
         *     ::template map<std::add_const_t>
         *     ::template map<std::add_pointer_t>;
         * ~~~
         */
        template <template <typename... X> typename F> using map = type_list<F<Ts>...>;

        /**
         * Invokes F with the elements of this type list, and returns the result. E.g.:
         * ~~~
         * // std::tuple<int, char, bool>
         * using as_tuple = type_list<int, char, bool>::template to<std::tuple>;
         * ~~~
         */
        template <template <typename... X> typename F> using to = F<Ts...>;


        /** Prepends all given types to this type_list and returns the result as a new type_list. */
        template <typename... Xs> using prepend = type_list<Xs..., Ts...>;
        /** Appends all given types to this type_list and returns the result as a new type_list. */
        template <typename... Xs> using append  = type_list<Ts..., Xs...>;

        /** Prepends all types in the given type_list to this type_list and returns the result as a new type_list. */
        template <typename List> using prepend_list = typename List::template append<Ts...>;
        /** Appends all types in the given type_list to this type_list and returns the result as a new type_list. */
        template <typename List> using append_list  = typename List::template prepend<Ts...>;


        /** True if this type_list contains the type T. */
        template <typename T> constexpr static inline bool contains = (std::is_same_v<T, Ts> || ...);
        /** True if this type_list contains all types in the pack Xs. */
        template <typename... Xs> constexpr static inline bool contains_all = (contains<Xs> && ...);
        /** True if this type_list contains any of the types in the pack Xs. */
        template <typename... Xs> constexpr static inline bool contains_any = (contains<Xs> || ...);


        /** Invokes fn's call operator with the types in the type list as template arguments. */
        template <typename F> constexpr static decltype(auto) apply(F&& fn) {
            return fn.template operator()<Ts...>();
        }


        /** Invokes fn's call operator once for every type in the type list with that type as a template argument. */
        template <typename F> constexpr static inline void foreach(F&& fn) {
            (fn.template operator()<Ts>(), ...);
        }


        /** Removes N elements from the start of the list. If N is greater than the size of the list, an empty list is returned. */
        template <std::size_t N> consteval static inline auto pop_front_impl(void) {
            if constexpr (N >= size) return type_list<>{};
            else if constexpr (N == 0) return type_list<Ts...>{};
            else return tail::template pop_front_impl<(N - 1)>();
        }

        /** @copydoc pop_front_impl */
        template <std::size_t N> using pop_front = decltype(pop_front_impl<N>());


        /** Removes N elements from the end of the list. If N is greater than the size of the list, an empty list is returned. */
        template <std::size_t N> consteval static inline auto pop_back_impl(void) {
            if constexpr (N >= size) return type_list<>{};
            else {
                return typename tail
                    ::template pop_back<N>
                    ::template prepend<head> {};

            }
        }

        /** @copydoc pop_back_impl */
        template <std::size_t N> using pop_back = decltype(pop_back_impl<N>());


        /** Returns the element at index N of the type_list. */
        template <std::size_t N> requires (N < size) using at = typename pop_front<N>::head;


        /** Removes the element at index N from the type_list. */
        template <std::size_t N> requires (N < size) using erase_at = typename pop_back<(size - N)>
            ::template append_list<pop_front<(N + 1)>>;


        /** Inserts the element T at index N into the type_list. */
        template <typename T, std::size_t N> requires (N <= size) using insert_at = typename pop_back<(size - N)>
            ::template append<T>
            ::template append_list<pop_front<N>>;


        /**
         * Filters the pack to all types for which the provided type trait is true.
         * @tparam Filter A type trait to filter the pack by.
         * @return A pack of all types T in the pack for which Filter<T>::value is true.
         */
        template <template <typename...> typename Filter> consteval static inline auto filter_trait_impl(void) {
            if constexpr (empty) return type_list<>{};

            else if constexpr (Filter<head>::value) {
                return typename type_list<head>
                    ::template append_list<
                        typename tail::template filter_trait<Filter>
                    > {};
            }

            else return tail::template filter_trait_impl<Filter>();
        }


        /** @copydoc filter_trait_impl */
        template <template <typename...> typename Filter> using filter_trait = decltype(filter_trait_impl<Filter>());
    };


    /** Concept for template instantiations of @ref type_list */
    template <typename L> concept list_of_types = is_template_v<type_list, L>;
}
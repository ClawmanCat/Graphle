#pragma once

#include <common.hpp>
#include <meta/type_list.hpp>
#include <meta/value.hpp>

#include <tuple>
#include <type_traits>
#include <utility>

#include <iostream>


// Check for MSVC for workaround, see below. __clang__ check is required to exclude clang-cl.
#if defined(_MSC_VER) && !defined(__clang__)
    #define GRAPHLE_MSVC_WORKAROUND
#endif


namespace graphle::meta {
    namespace detail {
        /**
         * Workaround for MSVC which really hates syntax of the form type_list<value<Indices>...> for some reason,
         * except when it is placed into a separate typedef like this.
         * Clang works with the original version of this code, but fails (cannot mangle this name) when using this workaround,
         * so add a compiler check before using it.
         *
         * @note: Bug last confirmed present on Visual Studio 2022, version 17.8.4.
         */
        template <typename Sequence> using indices_as_list = decltype([] <std::size_t... Is> (std::index_sequence<Is...>) {
            return type_list<value<Is>...>{};
        } (Sequence {}));


        /** Converts a pack of meta::value to an std::index_sequence of those values. */
        template <typename... Ts> struct values_to_sequence {
            using type = std::index_sequence<Ts::value...>;
        };


        /** Permutes the given tuple, invoking fn(P...) for every permuted set of tuple elements P. */
        template <typename Tuple, typename F, std::size_t... Current, std::size_t... Remaining>
        constexpr inline void foreach_tuple_permutation(Tuple&& tuple, F&& fn, std::index_sequence<Current...> current, std::index_sequence<Remaining...> remaining) {
            if constexpr (sizeof...(Remaining) == 0) {
                std::invoke(fn, std::get<Current>(tuple)...);
            } else {
                [&] <std::size_t... Is> (std::index_sequence<Is...>) {
                    ([&] <std::size_t I> (value<I>) {
                        #ifdef GRAPHLE_MSVC_WORKAROUND
                            using remaining_list = indices_as_list<decltype(remaining)>;
                        #else
                            using remaining_list = type_list<value<Remaining>...>;
                        #endif


                        foreach_tuple_permutation(
                            tuple,
                            fn,
                            // Append the Ith element of Remaining to current.
                            #ifdef GRAPHLE_MSVC_WORKAROUND
                                typename indices_as_list<decltype(current)>
                            #else
                                typename type_list<value<Current>...>
                            #endif
                                ::template append<typename remaining_list::template at<I>>
                                ::template to<values_to_sequence>
                                ::type {},
                            // Remove the Ith element from Remaining.
                            typename remaining_list
                                ::template erase_at<I>
                                ::template to<values_to_sequence>
                                ::type {}
                        );
                    } (value<Is>{}), ...);
                } (std::make_index_sequence<sizeof...(Remaining)>());
            }
        }
    }


    /** Permutes the given tuple, invoking fn(P...) for every permuted set of tuple elements P. */
    template <typename Tuple, typename F> constexpr inline void foreach_tuple_permutation(Tuple&& tuple, F&& fn) {
        constexpr std::size_t size = std::tuple_size_v<std::remove_cvref_t<Tuple>>;

        detail::foreach_tuple_permutation(
            tuple,
            fn,
            std::index_sequence<>{},
            std::make_index_sequence<size>()
        );
    }
}
#pragma once

#include <common.hpp>
#include <meta/type_list.hpp>
#include <meta/value.hpp>

#include <tuple>
#include <type_traits>
#include <utility>

#include <iostream>


namespace graphle::meta {
    namespace detail {
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
                        using remaining_list = type_list<value<Remaining>...>;


                        foreach_tuple_permutation(
                            tuple,
                            fn,
                            // Append the Ith element of Remaining to current.
                            typename type_list<value<Current>...>
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
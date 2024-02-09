#pragma once

#include <graphle.hpp>
#include <test_datastructures.hpp>

#include <vector>
#include <tuple>
#include <array>
#include <functional>


namespace graphle::test {
    /**
     * Foreach data structure in the list of structures, converts the graph 'src' to that data structure
     * and then invokes algorithm(structure, graph).
     * @tparam Structures A graphle::type_list of data structures to invoke 'algorithm' with.
     * @param src A source data structure to build the other data structures from.
     * @param algorithm An algorithm to invoke on each data structure.
     *  The provided parameter should be invocable as algorithm(structure, graph) for every structure type in 'Structures'.
     */
    template <graphle::meta::list_of_types Structures = graphle::test::datastructure_list>
    constexpr inline void foreach_as_datastructure(const graphle::test::ve_list_graph& src, auto&& algorithm) {
        Structures::foreach([&] <typename S> {
            auto as_structure = S::from_ve_list(src);
            auto as_graph     = as_structure.view_as_graph();

            std::invoke(algorithm, as_structure, as_graph);
        });
    }


    /** A list of vertices indicating the order vertices should be visited. */
    using vertex_order  = std::vector<std::size_t>;
    /** A list of vertex orderings indicating all possible orders in which vertices may be visited. */
    using order_options = std::vector<vertex_order>;


    /** Indicates the next ordering elements may match any of the provided options. */
    template <typename... Options> struct either {
        using is_either_tag = void;

        std::tuple<Options...> options;
        constexpr explicit either(Options... options) : options { std::move(options)... } {}
    };

    /** Indicates the next ordering elements must match the provided options in order. */
    template <typename... Elems> struct list {
        using is_list_tag = void;

        std::tuple<Elems...> elems;
        constexpr explicit list(Elems... elems) : elems { std::move(elems)... } {}
    };

    /** Indicates the next ordering elements must match the provided options in any order. */
    template <typename... Elems> struct unordered {
        using is_unordered_tag = void;

        std::tuple<Elems...> elems;
        constexpr explicit unordered(Elems... elems) : elems { std::move(elems)... } {}
    };


    namespace detail {
        constexpr inline auto parse_either_element(const auto& elem);
        constexpr inline auto parse_list_element(const auto& elem);
        constexpr inline auto parse_unordered_element(const auto& elem);
    }


    /**
     * Constructs a list of vertex orderings from the given ordering definition.
     * Each argument should either be a vertex index, an either, a list or an unordered. Usage example:
     *
     * ~~~
     * // Produces the following orderings:
     * // 0, 1, 3, 6, 7
     * // 0, 1, 3, 7, 6
     * // 0, 1, 4, 5, 6, 7
     * // 0, 1, 4, 5, 7, 6
     * auto ordering = make_ordering(
     *     0, 1,                        // First visit 1 and 2 in that order
     *     either { 3, list { 4, 5 } }, // Then visit either 3 or both 4 and 5 in that order
     *     unordered { 6, 7 }           // Then visit both 6 and 7 in any order
     * );
     * ~~~
     *
     * @note:
     *  This method evaluates <b>all</b> valid vertex orderings for the provided arguments.
     *  The number of orderings can explode very quickly for larger graphs.
     *  This code should therefore only be used for testing relatively small graphs, like the ones in test_data.hpp.
     */
    template <typename... Args> constexpr order_options make_ordering(const Args&... ordering) {
        auto parse_element = [] <typename E> (const E& elem) {
            if constexpr (requires { typename E::is_either_tag; }) {
                return detail::parse_either_element(elem);
            } else if constexpr (requires { typename E::is_list_tag; }) {
                return detail::parse_list_element(elem);
            } else if constexpr (requires { typename E::is_unordered_tag; }) {
                return detail::parse_unordered_element(elem);
            } else if constexpr (std::is_integral_v<E>) {
                return order_options { vertex_order { (std::size_t) elem } };
            }
        };


        if constexpr (sizeof...(ordering) == 1) return parse_element(ordering...);
        else return parse_element(list { ordering... });
    }


    namespace detail {
        /** Generate a order_options object from an either ordering component. */
        constexpr inline auto parse_either_element(const auto& elem) {
            return std::apply([] (const auto&... elems) {
                auto options = std::array { make_ordering(elems)... };

                order_options result;
                for (const auto& option : options) result.insert(result.end(), option.begin(), option.end());
                return result;
            }, elem.options);
        }


        /** Generate a order_options object from a list ordering component. */
        constexpr inline auto parse_list_element(const auto& elem) {
            return std::apply([] (const auto&... elems) {
                auto order = std::array { make_ordering(elems)... };

                order_options result;

                for (const auto& order_elem : order) {
                    if (result.empty()) {
                        result = std::move(order_elem);
                    } else {
                        order_options new_result;

                        /** @todo: Can use std::views::cartesian_product here once we have C++23. */
                        for (const auto& existing_option : result) {
                            for (const auto& new_option : order_elem) {
                                new_result.emplace_back(existing_option);
                                new_result.back().insert(new_result.back().end(), new_option.begin(), new_option.end());
                            }
                        }

                        result = std::move(new_result);
                    }
                }

                return result;
            }, elem.elems);
        }


        /** Generate a order_options object from an unordered ordering component. */
        constexpr inline auto parse_unordered_element(const auto& elem) {
            return std::apply([] (const auto&... elems) {
                order_options result;

                meta::foreach_tuple_permutation(
                    std::forward_as_tuple(elems...),
                    [&] (const auto&... permutation) {
                        auto options_for_permutation = make_ordering(permutation...);
                        result.insert(result.end(), options_for_permutation.begin(), options_for_permutation.end());
                    }
                );

                return result;
            }, elem.elems);
        }
    }
}
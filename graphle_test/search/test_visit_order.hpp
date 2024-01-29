#pragma once

#include <graphle.hpp>
#include <test_datastructures.hpp>

#include <cstddef>
#include <vector>
#include <span>
#include <ranges>
#include <typeinfo>
#include <format>
#include <tuple>
#include <array>


namespace graphle::test {
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
     *  This method evaluates all valid vertex orderings for the provided arguments.
     *  The number of orderings can explode very quickly for larger graphs.
     *  This code should therefore only be used for testing relatively small graphs, like the ones in test_data.hpp.
     */
    template <typename... Args> constexpr order_options make_ordering(const Args&... ordering) {
        auto parse_element = [] <typename E> (const E& elem) {
            if constexpr (requires { typename E::is_either_tag; }) {
                return std::apply([] (const auto&... elems) {
                    auto options = std::array { make_ordering(elems)... };

                    order_options result;
                    for (const auto& option : options) result.insert(result.end(), option.begin(), option.end());
                    return result;
                }, elem.options);
            }

            else if constexpr (requires { typename E::is_list_tag; }) {
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

            else if constexpr (requires { typename E::is_unordered_tag; }) {
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

            else if constexpr (std::is_integral_v<E>) {
                return order_options { vertex_order { (std::size_t) elem } };
            }
        };


        if constexpr (sizeof...(ordering) == 1) return parse_element(ordering...);
        else return parse_element(list { ordering... });
    }


    /**
     * Tests that the order the given search algorithm visits the vertices of the given graph in is the same as one of the provided orderings.
     * @tparam DataStructures A type_list of datastructures that work with the given algorithm.
     * @param src Graph data to construct the given datastructures from.
     * @param root The root vertex to start the search from.
     * @param options A list of orderings that are valid orders for the visitor to visit the vertices in.
     * @param algorithm The search algorithm to test.
     */
    template <graphle::meta::list_of_types DataStructures, typename Algorithm>
    constexpr inline void test_visit_order(const graphle::test::ve_list_graph& src, std::size_t root, const order_options& options, Algorithm&& algorithm) {
        DataStructures::foreach([&] <typename DS> {
            // Transform to spans so we can easily remove from the front.
            auto as_spans = options | views::transform([] (const auto& option) { return std::span { option }; });
            auto remaining_options = std::vector (as_spans.begin(), as_spans.end());

            auto structure = DS::from_ve_list(src);
            auto graph     = structure.view_as_graph();

            using vertex = graphle::vertex_of<decltype(graph)>;
            using edge   = graphle::edge_of<decltype(graph)>;


            auto visitor = graphle::search::visitor_from_arguments {
                .deduce_graph_type = graphle::meta::deduce_as<decltype(graph)>,
                .discover_vertex   = [&] (auto vertex, auto& graph) {
                    // Keep all orderings that have this vertex as their next vertex.
                    decltype(remaining_options) keep_options;

                    for (const auto& option : remaining_options) {
                        if (!option.empty() && option.front() == vertex->vertex_id) {
                            keep_options.push_back(option.subspan(1));
                        }
                    }

                    remaining_options = std::move(keep_options);
                },
                .finish_search = [&] (auto& graph) {
                    // There should be at least one ordering left that matched all our vertices, and there should be no remaining vertices in that ordering.
                    ASSERT_FALSE(remaining_options.empty());
                    ASSERT_TRUE(rng::any_of(remaining_options, [] (const auto& o) { return o.empty(); }));
                }
            };


            SUBTEST_SCOPE(typeid(DS).name()) {
                auto root_vertex = [&] {
                    if constexpr (vertex_list_graph<decltype(graph)>) {
                        return util::find_vertex(graph, [&] (auto v) { return v->vertex_id == root; });
                    } else if constexpr (requires { structure.root; }) {
                        if (structure.root->vertex_id == root) return structure.root.get();
                    }

                    FAIL("Cannot find root vertex for the given graph type.");
                } ();


                std::invoke(
                    algorithm,
                    graph,
                    root_vertex,
                    visitor
                );
            }
        });
    }
}



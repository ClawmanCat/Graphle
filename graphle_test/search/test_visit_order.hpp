#pragma once

#include <graphle.hpp>
#include <test_datastructures.hpp>
#include <test_utils.hpp>

#include <span>
#include <ranges>
#include <typeinfo>
#include <format>
#include <vector>


namespace graphle::test {
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
        foreach_as_datastructure<DataStructures>(src, [&] <typename DS, typename G> (DS& structure, G& graph) {
            // Transform to spans so we can easily remove from the front.
            auto as_spans = options | views::transform([] (const auto& option) { return std::span { option }; });
            auto remaining_options = std::vector (as_spans.begin(), as_spans.end());

            using vertex = graphle::vertex_of<G>;
            using edge   = graphle::edge_of<G>;


            auto visitor = graphle::search::visitor_from_arguments {
                .deduce_graph_type = graphle::meta::deduce_as<G>,
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
                    if constexpr (vertex_list_graph<G>) {
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



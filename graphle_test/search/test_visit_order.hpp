#pragma once

#include <graphle.hpp>
#include <test_datastructures.hpp>

#include <cstddef>
#include <vector>
#include <span>
#include <ranges>
#include <typeinfo>
#include <format>


namespace graphle::test {
    using vertex_list  = std::vector<std::size_t>;
    using vertex_order = std::vector<vertex_list>;


    /**
     * Tests that the order the given search algorithm visits the vertices of the given graph in is the same as the provided order.
     * @tparam DataStructures A type_list of datastructures that work with the given algorithm.
     * @param src Graph data to construct the given datastructures from.
     * @param root The root vertex to start the search from.
     * @param ordering The expected order vertices are visited in.
     * @param algorithm The search algorithm to test.
     */
    template <graphle::meta::list_of_types DataStructures, typename Algorithm>
    inline void test_visit_order(const graphle::test::ve_list_graph& src, std::size_t root, const vertex_order& vertex_ordering, Algorithm&& algorithm) {
        DataStructures::foreach([&] <typename DS> {
            // Intentional copy: we will remove visited elements.
            vertex_order vertex_order_copy = vertex_ordering;
            // Use span so we can easily pop from the front.
            std::span ordering = vertex_order_copy;

            auto structure = DS::from_ve_list(src);
            auto graph     = structure.view_as_graph();

            using vertex = graphle::vertex_of<decltype(graph)>;
            using edge   = graphle::edge_of<decltype(graph)>;


            auto visitor = graphle::search::visitor_from_arguments {
                .deduce_graph_type = graphle::meta::deduce_as<decltype(graph)>,
                .discover_vertex   = [&] (auto vertex, auto& graph) {
                    auto it = std::ranges::find(ordering.front(), vertex->vertex_id);
                    ASSERT_FALSE(it == ordering.front().end());

                    ordering.front().erase(it);
                    if (ordering.front().empty()) ordering = ordering.subspan(1);
                },
                .finish_search = [&] (auto& graph) {
                    ASSERT_TRUE(ordering.empty());
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



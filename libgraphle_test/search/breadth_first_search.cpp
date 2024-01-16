#include <test_framework.hpp>
#include <test_datastructures.hpp>
#include <test_data.hpp>


using vertex_list = std::vector<std::size_t>;


/**
 * @test bfs::visit_tree
 * Visits a tree-like graph using BFS and asserts visitor methods are called in the expected order.
 */
TEST(bfs, visit_tree) {
    const auto tree = graphle::test::make_tree_graph();


    auto expected_visit_order = std::deque {
        vertex_list { 0 },
        vertex_list { 1 },
        vertex_list { 2, 5 },
        vertex_list { 3, 6 },
        vertex_list { 4, 7, 8, 9, 10 },
    };


    graphle::test::datastructure_list::foreach([&] <typename DS> {
        auto as_structure = DS::from_ve_list(tree);
        auto as_graph     = as_structure.view_as_graph();

        using vertex = graphle::vertex_of<decltype(as_graph)>;


        auto visitor = graphle::search::visitor_from_arguments {
            .deduce_graph_type = graphle::meta::deduce_as<decltype(as_graph)>,
            .discover_vertex   = [&] (auto vertex, auto& graph) {
                auto it = std::ranges::find(expected_visit_order.front(), vertex->vertex_id);
                ASSERT_FALSE(it == expected_visit_order.front().end());

                expected_visit_order.front().erase(it);
                if (expected_visit_order.front().empty()) expected_visit_order.pop_front();
            }
        };


        auto vs = as_graph.get_vertices();

        graphle::search::breadth_first_search(
            as_graph,
            *std::ranges::find_if(vs, [] (auto* v) { return v->vertex_id == 0; }),
            visitor
        );


        ASSERT_TRUE(expected_visit_order.empty());
    });
}
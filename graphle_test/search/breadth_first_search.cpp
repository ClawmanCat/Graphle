#include <test_framework.hpp>
#include <test_data.hpp>
#include <search/test_visit_order.hpp>


using graphle::test::vertex_list;


/**
 * @test bfs::visit_tree
 * Visits a tree-like graph using BFS and asserts visitor methods are called in the expected order.
 */
TEST(bfs, visit_tree) {
    const auto tree = graphle::test::make_tree_graph();


    auto expected_visit_order = graphle::test::vertex_order {
        vertex_list { 0 },
        vertex_list { 1 },
        vertex_list { 2, 5 },
        vertex_list { 3, 6 },
        vertex_list { 4, 7, 8, 9, 10 },
    };


    using supported_datastructures = graphle::meta::type_list<
        graphle::test::ve_list_graph,
        graphle::test::ve_map_graph,
        graphle::test::out_edge_graph,
        graphle::test::v_list_out_edge_graph,
        graphle::test::in_out_edge_graph,
        graphle::test::v_list_in_out_edge_graph
    >;


    graphle::test::test_visit_order<supported_datastructures>(
        tree,
        0,
        expected_visit_order,
        [] (auto&&... args) { graphle::search::breadth_first_search(GRAPHLE_FWD(args)...); }
    );
}
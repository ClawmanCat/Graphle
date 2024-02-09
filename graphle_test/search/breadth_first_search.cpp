#include <test_framework.hpp>
#include <test_data.hpp>
#include <search/test_visit_order.hpp>


using graphle::test::either;
using graphle::test::list;
using graphle::test::unordered;


/** Subset of datastructures for which we can easily find the root vertex if its not equal to structure.root. */
using datastructure_list_find_root = graphle::meta::type_list<
    graphle::test::ve_list_graph,
    graphle::test::ve_map_graph,
    graphle::test::v_list_out_edge_graph,
    graphle::test::v_list_in_out_edge_graph
>;


/**
 * @test bfs::visit_tree
 * Visits a tree-like graph using BFS and asserts visitor methods are called in the expected order.
 */
TEST(bfs, visit_tree) {
    const auto tree = graphle::test::make_tree_graph();

    auto expected_visit_order = graphle::test::make_ordering(
        0, 1,
        unordered { 2, 5 },
        unordered { 3, 6 },
        unordered { 4, 7, 8, 9, 10 }
    );

    graphle::test::test_visit_order<graphle::test::datastructure_list>(
        tree,
        0,
        expected_visit_order,
        [] (auto&&... args) { graphle::search::breadth_first_search(GRAPHLE_FWD(args)...); }
    );
}


/**
 * @test bfs::visit_dag
 * Visits a dag graph using BFS and asserts visitor methods are called in the expected order.
 */
TEST(bfs, visit_dag) {
    const auto dag = graphle::test::make_dag_graph();


    auto expected_visit_order_from_0 = graphle::test::make_ordering(
        0, 2,
        unordered { 3, 6 },
        unordered { 4, 7 },
        unordered { 5, 8 }
    );


    auto expected_visit_order_from_1 = graphle::test::make_ordering(
        1, 3, 4, 5,
        unordered { 6, 8 },
        7
    );


    SUBTEST_SCOPE("root_vertex_0") {
        graphle::test::test_visit_order<graphle::test::datastructure_list>(
            dag,
            0,
            expected_visit_order_from_0,
            [] (auto&&... args) { graphle::search::breadth_first_search(GRAPHLE_FWD(args)...); }
        );
    }


    SUBTEST_SCOPE("root_vertex_1") {
        graphle::test::test_visit_order<datastructure_list_find_root>(
            dag,
            1,
            expected_visit_order_from_1,
            [] (auto&&... args) { graphle::search::breadth_first_search(GRAPHLE_FWD(args)...); }
        );
    }
}


/**
 * @test bfs::visit_cyclic
 * Visits a cyclic graph using BFS and asserts visitor methods are called in the expected order.
 */
TEST(bfs, visit_cyclic) {
    const auto cyclic = graphle::test::make_cyclic_graph();


    auto expected_visit_order_from_0 = graphle::test::make_ordering(
        0, 2, 3, 4, 5, 6, 7, 8
    );


    auto expected_visit_order_from_1 = graphle::test::make_ordering(
        1, 3, 4, 5, 6,
        unordered { 7, 2 },
        8
    );


    SUBTEST_SCOPE("root_vertex_0") {
        graphle::test::test_visit_order<graphle::test::datastructure_list>(
            cyclic,
            0,
            expected_visit_order_from_0,
            [] (auto&&... args) { graphle::search::breadth_first_search(GRAPHLE_FWD(args)...); }
        );
    }


    SUBTEST_SCOPE("root_vertex_1") {
        graphle::test::test_visit_order<datastructure_list_find_root>(
            cyclic,
            1,
            expected_visit_order_from_1,
            [] (auto&&... args) { graphle::search::breadth_first_search(GRAPHLE_FWD(args)...); }
        );
    }
}


/**
 * @test bfs::visit_linear
 * Visits a linear graph using BFS and asserts visitor methods are called in the expected order.
 */
TEST(bfs, visit_linear) {
    const auto linear = graphle::test::make_linear_graph();


    auto expected_visit_order = graphle::test::make_ordering(
        0, 1, 2, 3, 4, 5
    );


    graphle::test::test_visit_order<graphle::test::datastructure_list>(
        linear,
        0,
        expected_visit_order,
        [] (auto&&... args) { graphle::search::breadth_first_search(GRAPHLE_FWD(args)...); }
    );
}


/**
 * @test bfs::visit_bidirectional_edge
 * Visits a bidirectional edge graph using BFS and asserts visitor methods are called in the expected order.
 */
TEST(bfs, visit_bidirectional_edge) {
    const auto bidir_edge = graphle::test::make_bidirectional_edge_graph();


    auto expected_visit_order_from_0 = graphle::test::make_ordering(
        0, 1, 2,
        unordered { 3, 4 },
        5
    );


    auto expected_visit_order_from_4 = graphle::test::make_ordering(
        4, 2,
        unordered { 1, 3 },
        5
    );


    SUBTEST_SCOPE("root_vertex_0") {
        graphle::test::test_visit_order<graphle::test::datastructure_list>(
            bidir_edge,
            0,
            expected_visit_order_from_0,
            [] (auto&&... args) { graphle::search::breadth_first_search(GRAPHLE_FWD(args)...); }
        );
    }


    SUBTEST_SCOPE("root_vertex_4") {
        graphle::test::test_visit_order<datastructure_list_find_root>(
            bidir_edge,
            4,
            expected_visit_order_from_4,
            [] (auto&&... args) { graphle::search::breadth_first_search(GRAPHLE_FWD(args)...); }
        );
    }
}


/**
 * @test bfs::visit_size_one
 * Visits a size-one graph using BFS and asserts visitor methods are called in the expected order.
 */
TEST(bfs, visit_size_one) {
    const auto size_one = graphle::test::make_size_one_graph();


    auto expected_visit_order = graphle::test::make_ordering(0);


    graphle::test::test_visit_order<graphle::test::datastructure_list>(
        size_one,
        0,
        expected_visit_order,
        [] (auto&&... args) { graphle::search::breadth_first_search(GRAPHLE_FWD(args)...); }
    );
}
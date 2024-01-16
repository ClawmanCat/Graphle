#include <test_framework.hpp>
#include <test_datastructures.hpp>


struct vertex {};


/**
 * @test search_visitor::from_arguments_empty
 * Construct a search visitor without any visitor methods and check if it fulfils the required constraints.
 */
TEST(search_visitor, from_arguments_empty) {
    auto g = graphle::graph {
        .deduce_vertex_type = graphle::meta::deduce_as<vertex>
    };

    auto v = graphle::search::visitor_from_arguments {
        .deduce_graph_type = graphle::meta::deduce_as<decltype(g)>
    };

    ASSERT_TRUE(graphle::search::search_visitor_type<decltype(v), decltype(g)>);
}


/**
 * @test search_visitor::from_arguments_mono
 * Construct a search visitor one visitor method (discover_vertex) and check if it fulfils the required constraints.
 */
TEST(search_visitor, from_arguments_mono) {
    auto g = graphle::graph {
        .deduce_vertex_type = graphle::meta::deduce_as<vertex>
    };

    auto v = graphle::search::visitor_from_arguments {
        .deduce_graph_type = graphle::meta::deduce_as<decltype(g)>,
        .discover_vertex   = [] (auto v, auto& g) {}
    };

    ASSERT_TRUE(graphle::search::search_visitor_type<decltype(v), decltype(g)>);
}


/**
 * @test search_visitor::from_arguments_all
 * Construct a search visitor with all visitor methods and check if it fulfils the required constraints.
 */
TEST(search_visitor, from_arguments_all) {
    auto g = graphle::graph {
        .deduce_vertex_type = graphle::meta::deduce_as<vertex>
    };

    auto v = graphle::search::visitor_from_arguments {
        .deduce_graph_type  = graphle::meta::deduce_as<decltype(g)>,
        .discover_vertex    = [] (auto v, auto& g) {},
        .discover_leaf      = [] (auto v, auto& g) {},
        .discover_branch    = [] (auto v, auto& g) {},
        .discover_new_edge  = [] (auto e, auto& g) {},
        .discover_seen_edge = [] (auto e, auto& g) {},
        .begin_search       = [] (auto& g) {},
        .finish_search      = [] (auto& g) {}
    };

    ASSERT_TRUE(graphle::search::search_visitor_type<decltype(v), decltype(g)>);
}
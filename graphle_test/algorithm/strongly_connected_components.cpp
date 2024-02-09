#include <graphle.hpp>
#include <test_framework.hpp>
#include <test_datastructures.hpp>
#include <test_data.hpp>
#include <test_utils.hpp>

#include <vector>
#include <algorithm>
#include <typeinfo>


namespace graphle::test {
    using cycle            = std::vector<std::size_t>;
    using cycle_list       = std::vector<cycle>;
    using supported_graphs = meta::type_list<ve_list_graph>;


    constexpr inline void test_strongly_connected_components(const ve_list_graph& graph, std::size_t min_size, const cycle_list& expected_cycles) {
        graphle::test::foreach_as_datastructure<supported_graphs>(graph, [&] <typename DS> (const DS& structure, const auto& graph) {
            SUBTEST_SCOPE(typeid(DS).name()) {
                auto result = alg::strongly_connected_components(graph, min_size);
                ASSERT_TRUE(result.size() == expected_cycles.size());
                ASSERT_TRUE(rng::all_of(result, [&] (const auto& cycle) { return cycle.size() >= min_size; }));

                for (const auto& cycle : expected_cycles) {
                    for (auto possible_match : result) {
                        if (possible_match.size() != cycle.size()) continue;

                        for (std::size_t i = 0; i < possible_match.size(); ++i) {
                            rng::rotate(possible_match, std::next(possible_match.begin()));

                            if (std::ranges::equal(
                                possible_match,
                                cycle,
                                [] (const auto* v, const auto id) { return v->vertex_id == id; })
                            ) goto on_cycle_matched;
                        }
                    }

                    FAIL("No matching cycle for expected cycle in result.");
                    on_cycle_matched:;
                }
            }
        });
    }
}


TEST(strongly_connected_components, visit_tree) {
    auto tree = graphle::test::make_tree_graph();


    SUBTEST_SCOPE("min_size_1") {
        graphle::test::test_strongly_connected_components(
            tree,
            1,
            { { 0 }, { 1 }, { 2 }, { 3 }, { 4 }, { 5 }, { 6 }, { 7 }, { 8 }, { 9 }, { 10 } }
        );
    }


    SUBTEST_SCOPE("min_size_2") {
        graphle::test::test_strongly_connected_components(
            tree,
            2,
            { }
        );
    }
}


TEST(strongly_connected_components, visit_dag) {
    auto dag = graphle::test::make_dag_graph();


    SUBTEST_SCOPE("min_size_1") {
        graphle::test::test_strongly_connected_components(
            dag,
            1,
            { { 0 }, { 1 }, { 2 }, { 3 }, { 4 }, { 5 }, { 6 }, { 7 }, { 8 } }
        );
    }


    SUBTEST_SCOPE("min_size_2") {
        graphle::test::test_strongly_connected_components(
            dag,
            2,
            { }
        );
    }
}


TEST(strongly_connected_components, visit_cyclic_graph) {
    auto cyclic = graphle::test::make_cyclic_graph();


    SUBTEST_SCOPE("min_size_1") {
        graphle::test::test_strongly_connected_components(
            cyclic,
            1,
            {
                { 0 }, { 1 }, { 2 }, { 3 }, { 4 }, { 5 }, { 6 }, { 7 }, { 8 },
                { 5, 6, 7, 8 },
                { 2, 3, 4, 5, 6 }
            }
        );
    }


    SUBTEST_SCOPE("min_size_2") {
        graphle::test::test_strongly_connected_components(
            cyclic,
            2,
            {
                { 5, 6, 7, 8 },
                { 2, 3, 4, 5, 6 }
            }
        );
    }
}


TEST(strongly_connected_components, visit_linear_graph) {
    auto linear = graphle::test::make_linear_graph();


    SUBTEST_SCOPE("min_size_1") {
        graphle::test::test_strongly_connected_components(
            linear,
            1,
            { { 0 }, { 1 }, { 2 }, { 3 }, { 4 }, { 5 } }
        );
    }


    SUBTEST_SCOPE("min_size_2") {
        graphle::test::test_strongly_connected_components(
            linear,
            2,
            { }
        );
    }
}


TEST(strongly_connected_components, visit_bidirectional_edge_graph) {
    auto bidirectional_edge = graphle::test::make_bidirectional_edge_graph();


    SUBTEST_SCOPE("min_size_1") {
        graphle::test::test_strongly_connected_components(
            bidirectional_edge,
            1,
            {
                { 0 }, { 1 }, { 2 }, { 3 }, { 4 }, { 5 },
                { 1, 2 }, { 2, 4 }, { 3, 5 },
                { 2, 3, 5, 4 }
            }
        );
    }


    SUBTEST_SCOPE("min_size_2") {
        graphle::test::test_strongly_connected_components(
            bidirectional_edge,
            2,
            {
                { 1, 2 }, { 2, 4 }, { 3, 5 },
                { 2, 3, 5, 4 }
            }
        );
    }
}


TEST(strongly_connected_components, visit_size_1) {
    auto size_1 = graphle::test::make_size_one_graph();


    SUBTEST_SCOPE("min_size_1") {
        graphle::test::test_strongly_connected_components(
            size_1,
            1,
            {
                { 0 }
            }
        );
    }


    SUBTEST_SCOPE("min_size_2") {
        graphle::test::test_strongly_connected_components(
            size_1,
            2,
            { }
        );
    }
}


TEST(strongly_connected_components, visit_empty) {
    auto empty = graphle::test::make_empty_graph();


    SUBTEST_SCOPE("min_size_1") {
        graphle::test::test_strongly_connected_components(
            empty,
            1,
            { }
        );
    }


    SUBTEST_SCOPE("min_size_2") {
        graphle::test::test_strongly_connected_components(
            empty,
            2,
            { }
        );
    }
}
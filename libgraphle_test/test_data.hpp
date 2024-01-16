#pragma once

#include <libgraphle.hpp>
#include <test_datastructures.hpp>

#include <array>
#include <vector>


namespace graphle::test {
    /** @defgroup TestData Graphle Test Data */


    /**
     * @ingroup TestData
     * A tree-like directed graph with a varying number of children per vertex.
     * ~~~
     *      ┌-> 2 -> 3 -> 4
     * 0 -> 1
     *      └-> 5 -> 6 -> 7
     *               ├ -> 8
     *               ├ -> 9
     *               └ -> 10
     * ~~~
     */
     constexpr inline ve_list_graph make_tree_graph(void) {
         using V = typename ve_list_graph::vertex;

         return ve_list_graph {
             .vertices { V{0}, V{1}, V{2}, V{3}, V{4}, V{5}, V{6}, V{7}, V{8}, V{9}, V{10} },
             .edges    {
                 { V{0}, V{1} },
                 { V{1}, V{2} }, { V{2}, V{3} }, { V{3}, V{4} },
                 { V{1}, V{5} }, { V{5}, V{6} },
                 { V{6}, V{7} }, { V{6}, V{8} }, { V{6}, V{9} }, { V{6}, V{10} }
             }
         };
     }
     

     /**
      * @ingroup TestData
      * A directed acyclic graph (DAG).
      * ~~~
      * 0 -> 2 ------> 6 -> 7
      *      |         ^
      *      v         |
      * 1 -> 3 -> 4 -> 5 -> 8
      * ~~~
      */
     constexpr inline ve_list_graph make_dag_graph(void) {
         using V = typename ve_list_graph::vertex;

         return ve_list_graph {
             .vertices { V{0}, V{1}, V{2}, V{3}, V{4}, V{5}, V{6}, V{7}, V{8} },
             .edges    {
                 { V{0}, V{2} }, { V{2}, V{6} }, { V{6}, V{7} },
                 { V{1}, V{3} }, { V{3}, V{4} }, { V{4}, V{5} }, { V{5}, V{8} },
                 { V{2}, V{3} },
                 { V{5}, V{6} }
             }
         };
     }


    /**
     * @ingroup TestData
     * A directed graph with two cycles ([2 -> 3 -> 4 -> 5 -> 6 -> 2] and [5 -> 6 -> 7 -> 8 -> 5]).
     * ~~~
     * 0 -> 2 <------ 6 -> 7
     *      |         ^    |
     *      v         |    v
     * 1 -> 3 -> 4 -> 5 <- 8
     * ~~~ 
     */
    constexpr inline ve_list_graph make_cyclic_graph(void) {
        using V = typename ve_list_graph::vertex;

        return ve_list_graph {
            .vertices { V{0}, V{1}, V{2}, V{3}, V{4}, V{5}, V{6}, V{7}, V{8} },
            .edges    {
                { V{0}, V{2} },
                { V{1}, V{3} },
                { V{3}, V{4} }, { V{4}, V{5} }, { V{5}, V{6} }, { V{6}, V{2} }, { V{2}, V{3} },
                { V{6}, V{7} }, { V{7}, V{8} }, { V{8}, V{5} }
            }
        };
    }


    /**
     * @ingroup TestData
     * A directed graph where each vertex has exactly zero or one out edge.
     * ~~~
     * 0 -> 1 -> 2 -> 3 -> 4 -> 5
     * ~~~
     */
    constexpr inline ve_list_graph make_linear_graph(void) {
        using V = typename ve_list_graph::vertex;

        return ve_list_graph {
            .vertices { V{0}, V{1}, V{2}, V{3}, V{4}, V{5} },
            .edges    {
                { V{0}, V{1} }, { V{1}, V{2} }, { V{2}, V{3} }, { V{3}, V{4} }, { V{4}, V{5} }
            }
        };
    }


    /**
     * @ingroup TestData
     * A directed graph with vertices that have edges in both directions.
     * ~~~
     * 0 -> 1 <-> 2 <-> 4
     *            |     ^
     *            v     |
     *            3 <-> 5
     * ~~~
     */
    constexpr inline ve_list_graph make_bidirectional_edge_graph(void) {
        using V = typename ve_list_graph::vertex;

        return ve_list_graph {
            .vertices { V{0}, V{1}, V{2}, V{3}, V{4}, V{5} },
            .edges    {
                { V{0}, V{1} }, { V{1}, V{2} }, { V{2}, V{1} },
                { V{2}, V{3} }, { V{3}, V{5} }, { V{5}, V{3} }, { V{5}, V{4} }, { V{4}, V{2} }, { V{2}, V{4} }
            }
        };
    }


    /**
     * @ingroup TestData
     * A graph with one vertex.
     * ~~~
     * 0
     * ~~~
     */
    constexpr inline ve_list_graph make_size_one_graph(void) {
        using V = typename ve_list_graph::vertex;

        return ve_list_graph { .vertices { V{0} } };
    }


    /**
     * @ingroup TestData
     * A graph with no vertices.
     */
    constexpr inline ve_list_graph make_empty_graph(void) {
        return ve_list_graph { };
    }


    /**
     * @ingroup TestData
     * Creates a std::array of the results of all make_*_graph methods.
     */
    constexpr inline auto make_graphs(void) {
        return std::array {
            make_tree_graph(),
            make_dag_graph(),
            make_cyclic_graph(),
            make_linear_graph(),
            make_bidirectional_edge_graph(),
            make_size_one_graph(),
            make_empty_graph()
        };
    }
}
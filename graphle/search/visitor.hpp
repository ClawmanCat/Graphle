#pragma once

#include <common.hpp>
#include <graph/graph.hpp>
#include <utility/detail/functional.hpp>


#define GRAPHLE_VISIT_DERIVED(F, D, ...)                            \
[&] {                                                               \
    if constexpr (requires (Derived d) { d.F(__VA_ARGS__); }) {     \
        auto invoker = [&] () -> decltype(auto) {                   \
            return static_cast<Derived*>(this)->F(__VA_ARGS__);     \
        };                                                          \
                                                                    \
                                                                    \
        using result_t = std::invoke_result_t<decltype(invoker)>;   \
                                                                    \
        if constexpr (std::is_same_v<result_t, void>) {             \
            invoker();                                              \
            return D;                                               \
        } else {                                                    \
            return invoker();                                       \
        }                                                           \
    } else return D;                                                \
}()


namespace graphle::search {
    /**
     * @ingroup Search
     * Indicates to a search algorithm what to do after invoking a visitor method.
     */
    enum class visitor_result {
        /** Continue the search normally. */
        CONTINUE,
        /** Do not visit any vertices by reaching them from the current vertex, but continue the search otherwise. */
        STOP_TREE,
        /** Stop the search and return early. */
        STOP_SEARCH
    };


    /**
     * @ingroup Search
     * Equivalent to visitor_result, but not linked to any specific location in the graph, so lacking the option STOP_TREE.
     */
    enum class nonlocal_visitor_result {
        /** Continue the search normally. */
        CONTINUE,
        /** Stop the search and return early. */
        STOP_SEARCH
    };




    /**
     * @ingroup Search
     * CRTP base class for visitors used with search algorithms.
     * Derived classes may override any of the methods marked with *_base in this class by removing the *_base suffix from the derived method.
     *  (i.e. your derived method should be called discover_vertex not discover_vertex_base).
     * Derived methods may return either void or a visitor_result.
     *
     * @tparam Derived The CRTP-derived type.
     * @tparam Graph The associated graph type.
     */
    template <typename Derived, graph_ref Graph> class search_visitor {
    public:
        using graph  = std::remove_reference_t<Graph>;
        using vertex = vertex_of<Graph>;
        using edge   = edge_of<Graph>;


        /** Called when a vertex is first visited during search. */
        visitor_result discover_vertex_base(vertex v, graph& g) {
            return GRAPHLE_VISIT_DERIVED(discover_vertex, visitor_result::CONTINUE, v, g);
        }


        /**
         * Called after discover_vertex if the given vertex is a branch (I.e. it has outgoing edges).
         * If the graph is non-directed, there is no difference between ingoing and outgoing edges, so this method is never called.
         */
        visitor_result discover_branch_base(vertex v, graph& g) {
            return GRAPHLE_VISIT_DERIVED(discover_branch, visitor_result::CONTINUE, v, g);
        }


        /**
         * Called after discover_vertex if the given vertex is a leaf (I.e. it has no outgoing edges).
         * If the graph is non-directed, there is no difference between ingoing and outgoing edges, so this method is never called.
         */
        visitor_result discover_leaf_base(vertex v, graph& g) {
            return GRAPHLE_VISIT_DERIVED(discover_leaf, visitor_result::CONTINUE, v, g);
        }


        /** Called when an edge is visited that leads to an unvisited vertex. */
        visitor_result discover_edge_to_new_vertex_base(edge e, graph& g) {
            return GRAPHLE_VISIT_DERIVED(discover_edge_to_new_vertex, visitor_result::CONTINUE, e, g);
        }


        /** Called when an edge is visited that leads to an already-visited vertex. */
        visitor_result discover_edge_to_known_vertex_base(edge e, graph& g) {
            return GRAPHLE_VISIT_DERIVED(discover_edge_to_known_vertex, visitor_result::CONTINUE, e, g);
        }


        /** Called before the first vertex is visited. */
        nonlocal_visitor_result begin_search_base(graph& g) {
            return GRAPHLE_VISIT_DERIVED(begin_search, nonlocal_visitor_result::CONTINUE, g);
        }


        /** Called after the last vertex is visited. */
        nonlocal_visitor_result finish_search_base(graph& g) {
            return GRAPHLE_VISIT_DERIVED(finish_search, nonlocal_visitor_result::CONTINUE, g);
        }
    private:
        template <typename F, typename D, typename... Args> requires std::is_invocable_v<F, Args...>
        D call_derived_method(F method, D default_value, Args&&... args) {
            if constexpr (std::is_same_v<std::invoke_result_t<F, Args...>, void>) {
                std::invoke(method, GRAPHLE_FWD(args)...);
                return default_value;
            } else {
                return std::invoke(method, GRAPHLE_FWD(args)...);
            }
        }
    };




    /** Utility to construct a search_visitor from a set of function objects. */
    template <
        graph_ref Graph,
        typename DiscoverVertex   = util::no_op_t,
        typename DiscoverLeaf     = util::no_op_t,
        typename DiscoverBranch   = util::no_op_t,
        typename DiscoverNewEdge  = util::no_op_t,
        typename DiscoverSeenEdge = util::no_op_t,
        typename BeginSearch      = util::no_op_t,
        typename FinishSearch     = util::no_op_t
    > struct visitor_from_arguments :
        search_visitor<
            visitor_from_arguments<Graph, DiscoverVertex, DiscoverLeaf, DiscoverBranch, DiscoverNewEdge, DiscoverSeenEdge, BeginSearch, FinishSearch>,
            Graph
        >
    {
        // Required for template argument deduction. See README for more info.
        meta::deduce_as_t<Graph> deduce_graph_type;

        // Note: can be initialized using aggregate initialization.
        DiscoverVertex discover_vertex;
        DiscoverLeaf discover_leaf;
        DiscoverBranch discover_branch;
        DiscoverNewEdge discover_new_edge;
        DiscoverSeenEdge discover_seen_edge;
        BeginSearch begin_search;
        FinishSearch finish_search;
    };




    /** Concept for classes implementing the CRTP-interface search_visitor. */
    template <typename T, typename G> concept search_visitor_type = std::is_base_of_v<search_visitor<T, std::remove_cvref_t<G>>, T>;
    /** Equivalent to search_visitor_type but with universal-reference-like behaviour. */
    template <typename T, typename G> concept search_visitor_ref = search_visitor_type<std::remove_cvref_t<T>, G>;
}
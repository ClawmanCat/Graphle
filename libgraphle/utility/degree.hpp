#pragma once

#include <common.hpp>
#include <graph/graph.hpp>
#include <meta/always_false.hpp>

#include <algorithm>


namespace graphle::util {
    /**
     * @ingroup Utils
     * Returns the in-degree of the given vertex (i.e. the number of edges going into the vertex).
     */
    template <graph_ref G> requires (
        in_edges_graph<G>                          || // Either provide in-edges
        (out_edges_graph<G> && !directed_graph<G>) || // Or out-edges on a non-directed graph
        edge_list_graph<G>                            // Or a list of all edges
    ) constexpr inline std::size_t in_degree(G&& graph, vertex_of<G> vertex) {
        using GL   = std::remove_cvref_t<G>;
        using edge = edge_of<G>;

        if constexpr (GL::has_in_edges) {
            return rng::size(graph.get_in_edges(vertex));
        }

        else if constexpr (GL::has_out_edges && !GL::is_directed) {
            return rng::size(graph.get_out_edges(vertex));
        }

        else if constexpr (GL::has_edge_list) {
            return rng::count(graph.get_edges() | views::transform(&edge::second), vertex);
        }
    }


    /**
     * @ingroup Utils
     * Returns the out-degree of the given vertex (i.e. the number of edges going out of the vertex).
     */
    template <graph_ref G> requires (
        out_edges_graph<G>                        || // Either provide out-edges
        (in_edges_graph<G> && !directed_graph<G>) || // Or in-edges on a non-directed graph
        edge_list_graph<G>                           // Or a list of all edges
    ) constexpr inline std::size_t out_degree(G&& graph, vertex_of<G> vertex) {
        using GL   = std::remove_cvref_t<G>;
        using edge = edge_of<G>;

        if constexpr (GL::has_out_edges) {
            return rng::size(graph.get_out_edges(vertex));
        }

        else if constexpr (GL::has_in_edges && !GL::is_directed) {
            return rng::size(graph.get_in_edges(vertex));
        }

        else if constexpr (GL::has_edge_list) {
            return rng::count(graph.get_edges() | views::transform(&edge::first), vertex);
        }
    }


    /**
     * @ingroup Utils
     * Returns whether or not the given vertex is a root vertex (i.e. a vertex with no ingoing edges).
     */
    template <directed_graph G> constexpr inline bool is_root(G&& graph, vertex_of<G> vertex) {
        return in_degree(graph, vertex) == 0;
    }


    /**
     * @ingroup Utils
     * Returns whether or not the given vertex is a branch vertex (i.e. a vertex with at least one outgoing edge).
     */
    template <directed_graph G> constexpr inline bool is_branch(G&& graph, vertex_of<G> vertex) {
        return out_degree(graph, vertex) > 0;
    }


    /**
     * @ingroup Utils
     * Returns whether or not the given vertex is a leaf vertex (i.e. a vertex with no outgoing edges).
     */
    template <directed_graph G> constexpr inline bool is_leaf(G&& graph, vertex_of<G> vertex) {
        return out_degree(graph, vertex) == 0;
    }
}
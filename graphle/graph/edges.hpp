#pragma once

#include <common.hpp>
#include <graph/graph.hpp>


namespace graphle {
    /**
     * @ingroup Graph
     * A directed graph that provides some method to get the ingoing edges of a vertex.
     */
    template <typename G> concept in_edge_provider  = directed_graph<G> && (in_edges_graph<G>  || edge_list_graph<G>);

    /**
     * @ingroup Graph
     * A directed graph that provides some method to get the outgoing edges of a vertex.
     */
    template <typename G> concept out_edge_provider = directed_graph<G> && (out_edges_graph<G> || edge_list_graph<G>);

    /**
     * @ingroup Graph
     * A graph that provides some method to get the edges of a vertex.
     */
    template <typename G> concept edge_provider =
        edge_list_graph<G> ||
        (non_directed_graph<G> && (in_edges_graph<G> || out_edges_graph<G>)) ||
        (directed_graph<G> && in_edges_graph<G> && out_edges_graph<G>);


    /**
     * @ingroup Graph
     * Returns a view of all in-edges of the given vertex. The graph must be directed.
     */
    template <in_edge_provider G> constexpr inline auto find_in_edges(G&& graph, vertex_of<G> vertex) {
        if constexpr (in_edges_graph<G>) {
            return graph.get_in_edges(vertex);
        } else {
            return graph.get_edges() | views::filter([vertex] (const auto& edge) { return edge.second == vertex; });
        }
    }


    /**
     * @ingroup Graph
     * Returns a view of all out-edges of the given vertex. The graph must be directed.
     */
    template <out_edge_provider G> constexpr inline auto find_out_edges(G&& graph, vertex_of<G> vertex) {
        if constexpr (out_edges_graph<G>) {
            return graph.get_out_edges(vertex);
        } else {
            return graph.get_edges() | views::filter([vertex] (const auto& edge) { return edge.first == vertex; });
        }
    }


    /**
     * @ingroup Graph
     * Returns a view of all edges of the given vertex.
     */
    template <edge_provider G> constexpr inline auto find_edges(G&& graph, vertex_of<G> vertex) {
        if constexpr (directed_graph<G> && (in_edges_graph<G> && out_edges_graph<G>)) {
            return views::join(find_in_edges(graph, vertex), find_out_edges(graph, vertex));
        }

        else if constexpr (non_directed_graph<G> && (in_edges_graph<G> || out_edges_graph<G>)) {
            if constexpr (in_edges_graph<G>) return graph.get_in_edges(vertex);
            else return graph.get_out_edges(vertex);
        }

        else return graph.get_edges() | views::filter([vertex] (const auto& edge) { return edge.first == vertex || edge.second == vertex; });
    }
}
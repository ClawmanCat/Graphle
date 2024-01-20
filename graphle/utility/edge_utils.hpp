#pragma once

#include <common.hpp>
#include <graph/graph.hpp>
#include <meta/concepts.hpp>
#include <utility/functional.hpp>

#include <algorithm>


namespace graphle::util {
    /** Filter that compares the given field of an edge to the provided vertex. @ingroup Utils */
    template <graph_ref G, auto Field> struct edge_field_filter {
        vertex_of<G> vertex;

        constexpr bool operator()(edge_of<G> edge) const {
            return vertex_compare_of<G>{}(edge.*Field, vertex);
        }
    };

    /** Filter that keeps all edges coming from the given vertex. @ingroup Utils */
    template <graph_ref G> using edge_from_filter = edge_field_filter<G, &edge_of<G>::first>;
    /** Filter that keeps all edges going to the given vertex. @ingroup Utils */
    template <graph_ref G> using edge_to_filter   = edge_field_filter<G, &edge_of<G>::second>;


    /**
     * @ingroup Utils
     * Returns a range of all in edges of the given vertex.
     * If the graph is non-directed, this is the same set of edges as the out edges.
     *
     * @graph_requires{
     *  edge_list_graph<G> ||
     *  in_edges_graph<G>  ||
     *  (non_directed_graph<G> && out_edges_graph<G>)
     * }
     */
    template <graph_ref G> requires (
        edge_list_graph<G> ||
        in_edges_graph<G>  ||
        (non_directed_graph<G> && out_edges_graph<G>)
    ) constexpr inline auto in_edges(G&& graph, vertex_of<G> vertex) {
        using GL   = std::remove_cvref_t<G>;
        using edge = edge_of<G>;

        if constexpr (GL::has_in_edges) {
            return graph.get_in_edges(vertex);
        }

        else if constexpr (GL::has_out_edges && !GL::is_directed) {
            return graph.get_out_edges(vertex) | views::transform(util::transpose_edge);
        }

        else if constexpr (GL::has_edge_list) {
            return graph.get_edges() | views::filter(edge_to_filter<G> { vertex });
        }
    }


    /**
     * @ingroup Utils
     * Returns a range of all out edges of the given vertex.
     * If the graph is non-directed, this is the same set of edges as the in edges.
     *
     * @graph_requires{
     *  edge_list_graph<G> ||
     *  out_edges_graph<G> ||
     *  (non_directed_graph<G> && in_edges_graph<G>)
     * }
     */
    template <graph_ref G> requires (
        edge_list_graph<G> ||
        out_edges_graph<G> ||
        (non_directed_graph<G> && in_edges_graph<G>)
    ) constexpr inline auto out_edges(G&& graph, vertex_of<G> vertex) {
        using GL   = std::remove_cvref_t<G>;
        using edge = edge_of<G>;

        if constexpr (GL::has_out_edges) {
            return graph.get_out_edges(vertex);
        }

        else if constexpr (GL::has_in_edges && !GL::is_directed) {
            return graph.get_in_edges(vertex) | views::transform(util::transpose_edge);
        }

        else if constexpr (GL::has_edge_list) {
            return graph.get_edges() | views::filter(edge_from_filter<G> { vertex });
        }
    }


    /**
     * @ingroup Utils
     * Returns the in-degree of the given vertex (i.e. the number of edges going into the vertex).
     * If the graph is non-directed, this is the same as the out-degree.
     *
     * @graph_requires{
     *  edge_list_graph<G> ||
     *  in_edges_graph<G>  ||
     *  (non_directed_graph<G> && out_edges_graph<G>)
     * }
     */
    template <graph_ref G> requires (
        edge_list_graph<G> ||
        in_edges_graph<G>  ||
        (non_directed_graph<G> && out_edges_graph<G>)
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
            return rng::distance(graph.get_edges() | views::filter(edge_to_filter<G> { vertex }));
        }
    }


    /**
     * @ingroup Utils
     * Returns the out-degree of the given vertex (i.e. the number of edges going out of the vertex).
     * If the graph is non-directed, this is the same as the in-degree.
     *
     * @graph_requires{
     *  edge_list_graph<G> ||
     *  out_edges_graph<G> ||
     *  (non_directed_graph<G> && in_edges_graph<G>)
     * }
     */
    template <graph_ref G> requires (
        edge_list_graph<G> ||
        out_edges_graph<G> ||
        (non_directed_graph<G> && in_edges_graph<G>)
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
            return rng::distance(graph.get_edges() | views::filter(edge_from_filter<G> { vertex }));
        }
    }


    /**
     * @ingroup Utils
     * Returns whether or not the given vertex is a root vertex (i.e. a vertex with no ingoing edges).
     *
     * @graph_requires{
     *  directed_graph<G> && (
     *      edge_list_graph<G> ||
     *      in_edges_graph<G>
     *  )
     * }
     */
    template <directed_graph G> requires (edge_list_graph<G> || in_edges_graph<G>)
    constexpr inline bool is_root(G&& graph, vertex_of<G> vertex) {
        return in_degree(graph, vertex) == 0;
    }


    /**
     * @ingroup Utils
     * Returns whether or not the given vertex is a branch vertex (i.e. a vertex with at least one outgoing edge).
     *
     * @graph_requires{
     *  directed_graph<G> && (
     *      edge_list_graph<G> ||
     *      out_edges_graph<G>
     *  )
     * }
     */
    template <directed_graph G> requires (edge_list_graph<G> || out_edges_graph<G>)
    constexpr inline bool is_branch(G&& graph, vertex_of<G> vertex) {
        return out_degree(graph, vertex) > 0;
    }


    /**
     * @ingroup Utils
     * Returns whether or not the given vertex is a leaf vertex (i.e. a vertex with no outgoing edges).
     *
     * @graph_requires{
     *  directed_graph<G> && (
     *      edge_list_graph<G> ||
     *      out_edges_graph<G>
     *  )
     * }
     */
    template <directed_graph G> requires (edge_list_graph<G> || out_edges_graph<G>)
    constexpr inline bool is_leaf(G&& graph, vertex_of<G> vertex) {
        return out_degree(graph, vertex) == 0;
    }
}
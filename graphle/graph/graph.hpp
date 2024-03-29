#pragma once

#include <common.hpp>
#include <meta/concepts.hpp>
#include <meta/value.hpp>
#include <graph/vertex_compare.hpp>
#include <graph/graph_concepts.hpp>
#include <graph/constraint_debug_helper.hpp>

#include <ranges>
#include <concepts>
#include <utility>


namespace graphle {
    /**
     * @ingroup Graph
     * Non-owning view of an existing data structure for treating it as a graph.
     *
     * @note:
     *  For directed graphs a pair [A, B] is considered an edge from A to B and is distinct from an edge [B, A] from B to A.
     *  For non-directed graphs, the two are considered equivalent and if the graph has a GetEdges method
     *  it should either provide an edge from A to B or from B to A, <b>but not both</b>.
     *
     * @note:
     *  If the graph is non-directed, the methods GetInEdges and GetOutEdges are considered equivalent, and either or both may be provided.
     *
     * @tparam Vertex      The type of vertex of this graph (The graph will use Vertex* as its vertex type).
     * @tparam IsDirected  True if an edge from vertex A to vertex B is the same as an edge from vertex B to vertex A
     *  (Parameter is passed as an integral constant, i.e. std::true_type or std::false_type).
     * @tparam GetVertices The type of a function object returning the vertices of this graph, or meta::none.
     * @tparam GetEdges    The type of a function object returning the edges of this graph, or meta::none.
     * @tparam GetOutEdges The type of a function object returning the outgoing edges of a vertex, or meta::none.
     * @tparam GetInEdges  The type of a function object returning the ingoing edges of a vertex, or meta::none.
     * @tparam CompareAs   A struct containing typedefs for comparators and hashers for vertices and edges.
     *  The default value compares and hashes vertices by their address, and uses the contained vertices to compare and hash edges.
     *  The comparators and hashers are always provided together, since they must be mutually consistent
     *  (E.g. if vertices are compared by value they should not be hashed by address).
     * @tparam Error       Will be set to an error from @ref graph_constraint_errors if one of the constraints is not satisfied.
     *  This error will show up in any error messages involving the graph's type.
     */
    template <
        typename Vertex,
        meta::value_wrapper_of<bool>     IsDirected  = std::true_type,
        maybe_vertex_getter<Vertex>      GetVertices = meta::none,
        maybe_edge_getter<Vertex>        GetEdges    = meta::none,
        maybe_vertex_edge_getter<Vertex> GetOutEdges = meta::none,
        maybe_vertex_edge_getter<Vertex> GetInEdges  = meta::none,
        graph_compare_traits<Vertex>     CompareAs   = compare_by_address<Vertex>,
        typename Error                               = decltype(detail::graph_constraints_check<Vertex, IsDirected, GetVertices, GetEdges, GetOutEdges, GetInEdges>())
    > struct graph {
        using vertex_type = Vertex*;
        using edge_type   = detail::edge_for<Vertex>;


        constexpr static inline bool is_directed     = IsDirected::value;
        constexpr static inline bool has_vertex_list = !meta::is_none_v<GetVertices>;
        constexpr static inline bool has_edge_list   = !meta::is_none_v<GetEdges>;
        constexpr static inline bool has_out_edges   = !meta::is_none_v<GetOutEdges>;
        constexpr static inline bool has_in_edges    = !meta::is_none_v<GetInEdges>;

        using get_vertices_t   = GetVertices;
        using get_edges_t      = GetEdges;
        using get_out_edges_t  = GetOutEdges;
        using get_in_edges_t   = GetInEdges;
        using vertex_compare_t = typename CompareAs::vertex_compare;
        using edge_compare_t   = typename CompareAs::edge_compare;
        using vertex_hash_t    = typename CompareAs::vertex_hash;
        using edge_hash_t      = typename CompareAs::edge_hash;


        // Required for template argument deduction. See README for more info.
        meta::deduce_as_t<Vertex> deduce_vertex_type;
        meta::deduce_as_t<IsDirected> deduce_is_directed;
        meta::deduce_as_t<CompareAs> deduce_compare_as;

        // Note: can be initialized using aggregate initialization / designated initializers.
        GetVertices get_vertices;
        GetEdges get_edges;
        GetOutEdges get_out_edges;
        GetInEdges get_in_edges;
    };


    /** Concept for template instantiations of graphle::graph. @ingroup Graph */
    template <typename G> concept graph_type = meta::is_template_v<graph, G>;
    /** Equivalent to graph_type but with universal-reference-like behaviour. @ingroup Graph */
    template <typename G> concept graph_ref  = graph_type<std::remove_cvref_t<G>>;

    /** The vertex type associated with the graph type G. @ingroup Graph */
    template <graph_ref G> using vertex_of = typename std::remove_cvref_t<G>::vertex_type;
    /** The edge type associated with the graph type G. @ingroup Graph */
    template <graph_ref G> using edge_of   = typename std::remove_cvref_t<G>::edge_type;
    /** The vertex comparator associated with the graph type G. @ingroup Graph */
    template <graph_ref G> using vertex_compare_of = typename std::remove_cvref_t<G>::vertex_compare_t;
    /** The edge comparator associated with the graph type G. @ingroup Graph */
    template <graph_ref G> using edge_compare_of = typename std::remove_cvref_t<G>::edge_compare_t;
    /** The vertex hasher associated with the graph type G. @ingroup Graph */
    template <graph_ref G> using vertex_hash_of = typename std::remove_cvref_t<G>::vertex_hash_t;
    /** The edge hasher associated with the graph type G. @ingroup Graph */
    template <graph_ref G> using edge_hash_of = typename std::remove_cvref_t<G>::edge_hash_t;


    /** Checks whether or not a graph is directed. @ingroup Graph */
    template <graph_ref G> constexpr inline bool graph_is_directed     = std::remove_cvref_t<G>::is_directed;
    /** Checks whether or not a graph provides a list of vertices. @ingroup Graph */
    template <graph_ref G> constexpr inline bool graph_has_vertex_list = std::remove_cvref_t<G>::has_vertex_list;
    /** Checks whether or not a graph provides a list of edges. @ingroup Graph */
    template <graph_ref G> constexpr inline bool graph_has_edge_list   = std::remove_cvref_t<G>::has_edge_list;
    /** Checks whether or not a graph provides a list of out edges for a vertex. @ingroup Graph */
    template <graph_ref G> constexpr inline bool graph_has_out_edges   = std::remove_cvref_t<G>::has_out_edges;
    /** Checks whether or not a graph provides a list of in edges for a vertex. @ingroup Graph */
    template <graph_ref G> constexpr inline bool graph_has_in_edges    = std::remove_cvref_t<G>::has_in_edges;

    /** Checks that a graph is directed. @ingroup Graph */
    template <typename G> concept directed_graph     = graph_is_directed<G>;
    /** Checks that a graph is not directed. @ingroup Graph */
    template <typename G> concept non_directed_graph = !graph_is_directed<G>;
    /** Checks whether or not a graph provides a list of vertices. @ingroup Graph */
    template <typename G> concept vertex_list_graph  = graph_has_vertex_list<G>;
    /** Checks whether or not a graph provides a list of edges. @ingroup Graph */
    template <typename G> concept edge_list_graph    = graph_has_edge_list<G>;
    /** Checks whether or not a graph provides a list of out edges for a vertex. @ingroup Graph */
    template <typename G> concept out_edges_graph    = graph_has_out_edges<G>;
    /** Checks whether or not a graph provides a list of in edges for a vertex. @ingroup Graph */
    template <typename G> concept in_edges_graph     = graph_has_in_edges<G>;


    /** Checks whether or not T is a valid vertex type. @ingroup Graph */
    template <typename T> constexpr static inline bool is_vertex = std::is_pointer_v<T>;
    /** Checks whether or not T is a valid edge type. @ingroup Graph */
    template <typename T> constexpr static inline bool is_edge   = meta::is_template_v<std::pair, T> && is_vertex<typename T::first_type> && is_vertex<typename T::second_type>;
}
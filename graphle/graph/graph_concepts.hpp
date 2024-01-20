#pragma once

#include <common.hpp>
#include <meta/value.hpp>

#include <ranges>
#include <concepts>
#include <utility>


/**
 * @def GRAPHLE_MAYBE_CONCEPT_1(Name)
 * Generates a wrapper concept for the given 1-argument concept that also matches meta::none.
 */
#define GRAPHLE_MAYBE_CONCEPT_1(Name) \
template <typename F, typename T1> concept maybe_##Name = graphle::meta::is_none_v<F> || Name<F, T1>

/**
 * @def GRAPHLE_MAYBE_CONCEPT_2(Name)
 * Generates a wrapper concept for the given 2-argument concept that also matches meta::none.
 */
#define GRAPHLE_MAYBE_CONCEPT_2(Name) \
template <typename F, typename T1, typename T2> concept maybe_##Name = graphle::meta::is_none_v<F> || Name<F, T1, T2>


namespace graphle {
    /**
     * @ingroup Graph
     * Concept for a function object that returns a range of vertex pointers.
     * @tparam F A function object type.
     * @tparam Vertex The vertex type.
     */
    template <typename F, typename Vertex, typename R = std::invoke_result_t<F>> concept vertex_getter =
        rng::sized_range<R> &&
        std::convertible_to<Vertex*, rng::range_value_t<R>>;


    /**
     * @ingroup Graph
     * Concept for a function object that returns a range of edges (Pairs of vertex pointers).
     * @tparam F A function object type.
     * @tparam Vertex The vertex type.
     */
    template <typename F, typename Vertex, typename R = std::invoke_result_t<F>> concept edge_getter =
        rng::sized_range<R> &&
        std::convertible_to<std::pair<Vertex*, Vertex*>, rng::range_value_t<R>>;


    /**
     * @ingroup Graph
     * Concept for a function object that accepts a pointer to a vertex and returns a range of edges (Pairs of vertex pointers).
     * @tparam F A function object type.
     * @tparam Vertex The vertex type.
     */
    template <typename F, typename Vertex, typename R = std::invoke_result_t<F, Vertex*>> concept vertex_edge_getter =
        rng::sized_range<R> &&
        std::convertible_to<std::pair<Vertex*, Vertex*>, rng::range_value_t<R>>;


    /**
     * @ingroup Graph
     * Concept for a function object that accepts a pointer to a vertex and returns a value for said vertex.
     * @tparam F A function object type.
     * @tparam Vertex The vertex type.
     * @tparam Value The type of value returned by F.
     */
    template <typename F, typename Vertex, typename Value> concept vertex_value_getter =
        std::convertible_to<Value, std::invoke_result_t<F, Vertex*>>;


    /**
     * @ingroup Graph
     * Concept for a function object that accepts an edge (A pair of pointers to vertices) and returns a value for said edge.
     * @tparam F A function object type.
     * @tparam Vertex The vertex type.
     * @tparam Value The type of value returned by F.
     */
    template <typename F, typename Vertex, typename Value> concept edge_value_getter =
        std::convertible_to<Value, std::invoke_result_t<F, std::pair<Vertex*, Vertex*>>>;


    /**
     * @ingroup Graph
     * Concept for a function object that returns a value of the given type for a graph.
     * @tparam F A function object type.
     * @tparam Value The type of value returned by F.
     */
    template <typename F, typename Value> concept graph_value_getter =
        std::convertible_to<Value, std::invoke_result_t<F>>;


    // A type either matching the provided concept or equal to meta::none.
    GRAPHLE_MAYBE_CONCEPT_1(vertex_getter);
    GRAPHLE_MAYBE_CONCEPT_1(edge_getter);
    GRAPHLE_MAYBE_CONCEPT_1(vertex_edge_getter);
    GRAPHLE_MAYBE_CONCEPT_2(vertex_value_getter);
    GRAPHLE_MAYBE_CONCEPT_2(edge_value_getter);
    GRAPHLE_MAYBE_CONCEPT_1(graph_value_getter);
}
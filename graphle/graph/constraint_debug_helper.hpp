#pragma once

#include <graph/graph_concepts.hpp>
#include <meta/value.hpp>


namespace graphle::detail {
    /** @defgroup graph_constraint_errors Graph Constraint Error Types */
    
    
    /** No error. @ingroup graph_constraint_errors */
    struct all_constraints_satisfied {};
    /** Invalid value for parameter IsDirected. @ingroup graph_constraint_errors */
    template <typename T> struct invalid_is_directed_value {};
    /** Invalid value for parameter GetVertices. @ingroup graph_constraint_errors */
    template <typename Vertex, typename Get> struct invalid_vertex_getter {};
    /** Invalid value for parameter GetEdges. @ingroup graph_constraint_errors */
    template <typename Vertex, typename Get> struct invalid_edge_getter {};
    /** Invalid value for parameter GetInEdges. @ingroup graph_constraint_errors */
    template <typename Vertex, typename Get> struct invalid_in_edge_getter {};
    /** Invalid value for parameter GetOutEdges. @ingroup graph_constraint_errors */
    template <typename Vertex, typename Get> struct invalid_out_edge_getter {};


    /**
     * @ingroup Config
     * @def GRAPHLE_NO_STATIC_ASSERT
     * Using a static_assert for constraint checks will usually give better error messages,
     * but may cause some issues when trying to do metaprogramming. These asserts can be turned off with GRAPHLE_NO_STATIC_ASSERT.
     */
    #ifdef GRAPHLE_DOXYGEN
        #define GRAPHLE_NO_STATIC_ASSERT "Not Defined"
    #endif


    /**
     * @ingroup graph_constraint_errors
     * 
     * Checks the constraints for graph types and returns an error type if said constraints are not fulfilled.
     * This is used to make the error part of the graph's type, as the compiler can't be bothered to say which constraint is not satisfied.
     *
     * Unless @ref GRAPHLE_NO_STATIC_ASSERT is defined, this method will trigger a static_assert if one of the constraint checks fails.
     *
     * @return An instance of one of the following:
     *  all_constraints_satisfied
     *  invalid_is_directed_value<T>
     *  invalid_vertex_getter<Vertex, Getter>,
     *  invalid_edge_getter<Vertex, Getter>,
     *  invalid_in_edge_getter<Vertex, Getter>,
     *  invalid_out_edge_getter<Vertex, Getter>
     *
     *  @todo: While using the static_assert works for Clang, MSVC still refuses to actually tell us the full reason for the constraint failure here.
     *    Might be fixable by moving these asserts elsewhere because there are situations where it does give the full failure reason.
     */
    template <
        typename Vertex,
        typename IsDirected,
        typename GetVertices,
        typename GetEdges,
        typename GetOutEdges,
        typename GetInEdges
    > consteval auto graph_constraints_check(void) {
        #ifndef GRAPHLE_NO_STATIC_ASSERT
            static_assert(meta::value_wrapper_of<IsDirected, bool>);
            static_assert(maybe_vertex_getter<GetVertices, Vertex>);
            static_assert(maybe_edge_getter<GetEdges, Vertex>);
            static_assert(maybe_vertex_edge_getter<GetInEdges, Vertex>);
            static_assert(maybe_vertex_edge_getter<GetOutEdges, Vertex>);
        #endif


        if constexpr (!meta::value_wrapper_of<IsDirected, bool>) {
            return invalid_is_directed_value<IsDirected> {};
        }

        else if constexpr (!maybe_vertex_getter<GetVertices, Vertex>) {
            return invalid_vertex_getter<Vertex, GetVertices> {};
        }

        else if constexpr (!maybe_edge_getter<GetEdges, Vertex>) {
            return invalid_edge_getter<Vertex, GetEdges> {};
        }

        else if constexpr (!maybe_vertex_edge_getter<GetInEdges, Vertex>) {
            return invalid_in_edge_getter<Vertex, GetInEdges> {};
        }

        else if constexpr (!maybe_vertex_edge_getter<GetOutEdges, Vertex>) {
            return invalid_out_edge_getter<Vertex, GetOutEdges> {};
        }
        
        else return all_constraints_satisfied {};
    }
}
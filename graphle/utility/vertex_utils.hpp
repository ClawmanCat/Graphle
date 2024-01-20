#pragma once

#include <common.hpp>
#include <graph/graph.hpp>


namespace graphle::util {
    /**
     * Finds the first vertex matching the given predicate, or nullptr if no such vertex exists.
     *
     * @param graph The graph to search
     * @param pred A function object invocable as pred(vertex_of<G>) -> bool.
     * @return Either a vertex matching the given predicate or nullptr.
     *
     * @graph_requires{vertex_list_graph<G>}
     */
    template <graph_ref G, typename F>
    constexpr inline vertex_of<G> find_vertex(G&& graph, F&& pred) {
        for (auto* vertex : graph.get_vertices()) {
            if (std::invoke(pred, vertex)) return vertex;
        }

        return nullptr;
    }
}
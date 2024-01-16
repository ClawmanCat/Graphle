#pragma once

#include <common.hpp>
#include <graph/graph.hpp>
#include <graph/edges.hpp>
#include <search/search_impl.hpp>
#include <storage/storage_provider.hpp>
#include <storage/default_storage_provider.hpp>
#include <utility/detail/storage_utils.hpp>


namespace graphle::search {
    /**
     * @ingroup Search
     * Performs a depth first search (DFS) on the given graph, invoking the appropriate callbacks on the provided visitor.
     *
     * @param graph A graphle::graph to visit.
     * @param root The root vertex to start the depth first search from.
     * @param visitor A visitor implementing the graphle::search_visitor interface.
     * @param stack_provider An optional storage-provider which can provide a vector-like type for the algorithm to use.
     * @param set_provider An optional storage-provider which can provide a unordered-set-like type for the algorithm to use.
     * @return True if the algorithm finished normally or false if the visitor caused the algorithm to return early.
     */
    template <
        graph_ref G,
        search_visitor_ref<G> V,
        store::storage_provider_ref<store::storage_type::VECTOR, vertex_of<G>> PV
            = store::default_provided_t<store::storage_type::VECTOR, vertex_of<G>>,
        store::storage_provider_ref<store::storage_type::UNORDERED_SET, vertex_of<G>> PS
            = store::default_provided_t<store::storage_type::UNORDERED_SET, vertex_of<G>>
    >
    requires (edge_provider<G> || out_edge_provider<G>)
    constexpr inline bool depth_first_search(
        G&& graph,
        vertex_of<G> root,
        V&& visitor,
        PV&& stack_provider = store::get_default_storage_provider<store::storage_type::VECTOR, vertex_of<G>>(),
        PS&& set_provider   = store::get_default_storage_provider<store::storage_type::UNORDERED_SET, vertex_of<G>>()
    ) {
        return detail::search<store::storage_type::VECTOR>(
            graph,
            root,
            visitor,
            [] (auto& stack, auto elem) { stack.push_back(elem); },
            [] (auto& stack) { return util::take_back(stack); },
            GRAPHLE_FWD(stack_provider),
            GRAPHLE_FWD(set_provider)
        );
    }
}
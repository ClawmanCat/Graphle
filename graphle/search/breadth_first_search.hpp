#pragma once

#include <common.hpp>
#include <graph/graph.hpp>
#include <search/search_impl.hpp>
#include <storage/storage_provider.hpp>
#include <storage/default_storage_provider.hpp>
#include <utility/storage_utils.hpp>


namespace graphle::search {
    /**
     * @ingroup Search
     * Performs a breadth first search (BFS) on the given graph, invoking the appropriate callbacks on the provided visitor.
     *
     * @param graph A graphle::graph to visit.
     * @param root The root vertex to start the depth first search from.
     * @param visitor A visitor implementing the graphle::search_visitor interface.
     * @param deque_provider An optional storage-provider which can provide a deque-like type for the algorithm to use.
     * @param set_provider An optional storage-provider which can provide a unordered-set-like type for the algorithm to use.
     * @return True if the algorithm finished normally or false if the visitor caused the algorithm to return early.
     *
     * @graph_requires{
     *  edge_list_graph<G> ||
     *  out_edges_graph<G> ||
     *  (non_directed_graph<G> && in_edges_graph<G>)
     * }
     */
    template <
        graph_ref G,
        search_visitor_ref<G> V,
        store::storage_provider_ref<store::storage_type::DEQUE, vertex_of_t<G>> PQ
            = store::default_provider_t<store::storage_type::DEQUE, vertex_of_t<G>>,
        store::storage_provider_ref<store::storage_type::UNORDERED_SET, vertex_of_t<G>, vertex_hash_of_t<G>, vertex_compare_of_t<G>> PS
            = store::default_provider_t<store::storage_type::UNORDERED_SET, vertex_of_t<G>, vertex_hash_of_t<G>, vertex_compare_of_t<G>>
    > requires (
        edge_list_graph<G> ||
        out_edges_graph<G> ||
        (non_directed_graph<G> && in_edges_graph<G>)
    ) constexpr inline bool breadth_first_search(
        G&& graph,
        vertex_of_t<G> root,
        V&& visitor,
        PQ&& deque_provider = store::get_default_storage_provider<store::storage_type::DEQUE, vertex_of_t<G>>(),
        PS&& set_provider   = store::get_default_storage_provider<store::storage_type::UNORDERED_SET, vertex_of_t<G>, vertex_hash_of_t<G>, vertex_compare_of_t<G>>()
    ) {
        return detail::search<store::storage_type::DEQUE>(
            graph,
            root,
            visitor,
            [] (auto& queue, auto elem) { queue.push_back(elem); },
            [] (auto& queue) { return util::take_front(queue); },
            GRAPHLE_FWD(deque_provider),
            GRAPHLE_FWD(set_provider)
        );
    }
}
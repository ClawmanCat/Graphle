#pragma once

#include <common.hpp>
#include <graph/graph.hpp>
#include <graph/graph_concepts.hpp>
#include <graph/edges.hpp>
#include <storage/storage_provider.hpp>
#include <storage/default_storage_provider.hpp>
#include <utility/degree.hpp>
#include <utility/detail/storage_utils.hpp>
#include <meta/if_constexpr.hpp>
#include <views/edge_perspective.hpp>
#include <search/visitor.hpp>


namespace graphle::detail {
    /**
     * @ingroup Search
     * Common implementation for the BFS and DFS search algorithms, since their implementation is the same,
     * just using a queue vs. a stack to keep track of the pending elements.
     *
     * @tparam ST The storage type used for the pending vertex list (DEQUE for BFS, VECTOR for DFS).
     * @param graph A graphle::graph to visit.
     * @param root The root vertex to start the depth first search from.
     * @param visitor A visitor implementing the graphle::search_visitor interface.
     * @param emplace An object invocable as emplace(pending, element) to emplace the given element into the pending vertex list.
     * @param take An object invocable as take(pending) to take an element from the pending vertex list.
     * @param pending_provider An optional storage-provider which can provide storage of type ST for the algorithm to use.
     * @param set_provider An optional storage-provider which can provide a unordered-set-like type for the algorithm to use.
     * @return True if the algorithm finished normally or false if the visitor caused the algorithm to return early.
     */
    template <
        store::storage_type ST,
        graph_ref G,
        search::search_visitor_ref<G> V,
        typename EmplacePP,
        typename TakePP,
        store::storage_provider_ref<ST, vertex_of<G>> PP
            = store::default_provided_t<ST, vertex_of<G>>,
        store::storage_provider_ref<store::storage_type::UNORDERED_SET, vertex_of<G>> PS
            = store::default_provided_t<store::storage_type::UNORDERED_SET, vertex_of<G>>
    > constexpr inline bool search(
        G&& graph,
        vertex_of<G> root,
        V&& visitor,
        EmplacePP&& emplace,
        TakePP&& take,
        PP&& pending_provider = store::get_default_storage_provider<ST, vertex_of<G>>(),
        PS&& set_provider     = store::get_default_storage_provider<store::storage_type::UNORDERED_SET, vertex_of<G>>()
    ) {
        using VR  = search::visitor_result;
        using NVR = search::nonlocal_visitor_result;


        if (auto result = visitor.begin_search_base(graph); result == NVR::STOP_SEARCH) return false;


        decltype(auto) pending = pending_provider();
        emplace(pending, root);

        decltype(auto) seen = set_provider();
        seen.emplace(root);

        bool root_visited = false;


        while (!rng::empty(pending)) {
            vertex_of<G> next = take(pending);


            switch (visitor.discover_vertex_base(next, graph)) {
                case VR::STOP_SEARCH: return false;
                case VR::STOP_TREE:   continue;
                case VR::CONTINUE:    break;
            }

            // Concepts 'branch' and 'leaf' don't make sense for a non-directed graph so ignore these visitor callbacks.
            if constexpr (graph_is_directed<G>) {
                if (util::is_branch(graph, next)) {
                    switch (visitor.discover_branch_base(next, graph)) {
                        case VR::STOP_SEARCH: return false;
                        case VR::STOP_TREE:   continue;
                        case VR::CONTINUE:    break;
                    }
                }

                if (util::is_leaf(graph, next)) {
                    switch (visitor.discover_leaf_base(next, graph)) {
                        case VR::STOP_SEARCH: return false;
                        case VR::STOP_TREE:   continue;
                        case VR::CONTINUE:    break;
                    }
                }
            }


            auto edges = [&] {
                if constexpr (graph_is_directed<G>) return find_out_edges(graph, next);
                else return find_edges(graph, next) | views::from_vertex_perspective(next);
            } ();


            for (auto edge : edges) {
                if (seen.contains(edge.second)) {
                    switch (visitor.discover_edge_to_known_vertex_base(edge, graph)) {
                        case VR::STOP_SEARCH: return false;
                        case VR::STOP_TREE:   continue;
                        case VR::CONTINUE:    continue;
                    }
                } else {
                    switch (visitor.discover_edge_to_new_vertex_base(edge, graph)) {
                        case VR::STOP_SEARCH: return false;
                        case VR::STOP_TREE:   continue;
                        case VR::CONTINUE:
                            seen.emplace(edge.second);
                            emplace(pending, edge.second);
                            break;
                    }
                }
            }
        }


        if (auto result = visitor.finish_search_base(graph); result == NVR::STOP_SEARCH) return false;
        return true;
    }
}
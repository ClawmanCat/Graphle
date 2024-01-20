#pragma once

#include <common.hpp>
#include <graph/graph.hpp>
#include <search/depth_first_search.hpp>
#include <storage/storage_provider.hpp>
#include <storage/default_storage_provider.hpp>
#include <storage/storage_provider_helpers.hpp>
#include <utility/storage_utils.hpp>
#include <utility/vec_of_vecs_output_iterator.hpp>

#include <concepts>
#include <iterator>
#include <limits>


namespace graphle::alg {
    namespace detail {
        template <graph_ref G> using out_edge_range_t = decltype(util::out_edges(std::declval<G>(), std::declval<vertex_of<G>>()));


        template <graph_ref G> struct tarjan_vertex_data {
            tarjan_vertex_data(std::size_t index, G& graph, vertex_of<G> vertex) :
                index(index),
                low_link(index),
                stacked(true),
                edges(util::out_edges(graph, vertex)),
                edge_iterator(rng::begin(edges))
            {}


            std::size_t index;
            std::size_t low_link;
            bool stacked;

            out_edge_range_t<G> edges;
            rng::iterator_t<out_edge_range_t<G>> edge_iterator;
        };
    }


    /**
     * @ingroup Alg
     *
     * Finds all strongly connected components (cycles) in the given directed graph using Tarjan's algorithm
     * (Tarjan, R. (1972). Depth-First Search and Linear Graph Algorithms. SIAM Journal on Computing, 1(2), 146–160. doi:10.1137/0201010).
     * This non-recursive version of the algorithm is based on the implementation by Thomas Ahle:
     * https://stackoverflow.com/a/62006383
     *
     * This overload outputs its data into the provided output iterator.
     *
     * @param graph A graphle::graph to find the strongly connected components of.
     * @param output An output iterator with a value type that is itself an output iterator, into which vertices can be pushed.
     * @param min_size Strongly connected components with a cycle length smaller than this value will be discarded.
     *  Typically, when e.g. searching for cyclic dependencies, you will want to discard all size-1 cycles,
     *  since these indicate vertices that do not have any dependencies at all.
     * @param stack_provider An optional storage-provider which can provide a vector-like type for the algorithm to use.
     * @param min_stack_provider An optional storage-provider which can provide a vector-like type for the algorithm to use.
     * @param map_provider An optional storage-provider which can provide a unordered-map-like type for the algorithm to use.
     *
     * @graph_requires{
     *  directed_graph<G>    &&
     *  vertex_list_graph<G> &&
     *  (
     *      edge_list_graph<G> ||
     *      out_edges_graph<G>
     *  )
     * }
     */
    template <
        directed_graph G,
        typename Target,
        store::storage_provider_ref<store::storage_type::VECTOR, vertex_of<G>> PV
            = store::default_provided_t<store::storage_type::VECTOR, vertex_of<G>>,
        store::storage_provider_ref<store::storage_type::VECTOR, vertex_of<G>> PVM
            = store::default_provided_t<store::storage_type::VECTOR, vertex_of<G>>,
        store::storage_provider_ref<store::storage_type::UNORDERED_MAP, vertex_of<G>, detail::tarjan_vertex_data<G>, vertex_hash_of<G>, vertex_compare_of<G>> PM
            = store::default_provided_t<store::storage_type::UNORDERED_MAP, vertex_of<G>, detail::tarjan_vertex_data<G>, vertex_hash_of<G>, vertex_compare_of<G>>
    > requires (
        vertex_list_graph<G> &&
        (edge_list_graph<G> || out_edges_graph<G>) &&
        std::output_iterator<std::remove_cvref_t<Target>, typename std::remove_cvref_t<Target>::value_type> &&
        std::output_iterator<typename std::remove_cvref_t<Target>::value_type, vertex_of<G>>
    ) constexpr inline void strongly_connected_components(
        G&& graph,
        Target&& target,
        std::size_t min_size     = 0,
        PV&&  stack_provider     = store::get_default_storage_provider<store::storage_type::VECTOR, vertex_of<G>>(),
        PVM&& min_stack_provider = store::get_default_storage_provider<store::storage_type::VECTOR, vertex_of<G>>(),
        PM&&  map_provider       = store::get_default_storage_provider<store::storage_type::UNORDERED_MAP, vertex_of<G>, detail::tarjan_vertex_data<G>, vertex_hash_of<G>, vertex_compare_of<G>>()
    ) {
        decltype(auto) call_stack = stack_provider();
        decltype(auto) low_stack  = min_stack_provider();
        decltype(auto) data       = map_provider();
        std::size_t index         = 0;


        for (auto vertex : graph.get_vertices()) {
            if (!data.contains(vertex)) {
                call_stack.push_back(vertex);


                while (!rng::empty(call_stack)) {
                    auto v = util::take_back(call_stack);

                    if (!data.contains(v)) {
                        data.emplace(v, detail::tarjan_vertex_data<G> { index++, graph, v });
                        low_stack.push_back(v);
                    } else {
                        auto& vd = data.at(v);
                        vd.low_link = std::min(vd.low_link, data.at(vd.edge_iterator->second).low_link);
                    }


                    auto& vd = data.at(v);
                    auto& it = vd.edge_iterator;

                    for (/* no init */; it != vd.edges.end() && data.contains(it->second); ++it) {
                        auto  w  = it->second;
                        auto& wd = data.at(w);

                        if (wd.stacked) vd.low_link = std::min(vd.low_link, wd.index);
                    }


                    if (it != vd.edges.end()) {
                        call_stack.push_back(v);
                        call_stack.push_back(it->second);

                        ++it;
                        continue;
                    }


                    if (vd.low_link == vd.index) {
                        // Output iterators are not guaranteed to be dereferencable multiple times for the same element,
                        // so only dereference it if we're actually going to output an SCC.
                        const bool should_output = min_size < 2 || [&] {
                            std::size_t count = 0;

                            for (auto w : low_stack | views::reverse) {
                                if (!vertex_compare_of<G>(v, w)) ++count;
                                else break;
                            }

                            return count >= min_size;
                        } ();


                        vertex_of<G> w;

                        auto unstack = [&] {
                            w = util::take_back(low_stack);
                            data.at(w).stacked = false;
                            return w;
                        };


                        if (should_output) {
                            decltype(auto) scc_target = *target;

                            do *scc_target++ = unstack();
                            while (!vertex_compare_of<G>(v, w));
                        } else {
                            do unstack();
                            while (!vertex_compare_of<G>(v, w));
                        }
                    }
                }
            }
        }
    }


    /**
     * @ingroup Alg
     *
     * Finds all strongly connected components (cycles) in the given directed graph using Tarjan's algorithm
     * (Tarjan, R. (1972). Depth-First Search and Linear Graph Algorithms. SIAM Journal on Computing, 1(2), 146–160. doi:10.1137/0201010).
     * This non-recursive version of the algorithm is based on the implementation by Thomas Ahle:
     * https://stackoverflow.com/a/62006383
     *
     * This overload returns its data as a Vector<Vector<Vertex>> (See return type).
     *
     * @param graph A graphle::graph to find the strongly connected components of.
     * @param min_size Strongly connected components with a cycle length smaller than this value will be discarded.
     *  Typically, when e.g. searching for cyclic dependencies, you will want to discard all size-1 cycles,
     *  since these indicate vertices that do not have any dependencies at all.
     * @param inner_ret_provider An optional storage-provider which can provide a vector-like type for the algorithm to use. Used for the returned value (See return type).
     * @param outer_ret_provider An optional storage-provider which can provide a vector-like type for the algorithm to use. Used for the returned value (See return type).
     * @param stack_provider An optional storage-provider which can provide a vector-like type for the algorithm to use.
     * @param min_stack_provider An optional storage-provider which can provide a vector-like type for the algorithm to use.
     * @param map_provider An optional storage-provider which can provide a unordered-map-like type for the algorithm to use.
     * @return Returns a VectorOuter<VectorInner<Vertex>>,
     *  where VectorOuter is the storage type provided by OuterPR (std::vector by default)
     *  where VectorInner is the storage type provided by InnerPR (std::vector by default)
     *  The value category of the returned value is equal to that of the outer_ret_provider,
     *  i.e. if outer_ret_provider() returns a reference, this function returns a reference also.
     *
     * @graph_requires{
     *  directed_graph<G>    &&
     *  vertex_list_graph<G> &&
     *  (
     *      edge_list_graph<G> ||
     *      out_edges_graph<G>
     *  )
     * }
     */
    template <
        directed_graph G,
        store::storage_provider_ref<store::storage_type::VECTOR, vertex_of<G>> InnerPR
            = store::default_provided_t<store::storage_type::VECTOR, vertex_of<G>>,
        store::storage_provider_ref<store::storage_type::VECTOR, store::provided_storage_value_type<InnerPR>> OuterPR
            = store::default_provided_t<store::storage_type::VECTOR, store::provided_storage_value_type<InnerPR>>,
        store::storage_provider_ref<store::storage_type::VECTOR, vertex_of<G>> PV
            = store::default_provided_t<store::storage_type::VECTOR, vertex_of<G>>,
        store::storage_provider_ref<store::storage_type::VECTOR, vertex_of<G>> PVM
            = store::default_provided_t<store::storage_type::VECTOR, vertex_of<G>>,
        store::storage_provider_ref<store::storage_type::UNORDERED_MAP, vertex_of<G>, detail::tarjan_vertex_data<G>, vertex_hash_of<G>, vertex_compare_of<G>> PM
            = store::default_provided_t<store::storage_type::UNORDERED_MAP, vertex_of<G>, detail::tarjan_vertex_data<G>, vertex_hash_of<G>, vertex_compare_of<G>>
    > requires (
        vertex_list_graph<G> &&
        (edge_list_graph<G> || out_edges_graph<G>)
    ) constexpr inline decltype(auto) strongly_connected_components(
        G&& graph,
        std::size_t min_size                          = 0,
        GRAPHLE_MULTIPLE InnerPR&& inner_ret_provider = store::get_default_storage_provider<store::storage_type::VECTOR, vertex_of<G>>(),
        OuterPR&& outer_ret_provider                  = store::get_default_storage_provider<store::storage_type::VECTOR, store::provided_storage_value_type<InnerPR>>(),
        PV&&  stack_provider                          = store::get_default_storage_provider<store::storage_type::VECTOR, vertex_of<G>>(),
        PVM&& min_stack_provider                      = store::get_default_storage_provider<store::storage_type::VECTOR, vertex_of<G>>(),
        PM&&  map_provider                            = store::get_default_storage_provider<store::storage_type::UNORDERED_MAP, vertex_of<G>, detail::tarjan_vertex_data<G>, vertex_hash_of<G>, vertex_compare_of<G>>()
    ) {
        decltype(auto) result = outer_ret_provider();

        strongly_connected_components(
            graph,
            util::vec_of_vecs_output_iterator { result, inner_ret_provider },
            min_size,
            GRAPHLE_FWD(stack_provider),
            GRAPHLE_FWD(min_stack_provider),
            GRAPHLE_FWD(map_provider)
        );

        return result;
    }
}
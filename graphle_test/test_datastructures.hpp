#pragma once

#include <graphle.hpp>

#include <vector>
#include <utility>
#include <memory>
#include <unordered_set>
#include <unordered_map>


/**
 * @defgroup TestGraphs Graphle Test Data Structures
 * @todo Add datastructures for non-directed graphs.
 */


/**
 * @ingroup TestGraphs
 * Hasher for test data vertices.
 */
template <typename T> requires requires (T v) { v.vertex_id; }
struct std::hash<T> {
    constexpr std::size_t operator()(const T& v) const {
        return std::hash<std::size_t>{}(v.vertex_id);
    }
};


namespace graphle::test {
    /**
     * @ingroup TestGraphs
     * Equality for test data vertices.
     */
    template <typename T> requires requires (T v) { v.vertex_id; }
    constexpr bool operator==(const T& a, const T& b) {
        return a.vertex_id == b.vertex_id;
    }


    /**
     * @ingroup TestGraphs
     * Conversion between different graph vertex types.
     */
    GRAPHLE_MAKE_NIEBLOID(to_vertex, [] (const auto& v) { return V { v.vertex_id }; }, typename V);

    /**
     * @ingroup TestGraphs
     * Conversion between different graph edge types.
     */
    GRAPHLE_MAKE_NIEBLOID(to_edge, ([] (const auto& e) { return std::pair { to_vertex<V>(e.first), to_vertex<V>(e.second) }; }), typename V);


    /**
     * Returns the projected value of the container 'container' at key 'key' if it exists,
     * or the empty range (projection(container.begin())->end(), projection(container.begin())->end()) otherwise.
     * Dumb workaround for some of the test containers that use [set|map].at() to get the edges for a vertex,
     * but that do not have a key for every vertex.
     * This will fail if the provided container is empty, but it never is for our test datastructures.
     * @param container A set- or map-like container.
     * @param key The key to find.
     * @param projection A projection from the containers value type to the stored range type.
     * @return The range projection(*container.at(key)) if the container has the given key or an empty range with the same type otherwise.
     */
    template <typename C, typename K, typename P> constexpr inline auto range_at_key_or_dummy(C& container, const K& key, P&& projection = std::identity {}) {
        auto it = container.find(key);


        if (it == container.end()) {
            auto& dummy = std::invoke(projection, container.begin());
            return std::ranges::subrange(dummy.end(), dummy.end());
        } else {
            auto& range = std::invoke(projection, *it);
            return std::ranges::subrange(range.begin(), range.end());
        }
    }


    /** Wrapper for @ref range_at_key_or_dummy for map-like types. */
    template <typename M, typename K> constexpr inline auto range_at_map_key_or_dummy(M& map, const K& key) {
        return range_at_key_or_dummy(map, key, &std::ranges::range_value_t<M>::second);
    }


    /**
     * @ingroup TestGraphs
     * Graph of the form G { vertices = V[], edges = E[] }
     */
    struct ve_list_graph {
        struct vertex { std::size_t vertex_id; };

        std::vector<vertex> vertices;
        std::vector<std::pair<vertex, vertex>> edges;


        constexpr static ve_list_graph from_ve_list(const ve_list_graph& g) {
            return g;
        }

        constexpr auto view_as_graph(void) {
            return graph {
                .deduce_vertex_type = graphle::meta::deduce_as<vertex>,
                .deduce_compare_as  = graphle::meta::deduce_as<graphle::compare_by_value<vertex>>,
                .get_vertices       = [this] { return views::all(vertices) | views::transform(util::addressof); },
                .get_edges          = [this] { return views::all(edges) | views::transform(util::transform_edge(util::addressof)); }
            };
        }
    };


    /**
     * @ingroup TestGraphs
     * Graph of the form G { vertices = Set<V>, edges = Map<V, V[]> }
     */
    struct ve_map_graph {
        struct vertex {
            std::size_t vertex_id;
        };

        std::unordered_set<vertex> vertices;
        std::unordered_map<vertex, std::vector<vertex>> edges;


        static ve_map_graph from_ve_list(const ve_list_graph& g) {
            auto vs = g.vertices | std::views::transform(to_vertex<vertex>);
            auto es = g.edges    | std::views::transform(to_edge<vertex>);


            auto result = ve_map_graph {
                .vertices { vs.begin(), vs.end() }
            };

            for (const auto& e : es) result.edges[e.first].push_back(e.second);


            return result;
        }


        constexpr auto view_as_graph(void) {
            return graph {
                .deduce_vertex_type = meta::deduce_as<const vertex>,
                .deduce_compare_as  = graphle::meta::deduce_as<graphle::compare_by_value<vertex>>,
                .get_vertices       = [this] { return views::all(vertices) | views::transform(util::addressof); },
                .get_out_edges      = [this] (const vertex* v) { return range_at_map_key_or_dummy(edges, *v) | views::transform(util::as_const) | views::transform(util::addressof) | views::edge_from(v); }
            };
        }
    };


    /**
     * @ingroup TestGraphs
     * Graph of the form G { root = V }, V { edges = V*[] }
     */
    struct out_edge_graph {
        // This is bad performance-wise but this type is only used for testing.
        struct vertex { std::size_t vertex_id; std::vector<std::shared_ptr<vertex>> out; };
        std::shared_ptr<vertex> root;


        static out_edge_graph from_ve_list(const ve_list_graph& g, std::size_t root = 0) {
            std::unordered_map<std::size_t, std::shared_ptr<vertex>> vertices;

            for (const auto& v : g.vertices) {
                vertices.emplace(v.vertex_id, std::make_shared<vertex>(v.vertex_id));
            }

            for (const auto& e : g.edges) {
                auto& from = vertices.at(e.first.vertex_id);
                auto& to   = vertices.at(e.second.vertex_id);

                from->out.push_back(to);
            }

            return out_edge_graph { .root = vertices.at(root) };
        }


        constexpr auto view_as_graph(void) {
            return graph {
                .deduce_vertex_type = graphle::meta::deduce_as<vertex>,
                .deduce_compare_as  = graphle::meta::deduce_as<graphle::compare_by_value<vertex>>,
                .get_out_edges = [] (vertex* v) {
                    return views::all(v->out)
                        | views::transform(&std::shared_ptr<vertex>::get)
                        | views::edge_from(v);
                }
            };
        }
    };


    /**
     * @ingroup TestGraphs
     * Graph of the form G { vertices = V[] }, V { edges = V*[] }
     */
    struct v_list_out_edge_graph {
        struct vertex { std::size_t vertex_id; std::vector<vertex*> out; };
        std::vector<vertex> vertices;


        static v_list_out_edge_graph from_ve_list(const ve_list_graph& g) {
            auto vs = g.vertices | std::views::transform(to_vertex<vertex>);
            v_list_out_edge_graph result { .vertices { vs.begin(), vs.end() } };
            std::unordered_map<std::size_t, vertex*> vertex_map;

            for (const auto [from, to] : g.edges) {
                for (auto v : { from, to }) {
                    if (!vertex_map.contains(v.vertex_id)) {
                        vertex_map.emplace(v.vertex_id, &*std::ranges::find(result.vertices, vertex { v.vertex_id }));
                    }
                }

                vertex_map.at(from.vertex_id)->out.push_back(vertex_map.at(to.vertex_id));
            }

            return result;
        }


        constexpr auto view_as_graph(void) {
            return graph {
                .deduce_vertex_type = graphle::meta::deduce_as<vertex>,
                .deduce_compare_as  = graphle::meta::deduce_as<graphle::compare_by_value<vertex>>,
                .get_vertices       = [this] { return views::all(vertices) | views::transform(util::addressof); },
                .get_out_edges      = [] (vertex* v) { return views::all(v->out) | views::edge_from(v); }
            };
        }
    };


    /**
     * @ingroup TestGraphs
     * Graph of the form G { root = V }, V { in_edges = V*[], out_edges = V*[] }
     */
    struct in_out_edge_graph {
        // This is bad performance-wise but this type is only used for testing.
        struct vertex {
            std::size_t vertex_id;
            std::vector<std::shared_ptr<vertex>> in;
            std::vector<std::shared_ptr<vertex>> out;
        };

        std::shared_ptr<vertex> root;


        static in_out_edge_graph from_ve_list(const ve_list_graph& g, std::size_t root = 0) {
            std::unordered_map<std::size_t, std::shared_ptr<vertex>> vertices;

            for (const auto& v : g.vertices) {
                vertices.emplace(v.vertex_id, std::make_shared<vertex>(v.vertex_id));
            }

            for (const auto& e : g.edges) {
                auto& from = vertices.at(e.first.vertex_id);
                auto& to   = vertices.at(e.second.vertex_id);

                from->out.push_back(to);
                to->in.push_back(from);
            }

            return in_out_edge_graph { .root = vertices.at(root) };
        }


        constexpr auto view_as_graph(void) {
            return graph {
                .deduce_vertex_type = graphle::meta::deduce_as<vertex>,
                .deduce_compare_as  = graphle::meta::deduce_as<graphle::compare_by_value<vertex>>,
                .get_out_edges = [] (vertex* v) {
                    return views::all(v->out)
                        | views::transform(&std::shared_ptr<vertex>::get)
                        | views::edge_from(v);
                },
                .get_in_edges = [] (vertex* v) {
                     return views::all(v->in)
                        | views::transform(&std::shared_ptr<vertex>::get)
                        | views::edge_to(v);
                }
            };
        }
    };


    /**
     * @ingroup TestGraphs
     * Graph of the form G { vertices = V[] }, V { in_edges = V*[], out_edges = V*[] }
     */
    struct v_list_in_out_edge_graph {
        struct vertex {
            std::size_t vertex_id;
            std::vector<vertex*> in;
            std::vector<vertex*> out;
        };

        std::vector<vertex> vertices;


        static v_list_in_out_edge_graph from_ve_list(const ve_list_graph& g) {
            auto vs = g.vertices | std::views::transform(to_vertex<vertex>);
            v_list_in_out_edge_graph result { .vertices { vs.begin(), vs.end() } };
            std::unordered_map<std::size_t, vertex*> vertex_map;

            for (const auto [from, to] : g.edges) {
                for (auto v : { from, to }) {
                    if (!vertex_map.contains(v.vertex_id)) {
                        vertex_map.emplace(v.vertex_id, &*std::ranges::find(result.vertices, vertex { v.vertex_id }));
                    }
                }

                vertex_map.at(from.vertex_id)->out.push_back(vertex_map.at(to.vertex_id));
                vertex_map.at(to.vertex_id)->in.push_back(vertex_map.at(from.vertex_id));
            }

            return result;
        }


        constexpr auto view_as_graph(void) {
            return graph {
                .deduce_vertex_type = graphle::meta::deduce_as<vertex>,
                .deduce_compare_as  = graphle::meta::deduce_as<graphle::compare_by_value<vertex>>,
                .get_vertices       = [this] { return views::all(vertices) | views::transform(util::addressof); },
                .get_out_edges      = [] (vertex* v) { return views::all(v->out) | views::edge_from(v); },
                .get_in_edges       = [] (vertex* v) { return views::all(v->out) | views::edge_to(v); }
            };
        }
    };


    /**
     * @ingroup TestGraphs
     * List of test datastructure types.
     */
    using datastructure_list = meta::type_list<
        ve_list_graph,
        ve_map_graph,
        out_edge_graph,
        v_list_out_edge_graph,
        in_out_edge_graph,
        v_list_in_out_edge_graph
    >;
}

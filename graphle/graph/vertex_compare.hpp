#pragma once

#include <common.hpp>
#include <meta/concepts.hpp>


namespace graphle {
    namespace detail {
        template <typename V> using edge_for = std::pair<V*, V*>;
    }


    /** Equality comparators for vertices and edges. */
    namespace comparators {
        /** Equality comparator that compares two vertices by their address. */
        template <typename Vertex> struct vertex_address {
            constexpr bool operator()(const Vertex* a, const Vertex* b) const {
                return a == b;
            }
        };

        /** Equality comparator that compares two vertices by their equality operator. */
        template <typename Vertex> struct vertex_value {
            constexpr bool operator()(const Vertex* a, const Vertex* b) const {
                return (*a) == (*b);
            }
        };

        /** Equality comparator that compares two edges using a vertex comparator. */
        template <typename Vertex, typename VertexCmp> struct edge_as_vertex {
            VertexCmp vertex_comparator = {};

            using edge       = std::pair<Vertex*, Vertex*>;
            using const_edge = std::pair<const Vertex*, const Vertex*>;

            constexpr bool operator()(const edge& a, const edge& b) const {
                return vertex_comparator(a.first, b.first) && vertex_comparator(a.second, b.second);
            }

            constexpr bool operator()(const const_edge& a, const const_edge& b) const {
                return vertex_comparator(a.first, b.first) && vertex_comparator(a.second, b.second);
            }
        };
    }


    /** Hashers for vertices and edges. */
    namespace hashers {
        constexpr inline std::size_t hash_combine(std::size_t a, std::size_t b) {
            // Shamelessly stolen from boost::hash_combine
            // https://www.boost.org/doc/libs/1_55_0/doc/html/hash/reference.html#boost.hash_combine
            return a ^ (b + 0x9E3779B9 + (a << 6) + (a >> 2));
        }


        /** Hasher that hashes a vertex's address. */
        template <typename Vertex> struct vertex_address {
            constexpr bool operator()(const Vertex* a) const {
                return std::hash<Vertex*>{}(a);
            }
        };

        /** Hasher that invokes std::hash for the vertex (not the pointer). */
        template <typename Vertex> struct vertex_value {
            constexpr bool operator()(const Vertex* a) const {
                return std::hash<Vertex>{}(*a);
            }
        };

        /** Hasher that hashes an edge using a vertex comparator. */
        template <typename Vertex, typename VertexHash> struct edge_as_vertex {
            VertexHash vertex_hasher = {};

            using edge       = std::pair<Vertex*, Vertex*>;
            using const_edge = std::pair<const Vertex*, const Vertex*>;

            constexpr bool operator()(const edge& a) const {
                return hash_combine(vertex_hasher(a.first), vertex_hasher(a.second));
            }

            constexpr bool operator()(const const_edge& a) const {
                return hash_combine(vertex_hasher(a.first), vertex_hasher(a.second));
            }
        };
    }


    /** Concept for a type to be used as the CompareAs parameter for graphle::graph. */
    template <typename T, typename Vertex, typename Edge = detail::edge_for<Vertex>> concept graph_compare_traits = requires {
        meta::invocable_r<typename T::vertex_compare, bool, const Vertex*, const Vertex*>;
        meta::invocable_r<typename T::edge_compare, bool, const Edge, const Edge>;
        meta::invocable_r<typename T::vertex_hash, std::size_t, const Vertex*>;
        meta::invocable_r<typename T::edge_hash, std::size_t, const Edge>;
    };


    /** Compare vertices by their address. Can be used for the CompareAs parameter of graphle::graph. */
    template <typename Vertex> struct compare_by_address {
        using vertex_compare = comparators::vertex_address<Vertex>;
        using edge_compare   = comparators::edge_as_vertex<Vertex, vertex_compare>;
        using vertex_hash    = hashers::vertex_address<Vertex>;
        using edge_hash      = hashers::edge_as_vertex<Vertex, vertex_hash>;
    };


    /** Compare vertices by their value. Can be used for the CompareAs parameter of graphle::graph. */
    template <typename Vertex> struct compare_by_value {
        using vertex_compare = comparators::vertex_value<Vertex>;
        using edge_compare   = comparators::edge_as_vertex<Vertex, vertex_compare>;
        using vertex_hash    = hashers::vertex_value<Vertex>;
        using edge_hash      = hashers::edge_as_vertex<Vertex, vertex_hash>;
    };
}
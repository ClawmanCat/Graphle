#pragma once

#include <common.hpp>
#include <graph/graph.hpp>
#include <utility/range_utils.hpp>

#include <utility>


namespace graphle {
    namespace detail {
        template <rng::input_range R, typename ProjectBound, typename ProjectRange> requires (rng::view<R> && is_vertex<rng::range_value_t<R>>)
        class edge_from_vertex_view : public rng::view_interface<edge_from_vertex_view<R, ProjectBound, ProjectRange>> {
        public:
            using vertex_type = typename rng::range_value_t<R>;
            using edge_type   = std::pair<vertex_type, vertex_type>;

            /**
             * @warning The constness of bind_vertex should match that of the range. This means that e.g. code like the following will not work:
             * ~~~
             * std::unordered_set<Vertex> vertices;  // Note: set so we always get a const version of the value!
             * std::vector<Vertex> edges_for_vertex; // Note: vector so we always get a mutable version of the value!
             *
             * auto view = edges_for_vertex
             *     | views::transform(util::addressof)     // Vertex*
             *     | views::edge_from(&*vertices.begin()); // const Vertex*
             * ~~~
             */
            constexpr edge_from_vertex_view(R base_range, vertex_type bind_vertex) :
                base_range(std::move(base_range)),
                bind_vertex(bind_vertex)
            {}

            [[nodiscard]] constexpr auto begin(void) const { return iterator { rng::begin(base_range), bind_vertex }; }
            [[nodiscard]] constexpr auto end  (void) const { return iterator { rng::end(base_range), bind_vertex   }; }

            [[nodiscard]] constexpr auto size(void) const requires rng::sized_range<const R> {
                return rng::size(base_range);
            }
        private:
            R base_range;
            vertex_type bind_vertex;


            class iterator : public transforming_wrapping_iterator<iterator, detail::const_iterator_t<R>, edge_type> {
            public:
                using base = transforming_wrapping_iterator<iterator, detail::const_iterator_t<R>, edge_type>;

                constexpr iterator(void) = default;
                constexpr iterator(detail::const_iterator_t<R> base_iterator, vertex_type bind_vertex) : base(std::move(base_iterator)), bind_vertex(bind_vertex) {}
            private:
                friend class wrapping_iterator<iterator, detail::const_iterator_t<R>, edge_type>;


                vertex_type bind_vertex;


                constexpr auto get(void) const {
                    auto* provided_vertex = *base::wrapped_iterator;

                    edge_type result;
                    ProjectBound{}(result) = bind_vertex;
                    ProjectRange{}(result) = provided_vertex;

                    return result;
                }
            };
        };
    }


    namespace views {
        /**
         * @ingroup Views
         * View that stores a vertex S and constructs edges [S, P] for every vertex P in the viewed range.
         */
        constexpr inline auto edge_from = detail::range_adaptor<
            detail::edge_from_vertex_view,
            detail::project_nth<0>,
            detail::project_nth<1>
        > {};

        /**
         * @ingroup Views
         * View that stores a vertex S and constructs edges [P, S] for every vertex P in the viewed range.
         */
        constexpr inline auto edge_to = detail::range_adaptor<
            detail::edge_from_vertex_view,
            detail::project_nth<0>,
            detail::project_nth<1>
        > {};
    }
}



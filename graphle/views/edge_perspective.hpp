#pragma once

#include <common.hpp>
#include <graph/graph.hpp>
#include <meta/const_pointer.hpp>
#include <utility/range_utils.hpp>

#include <utility>


namespace graphle {
    namespace detail {
        template <rng::input_range R, typename Projection> requires (rng::view<R> && is_edge<rng::range_value_t<R>>)
        class vertex_perspective_view : public rng::view_interface<vertex_perspective_view<R, Projection>> {
        public:
            using vertex_type = typename rng::range_value_t<R>::first_type;
            using edge_type   = std::pair<vertex_type, vertex_type>;

            vertex_perspective_view(R base_range, meta::pointed_to_as_const<vertex_type> target) :
                base_range(std::move(base_range)),
                target(target)
            {}

            [[nodiscard]] constexpr auto begin(void) const { return iterator { rng::begin(base_range), target }; }
            [[nodiscard]] constexpr auto end  (void) const { return iterator { rng::end(base_range), target   }; }

            [[nodiscard]] constexpr auto size(void) const requires rng::sized_range<const R> {
                return rng::size(base_range);
            }
        private:
            R base_range;
            meta::pointed_to_as_const<vertex_type> target;


            class iterator : public transforming_wrapping_iterator<iterator, rng::iterator_t<R>, edge_type> {
            public:
                using base = transforming_wrapping_iterator<iterator, rng::iterator_t<R>, edge_type>;

                constexpr iterator(void) = default;
                iterator(rng::iterator_t<R> base_iterator, meta::pointed_to_as_const<vertex_type> target) : base(std::move(base_iterator)), target(target) {}

            private:
                friend class wrapping_iterator<iterator, rng::iterator_t<R>, edge_type>;


                meta::pointed_to_as_const<vertex_type> target;


                constexpr auto get(void) const {
                    auto edge = *base::wrapped_iterator;
                    if (Projection{}(edge) == target) std::swap(edge.first, edge.second);
                    return edge;
                }
            };
        };
    }


    namespace views {
        /**
         * @ingroup Views
         * View that transposes every edge that includes the vertex V such that V is the first element of that edge.
         */
        constexpr inline auto from_vertex_perspective = detail::range_adaptor<
            detail::vertex_perspective_view,
            detail::project_nth<1>
        > {};

        /**
         * @ingroup Views
         * View that transposes every edge that includes the vertex V such that V is the second element of that edge.
         */
        constexpr inline auto to_vertex_perspective = detail::range_adaptor<
            detail::vertex_perspective_view,
            detail::project_nth<0>
        > {};
    }
}



#pragma once

#include <common.hpp>
#include <graph/graph.hpp>
#include <utility/range_utils.hpp>


namespace graphle {
    namespace detail {
        template <rng::input_range R> requires (rng::view<R> && is_edge_v<rng::range_value_t<R>>)
        class edge_duplication_view : public rng::view_interface<edge_duplication_view<R>> {
        public:
            using edge_type = rng::range_value_t<R>;


            explicit edge_duplication_view(R base_range) : base_range(std::move(base_range)) {}

            [[nodiscard]] constexpr auto begin(void) const { return iterator { rng::begin(base_range) }; }
            [[nodiscard]] constexpr auto end  (void) const { return iterator { rng::end(base_range)   }; }

            [[nodiscard]] constexpr auto size(void) const requires rng::sized_range<const R> {
                return 2 * rng::size(base_range);
            }
        private:
            R base_range;


            class iterator : public wrapping_iterator<iterator, rng::iterator_t<R>, edge_type> {
            public:
                using wrapping_iterator<iterator, rng::iterator_t<R>, edge_type>::is_forward;
                using wrapping_iterator<iterator, rng::iterator_t<R>, edge_type>::is_bidirectional;
                using wrapping_iterator<iterator, rng::iterator_t<R>, edge_type>::is_random_access;


                constexpr iterator(void) = default;
                constexpr explicit iterator(rng::iterator_t<R> base_iterator) : base_iterator(base_iterator) {}


                [[nodiscard]] constexpr bool operator==(const iterator& other) const requires is_forward {
                    return base_iterator == other.base_iterator && transpose == other.transpose;
                }

                [[nodiscard]] constexpr auto operator<=>(const iterator& other) const requires is_random_access {
                    if (auto cmp = (base_iterator <=> other.base_iterator); cmp != 0) return cmp;
                    return transpose <=> other.transpose;
                }
            private:
                friend class wrapping_iterator<iterator, rng::iterator_t<R>, edge_type>;


                rng::iterator_t<R> base_iterator;
                bool transpose = false;


                constexpr auto get(void) const {
                    auto edge = *base_iterator;

                    if (transpose) return std::pair { edge.second, edge.first };
                    else return edge;
                }

                constexpr void next(void) requires is_forward {
                    transpose = !transpose;
                    if (!transpose) ++base_iterator;
                }

                constexpr void prev(void) requires is_bidirectional {
                    transpose = !transpose;
                    if (transpose) --base_iterator;
                }

                constexpr void advance(std::ptrdiff_t distance) requires is_random_access {
                    base_iterator += (distance >> 1);
                    if (distance & 1) transpose = !transpose;
                }
            };
        };
    }


    namespace views {
        /**
         * @ingroup Views
         * View that iterates both [A, B] as well as [B, A] for every original edge [A, B].
         */
        constexpr inline auto duplicate_transposed_edges = detail::range_adaptor<detail::edge_duplication_view> {};
    }
}
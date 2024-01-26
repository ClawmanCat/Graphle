#pragma once

#include <common.hpp>
#include <meta/value.hpp>
#include <utility/range_utils.hpp>

#include <utility>
#include <optional>


namespace graphle {
    namespace detail {
        template <rng::input_range R> requires rng::view<R>
        class maybe_exists_view : public rng::view_interface<maybe_exists_view<R>> {
        public:
            constexpr explicit maybe_exists_view(R base_range) : base_range(base_range) {}
            constexpr explicit maybe_exists_view(meta::deduce_as_t<R>) : base_range(std::nullopt) {}

            [[nodiscard]] constexpr auto begin(void) const { return base_range ? iterator { rng::begin(*base_range) } : iterator { }; }
            [[nodiscard]] constexpr auto end  (void) const { return base_range ? iterator { rng::end(*base_range) }   : iterator { }; }

            [[nodiscard]] constexpr auto size(void) const requires rng::sized_range<const R> {
                return base_range ? rng::size(*base_range) : 0;
            }
        private:
            std::optional<R> base_range;


            class iterator : public wrapping_iterator<iterator, rng::iterator_t<R>, typename range_iterator_traits<R>::reference> {
            public:
                using base = wrapping_iterator<iterator, rng::iterator_t<R>, typename range_iterator_traits<R>::reference>;

                constexpr iterator(void) : wrapped(std::nullopt) {}
                constexpr explicit iterator(rng::iterator_t<R> wrapped) : wrapped(wrapped) {}

                // Implicitly deleted if defaulted for some reason?
                constexpr bool operator==(const iterator& o) const {
                    return wrapped == o.wrapped;
                }

                // Implicitly deleted if defaulted for some reason?
                constexpr auto operator<=>(const iterator& o) const {
                    return wrapped <=> o.wrapped;
                }
            private:
                friend class wrapping_iterator<iterator, rng::iterator_t<R>, typename range_iterator_traits<R>::reference>;

                std::optional<rng::iterator_t<R>> wrapped;

                constexpr decltype(auto) get(void) const { return **wrapped; }

                constexpr void next(void) requires base::is_forward { ++(*wrapped); }
                constexpr void prev(void) requires base::is_bidirectional { --(*wrapped); }
                constexpr void advance(std::ptrdiff_t delta) requires base::is_random_access { (*wrapped) += delta; }
            };
        };
    }


    namespace views {
        /**
         * @ingroup Views
         * View that either wraps a provided view or is an empty view.
         * This can be used to provide a wrapper around a range that might not exist, e.g.:
         * ~~~
         * std::unordered_map<Key, std::vector<T>> my_values;
         * using view_type = decltype(ranges::all(my_values.at(key)));
         *
         * auto values_for_key = my_values.contains(key)
         *     ? views::maybe_exists(ranges::all(my_values.at(key)))
         *     : views::maybe_exists(meta::deduce_type<view_type>);
         * ~~~
         */
        constexpr inline auto maybe_exists = detail::primary_range_adaptor<detail::maybe_exists_view> {};
    }
}



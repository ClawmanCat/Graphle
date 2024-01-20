#pragma once

#include <common.hpp>
#include <meta/type_list.hpp>

#include <concepts>
#include <iterator>


// Boilerplate code for std::ranges compatibility.
// Custom views should instantiate the Niebloid range_adapter in the appropriate views namespace.
namespace graphle::detail {
    /** Base class for all Graphle views so they can be matched by our operator|. */
    struct graphle_view_base {};


    /** Returns the type of the const iterator of the range R. This is a C++23 feature so we have to provide our own. */
    template <rng::range R> using const_iterator_t = decltype(rng::cbegin(std::declval<R&>()));


    /** Projection that returns the Nth element of a tuple-like type. */
    template <std::size_t N> struct project_nth {
        template <typename TupleLike> [[nodiscard]] constexpr decltype(auto) operator()(TupleLike&& tpl) const {
            return std::get<N>(tpl);
        }
    };


    /**
     * Boilerplate range adaptor closure for std::ranges compatibility for view type G.
     * @tparam G The template of a view type, accepting a range and the types in L as its arguments.
     * @tparam L A list of types passed to G as template arguments together with the range.
     * @tparam Args Arguments that can be used to construct a view of type G<Range, L...>.
     */
    template <template <typename...> typename G, meta::list_of_types L, typename... Args> struct range_adaptor_closure {
        std::tuple<Args...> args;


        template <typename... UArgs> requires (std::is_same_v<std::remove_cvref_t<UArgs>, Args> && ...)
        explicit constexpr range_adaptor_closure(UArgs&&... args) :
            args(GRAPHLE_FWD(args)...)
        { }


        template <rng::viewable_range R> constexpr auto operator()(R&& range) const & {
            return std::apply([&] (const auto&... args) {
                return L::apply([&] <typename... GArgs> {
                    return G<std::remove_reference_t<R>, GArgs...> { GRAPHLE_FWD(range), args... };
                });
            }, args);
        }


        template <rng::viewable_range R> constexpr auto operator()(R&& range) && {
            return std::apply([&] (auto&&... args) {
                return L::apply([&] <typename... GArgs> {
                    return G<std::remove_reference_t<R>, GArgs...> { GRAPHLE_FWD(range), GRAPHLE_FWD(args)... };
                });
            }, args);
        }
    };


    /**
     * Boilerplate range adaptor for std::ranges compatibility for view type G.
     * @tparam G The template of a view type, accepting a range and the types in GArgs as its arguments.
     * @tparam GArgs A list of types passed to G as template arguments together with the range.
     */
    template <template <typename...> typename G, typename... GArgs> struct range_adaptor {
        template <rng::viewable_range R, typename... Args> requires std::is_constructible_v<G<R, GArgs...>, R&&, Args&&...>
        constexpr auto operator()(R&& range, Args&&... args) const {
            return G<std::remove_reference_t<R>, GArgs...> { GRAPHLE_FWD(range), GRAPHLE_FWD(args)... };
        }


        template <typename... Args> requires std::is_constructible_v<
            range_adaptor_closure<G, meta::type_list<GArgs...>, std::remove_reference_t<Args>...>,
            std::remove_reference_t<Args>...
        > constexpr auto operator()(Args&&... args) const {
            return range_adaptor_closure<G, meta::type_list<GArgs...>, std::remove_reference_t<Args>...> { GRAPHLE_FWD(args)... };
        }
    };


    /**
     * CRTP base class for (at least forward-) iterators that wrap some other iterator.
     *
     * Derived class should provide the following methods:
     *  - operator==(const Derived&) const
     *  - operator<=>(const Derived&) if Iterator is a random_access_iterator
     *  - get(void) const
     *  - next(void)
     *  - prev(void) if Iterator is a bidirectional_iterator
     *  - advance(std::ptrdiff_t) if Iterator is a random_access_iterator
     *
     * @tparam Derived The CRTP-derived iterator class.
     * @tparam Iterator The iterator type that is wrapped by Derived.
     * @tparam T The type returned by get().
     */
    template <typename Derived, typename Iterator, typename T> class wrapping_iterator {
    public:
        using value_type        = std::remove_reference_t<T>;
        using reference         = T;
        using pointer           = std::add_pointer_t<reference>;
        using difference_type   = typename std::iterator_traits<Iterator>::difference_type;
        using iterator_category = std::common_type_t<typename std::iterator_traits<Iterator>::iterator_category, std::random_access_iterator_tag>;

        constexpr static inline bool is_forward       = std::is_base_of_v<std::input_iterator_tag,         iterator_category>;
        constexpr static inline bool is_bidirectional = std::is_base_of_v<std::bidirectional_iterator_tag, iterator_category>;
        constexpr static inline bool is_random_access = std::is_base_of_v<std::random_access_iterator_tag, iterator_category>;


        [[nodiscard]] reference operator*(void) const requires is_forward {
            return derived().get();
        }

        [[nodiscard]] pointer operator->(void) const requires is_forward {
            return std::addressof(derived().get());
        }

        [[nodiscard]] reference operator[](difference_type d) const requires is_random_access {
            return *(derived() + d);
        }


        Derived& operator++(void) requires is_forward       { derived().next(); return derived(); }
        Derived& operator--(void) requires is_bidirectional { derived().prev(); return derived(); }
        Derived  operator++(int)  requires is_forward       { auto old = derived(); derived().next(); return old(); }
        Derived  operator--(int)  requires is_bidirectional { auto old = derived(); derived().prev(); return old(); }


        friend Derived& operator+=(Derived& self, difference_type d) requires is_random_access { self.advance(+d); return self(); }
        friend Derived& operator-=(Derived& self, difference_type d) requires is_random_access { self.advance(-d); return self(); }
        [[nodiscard]] friend Derived operator+(const Derived& self, difference_type d) requires is_random_access { auto temp = self; temp.advance(+d); return temp; }
        [[nodiscard]] friend Derived operator-(const Derived& self, difference_type d) requires is_random_access { auto temp = self; temp.advance(-d); return temp; }
    private:
        [[nodiscard]] Derived& derived(void) { return static_cast<Derived&>(*this); }
        [[nodiscard]] const Derived& derived(void) const { return static_cast<const Derived&>(*this); }
    };


    /**
     * CRTP base class for (at least forward-) iterators that wrap some other iterator, transforming its value.
     * Unlike wrapping_iterator, this class increments/decrements the iterator automatically by incrementing/decrementing the wrapped iterator.
     *
     * Derived class should provide the following methods:
     * - get(void) const
     *
     * @tparam Derived The CRTP-derived iterator class.
     * @tparam Iterator The iterator type that is wrapped by Derived.
     * @tparam T The type returned by get().
     */
    template <typename Derived, typename Iterator, typename T> class transforming_wrapping_iterator : public wrapping_iterator<Derived, Iterator, T> {
    public:
        using base              = wrapping_iterator<Derived, Iterator, T>;
        using value_type        = typename base::value_type;
        using reference         = typename base::reference;
        using pointer           = typename base::pointer;
        using difference_type   = typename base::difference_type;
        using iterator_category = typename base::iterator_category;

        using base::is_forward;
        using base::is_bidirectional;
        using base::is_random_access;


        constexpr transforming_wrapping_iterator(void) = default;
        constexpr explicit transforming_wrapping_iterator(Iterator wrapped_iterator) : wrapped_iterator(std::move(wrapped_iterator)) {}


        using base::operator*;
        using base::operator->;
        using base::operator++;
        using base::operator--;
        using base::operator[];


        [[nodiscard]] constexpr bool operator==(const transforming_wrapping_iterator& other) const requires is_forward {
            return wrapped_iterator == other.wrapped_iterator;
        }

        [[nodiscard]] constexpr auto operator<=>(const transforming_wrapping_iterator& other) const requires is_random_access {
            return wrapped_iterator <=> other.wrapped_iterator;
        }

    protected:
        Iterator wrapped_iterator;

    private:
        friend base;


        constexpr void next(void) requires is_forward {
            ++wrapped_iterator;
        }

        constexpr void prev(void) requires is_bidirectional {
            --wrapped_iterator;
        }

        constexpr void advance(difference_type distance) requires is_random_access {
            wrapped_iterator += distance;
        }
    };
}


/**
 * @ingroup Views
 * Range pipe operator for all Graphle views (Types deriving from graphle::detail::graphle_view_base).
 */
template <std::ranges::viewable_range R, template <typename...> typename G, typename... Args>
constexpr auto operator|(R&& range, const graphle::detail::range_adaptor_closure<G, Args...>& closure) {
    return closure(GRAPHLE_FWD(range));
}
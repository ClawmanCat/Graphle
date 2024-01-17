#pragma once

#include <common.hpp>
#include <storage/storage_provider.hpp>

#include <vector>
#include <unordered_map>
#include <unordered_set>
#include <deque>


namespace graphle::store {
    /**
     * @ingroup Store
     * Used to differentiate between the graphle-implementation and any user-provided implementation of the default_storage_provider.
     */
    enum class overload_mode { DEFAULT_IMPLEMENTATION, USER_PROVIDED };


    /**
     * @ingroup Store
     * The default storage provider is used to provide storage required for graph algorithms when no storage provider is given by the user.
     * It should return an instance (by value or by reference) of a valid storage object for the given storage type when invoked.
     *
     * Users may change the default storage provider for a given storage type by specializing the template with Overload = overload_mode::USER_PROVIDED.
     * This is useful e.g. if your project uses a specific container type (e.g. Abseil's or Boost's unordered map instead of the default one),
     * and you wish to replace the storage used by graphle with that type globally.
     * It is also possible to just provide a storage provider to the algorithm that needs it to control the storage on a case-by-case basis.
     *
     * @tparam ST The storage type that must be provided.
     * @tparam Overload Allows users to specialize this template.
     * @tparam Args Template arguments for the provided storage object (E.g. key and value types for a map storage provider).
     */
    template <storage_type ST, overload_mode Overload, typename... Args> struct default_storage_provider {
        using not_specialized_tag = void;
    };


    /**
     * @ingroup Store
     * The default storage provider for a sequence of objects. Constructs and returns a std::vector<T>.
     */
    template <typename T> struct default_storage_provider<storage_type::VECTOR, overload_mode::DEFAULT_IMPLEMENTATION, T> {
        constexpr auto operator()(void) const noexcept {
            return std::vector<T>{};
        }
    };


    /**
     * @ingroup Store
     * The default storage provider for an unordered map of objects. Constructs and returns a std::unordered_map<K, V, Hash, Eq>.
     */
    template <typename K, typename V, typename Hash, typename Eq> struct default_storage_provider<storage_type::UNORDERED_MAP, overload_mode::DEFAULT_IMPLEMENTATION, K, V, Hash, Eq> {
        constexpr auto operator()(void) const noexcept {
            return std::unordered_map<K, V, Hash, Eq>{};
        }
    };


    /**
     * @ingroup Store
     * The default storage provider for an unordered set of objects. Constructs and returns a std::unordered_set<K, Hash, Eq>.
     */
    template <typename K, typename Hash, typename Eq> struct default_storage_provider<storage_type::UNORDERED_SET, overload_mode::DEFAULT_IMPLEMENTATION, K, Hash, Eq> {
        constexpr auto operator()(void) const noexcept {
            return std::unordered_set<K, Hash, Eq>{};
        }
    };


    /**
     * @ingroup Store
     * The default storage provider for a double-ended queue of objects. Constructs and returns a std::deque<T>.
     */
    template <typename T> struct default_storage_provider<storage_type::DEQUE, overload_mode::DEFAULT_IMPLEMENTATION, T> {
        constexpr auto operator()(void) const noexcept {
            return std::deque<T>{};
        }
    };


    /**
     * @ingroup Store
     * Returns an instance of the user-provided default storage provider for the given storage type if there is one,
     * or an instance of the default implementation specified above otherwise.
     * @tparam ST The storage type that must be provided.
     * @tparam Args Template arguments for the provided storage object (E.g. key and value types for a map storage provider).
     * @return An instance of the default storage provider for the given storage type.
     */
    template <storage_type ST, typename... Args> constexpr inline auto get_default_storage_provider(void) {
        constexpr bool is_user_provided = !requires {
            typename default_storage_provider<ST, overload_mode::USER_PROVIDED, Args...>::not_specialized_tag;
        };


        using provider_type = default_storage_provider<
            ST,
            is_user_provided ? overload_mode::USER_PROVIDED : overload_mode::DEFAULT_IMPLEMENTATION,
            Args...
        >;

        static_assert(
            is_storage_provider_v<provider_type, ST, Args...>,
            "The default storage provider for the given storage type does not provide a valid storage object of that type."
        );


        return provider_type {};
    }


    /** Equal to the type returned by @ref get_default_storage_provider */
    template <storage_type ST, typename... Args> using default_provided_t = decltype(get_default_storage_provider<ST, Args...>());
}
#pragma once

#include <common.hpp>

#include <iterator>
#include <concepts>


/**
 * @ingroup Store
 * @def GRAPHLE_REUSABLE
 * If a storage provider is invoked multiple times by a single call to a Graphle algorithm, it will be marked with GRAPHLE_REUSABLE.
 * Note that at most one instance of the provider will be used at a time, so it is valid behaviour to simply clear the same storage object and return it again.
 */
#define GRAPHLE_REUSABLE /** @qualifier GRAPHLE_REUSABLE */

/**
 * @ingroup Store
 * @def GRAPHLE_MULTIPLE
 * If a storage provider is invoked multiple times by a single call to a Graphle algorithm, it will be marked with GRAPHLE_MULTIPLE.
 * Unlike GRAPHLE_REUSABLE, this macro indicates that the provider may be invoked again while its previous value is still in use, meaning it should return a new object each time.
 */
#define GRAPHLE_MULTIPLE /** @qualifier GRAPHLE_MULTIPLE */


namespace graphle::store {
    /**
     * @ingroup Store
     * Defines different types of storage used internally by Graphle algorithms.
     * Each value of this enum has an associated concept with requirements for the type to be used with Graphle.
     */
    enum class storage_type {
        /** Vector-like storage type. Associated concept: @ref vector_storage_type */
        VECTOR,
        /** Map-like storage type with hashable keys. Associated concept: @ref unordered_map_storage_type */
        UNORDERED_MAP,
        /** Set-like storage type with hashable keys. Associated concept: @ref unordered_set_storage_type */
        UNORDERED_SET,
        /** Deque-like storage type. Associated concept: @ref deque_storage_type */
        DEQUE
    };




    /**
     * @ingroup Store
     * Concept to check if a type is vector-like.
     */
    template <typename S, typename T> concept vector_storage_type = requires (S storage, T value) {
        { storage.clear()          };
        { storage.push_back(value) };
        { storage.pop_back()       };
        { storage.front()          } -> std::convertible_to<T&>;
        { storage.back()           } -> std::convertible_to<T&>;
        { rng::size(storage)       } -> std::convertible_to<std::size_t>;
        { rng::begin(storage)      } -> std::forward_iterator;
        { rng::end(storage)        } -> std::forward_iterator;
    } && std::convertible_to<rng::range_value_t<S>, T>;


    /**
     * @ingroup Store
     * Concept to check if a type is unordered-map-like.
     */
    template <typename S, typename K, typename V, typename I = typename S::iterator> concept unordered_map_storage_type = requires (S storage, K key, V value) {
        { storage.clear()             };
        { storage.emplace(key, value) } -> std::convertible_to<std::pair<I, bool>>;
        { storage.erase(key)          };
        { storage.find(key)           } -> std::convertible_to<I>;
        { storage.contains(key)       } -> std::convertible_to<bool>;
        { storage.at(key)             } -> std::convertible_to<V&>;
        { rng::size(storage)          } -> std::convertible_to<std::size_t>;
        { rng::begin(storage)         } -> std::forward_iterator;
        { rng::end(storage)           } -> std::forward_iterator;
    } && std::convertible_to<rng::range_value_t<S>, std::pair<const K, V>>;


    /**
     * @ingroup Store
     * Concept to check if a type is unordered-set-like
     */
    template <typename S, typename K, typename I = typename S::iterator> concept unordered_set_storage_type = requires (S storage, K key) {
        { storage.clear()       };
        { storage.emplace(key)  } -> std::convertible_to<std::pair<I, bool>>;
        { storage.erase(key)    };
        { storage.find(key)     } -> std::convertible_to<I>;
        { storage.contains(key) } -> std::convertible_to<bool>;
        { rng::size(storage)    } -> std::convertible_to<std::size_t>;
        { rng::begin(storage)   } -> std::forward_iterator;
        { rng::end(storage)     } -> std::forward_iterator;
    } && std::convertible_to<rng::range_value_t<S>, const K>;


    /**
     * @ingroup Store
     * Concept to check if a type is deque-like.
     */
    template <typename S, typename T> concept deque_storage_type = requires (S storage, T value) {
        { storage.clear()           };
        { storage.push_front(value) };
        { storage.pop_front()       };
        { storage.push_back(value)  };
        { storage.pop_back()        };
        { storage.front()           } -> std::convertible_to<T&>;
        { storage.back()            } -> std::convertible_to<T&>;
        { rng::size(storage)        } -> std::convertible_to<std::size_t>;
        { rng::begin(storage)       } -> std::forward_iterator;
        { rng::end(storage)         } -> std::forward_iterator;
    } && std::convertible_to<rng::range_value_t<S>, T>;




    /**
     * @ingroup Store
     * Type trait to check if the type S is a container of storage type ST.
     */
    template <typename S, storage_type ST, typename... Args> struct is_storage_type {
        constexpr static inline bool value = false;
    };

    /** @copydoc is_storage_type */
    template <typename S, typename T> struct is_storage_type<S, storage_type::VECTOR, T> {
        constexpr static inline bool value = vector_storage_type<S, T>;
    };

    /** @copydoc is_storage_type */
    template <typename S, typename K, typename V, typename Hash, typename Eq> struct is_storage_type<S, storage_type::UNORDERED_MAP, K, V, Hash, Eq> {
        constexpr static inline bool value = unordered_map_storage_type<S, K, V>;
    };

    /** @copydoc is_storage_type */
    template <typename S, typename K, typename Hash, typename Eq> struct is_storage_type<S, storage_type::UNORDERED_SET, K, Hash, Eq> {
        constexpr static inline bool value = unordered_set_storage_type<S, K>;
    };

    /** @copydoc is_storage_type */
    template <typename S, typename T> struct is_storage_type<S, storage_type::DEQUE, T> {
        constexpr static inline bool value = deque_storage_type<S, T>;
    };


    /** @copydoc is_storage_type */
    template <typename S, storage_type ST, typename... Args>
    constexpr inline bool is_storage_type_v = is_storage_type<S, ST, Args...>::value;

    /** Concept for @ref is_storage_type. @ingroup Store */
    template <typename S, storage_type ST, typename... Args>
    concept storage_for = is_storage_type_v<S, ST, Args...>;

    /** Equivalent to storage_for but with universal-reference-like behaviour. @ingroup Store */
    template <typename S, storage_type ST, typename... Args>
    concept storage_ref = storage_for<std::remove_cvref_t<S>, ST, Args...>;




    /** Type trait to check if the type P is a storage provider (See default_storage_provider.hpp) of storage type ST. @ingroup Store */
    template <typename P, storage_type ST, typename... Args> struct is_storage_provider {
        constexpr static inline bool value = is_storage_type_v<
            std::remove_reference_t<std::invoke_result_t<P>>,
            ST,
            Args...
        >;
    };

    /** @copydoc is_storage_provider */
    template <typename P, storage_type ST, typename... Args>
    constexpr inline bool is_storage_provider_v = is_storage_provider<P, ST, Args...>::value;

    /** Concept for @ref is_storage_provider. @ingroup Store */
    template <typename P, storage_type ST, typename... Args>
    concept storage_provider_for = is_storage_provider_v<P, ST, Args...>;

    /** Equivalent to storage_provider_for but with universal-reference-like behaviour. @ingroup Store */
    template <typename P, storage_type ST, typename... Args>
    concept storage_provider_ref = storage_provider_for<std::remove_cvref_t<P>, ST, Args...>;


    /** The type of storage returned by the storage provider P (with its original reference type). @ingroup Store */
    template <typename P> using provided_storage_type = std::invoke_result_t<P>;
    /** The type of storage returned by the storage provider P (as an lvalue). @ingroup Store */
    template <typename P> using provided_storage_value_type = std::remove_reference_t<std::invoke_result_t<P>>;


    /**
     * @ingroup Store
     * Allows rebinding the template arguments passed to the given storage provider. E.g.:
     * ~~~
     * using cat_vec_provider = decltype(get_default_storage_provider<storage_type::VECTOR, Cat>());
     * using dog_vec_provider = typename rebind_storage_provider<cat_vec_provider>::template type<Dog>;
     * ~~~
     */
    template <typename P> struct rebind_storage_provider {};

    /** @copydoc rebind_storage_provider */
    template <template <storage_type, typename...> typename P, storage_type ST, typename... Args> requires is_storage_provider_v<P<ST, Args...>, ST, Args...>
    struct rebind_storage_provider<P<ST, Args...>> {
        template <typename... NewArgs> using type = P<ST, NewArgs...>;
    };
}
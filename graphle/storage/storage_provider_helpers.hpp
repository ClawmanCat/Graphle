#pragma once

#include <common.hpp>

#include <type_traits>
#include <tuple>


namespace graphle::store {
    /**
     * @ingroup Store
     * Storage provider that returns a new instance of T constructed with Args whenever it is invoked.
     */
    template <typename T, typename... Args> requires std::is_constructible_v<T, Args...>
    class provide_newly_constructed {
    public:
        explicit provide_newly_constructed(Args&&... args) : args(GRAPHLE_FWD(args)...) {}

        constexpr auto operator()(void) const & noexcept {
            return std::apply([] (const auto&... args)  { return T { args... }; }, args);
        }

        constexpr auto operator()(void) && noexcept {
            return std::apply([] (auto&&... args)  { return T { GRAPHLE_FWD(args)... }; }, std::move(args));
        }
    private:
        std::tuple<Args...> args;
    };


    /**
     * @ingroup Store
     * Storage provider that returns an existing copy of T (owned by the provider) whenever it is invoked,
     * clearing it before each usage with T::clear(), unless ReturnCleared is set to false.
     */
    template <typename T, bool ReturnCleared = true> requires requires (T value) { value.clear(); }
    class provide_owned {
    public:
        explicit provide_owned(T value, std::bool_constant<ReturnCleared> = {}) : value(std::move(value)) {}

        constexpr auto operator()(void) const & noexcept {
            if constexpr (ReturnCleared) value.clear();
            return value;
        }

        constexpr auto operator()(void) && noexcept {
            if constexpr (ReturnCleared) value.clear();
            return std::move(value);
        }
    private:
        T value;
    };


    /**
     * @ingroup Store
     * Storage provider that returns an existing copy of T (owned by the caller) whenever it is invoked,
     * clearing it before each usage with T::clear(), unless ReturnCleared is set to false.
     * The provided storage object must remain valid for the duration of the lifetime of the provide_external provider that it is wrapped in.
     */
    template <typename T, bool ReturnCleared = true> requires requires (T value) { value.clear(); }
    class provide_external {
    public:
        explicit provide_external(T& value, std::bool_constant<ReturnCleared> = {}) : value(std::addressof(value)) {}

        constexpr auto& operator()(void) const noexcept {
            if constexpr (ReturnCleared) value->clear();
            return *value;
        }
    private:
        T* value;
    };
}
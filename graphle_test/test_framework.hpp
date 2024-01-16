#pragma once

#include <vector>
#include <string_view>
#include <iostream>
#include <source_location>
#include <format>


/**
 * @def TEST(Suite, Name)
 * Defines a new test with the given suite and name which will be invoked when tests are run.
 */
#define TEST(Suite, Name)                                       \
struct test_##Suite_##Name {                                    \
    static void main(void);                                     \
                                                                \
    static inline auto autoregister = [] {                      \
        graphle::test::test_registry::instance()                \
            .register_test(                                     \
                #Suite,                                         \
                #Name,                                          \
                &test_##Suite_##Name::main);                    \
                                                                \
        return 0;                                               \
    } ();                                                       \
};                                                              \
                                                                \
void test_##Suite_##Name::main(void)


/**
 * @def FAIL(...)
 * Fails the current test with the given message.
 */
#define FAIL(...) \
throw graphle::test::assertion_failure { __VA_ARGS__ }

/**
 * @def ASSERT_TRUE(...)
 * Fails the current test if the given condition is not true.
 */
#define ASSERT_TRUE(...) \
if (!bool(__VA_ARGS__)) FAIL(#__VA_ARGS__)

/**
 * @def ASSERT_FALSE(...)
 * Fails the current test if the given condition is true.
 */
#define ASSERT_FALSE(...) \
if (bool(__VA_ARGS__)) FAIL(#__VA_ARGS__)


namespace graphle::test {
    template <typename R, typename... A> using fn = R(*)(A...);


    /** Exception thrown when a test fails. */
    struct assertion_failure : std::exception {
        explicit assertion_failure(std::string_view condition, std::source_location where = std::source_location::current()) :
            condition(condition),
            where(where),
            message(std::format("Assertion failure at line {} in {} ({}): {}", where.line(), where.file_name(), where.function_name(), condition))
        {}

        const char* what(void) const override {
            return message.c_str();
        }

        std::string_view condition;
        std::source_location where;
        std::string message;
    };


    /** Registry to keep track of tests. */
    class test_registry {
    public:
        constexpr static inline const char* color_red     = "\x1B[31m";
        constexpr static inline const char* color_green   = "\x1B[32m";
        constexpr static inline const char* color_restore = "\033[0m";


        static test_registry& instance(void) {
            static test_registry i { };
            return i;
        }


        void register_test(std::string_view suite, std::string_view name, fn<void> test) {
            tests.emplace_back(test, std::format("{}::{}", suite, name));
        }


        int run_tests(void) {
            bool has_failures = false;


            for (const auto& [test, name] : tests) {
                try {
                    std::invoke(test);

                    printf(color_green);
                    std::cout << "[Test " << name << "]: SUCCESS\n";
                    printf(color_restore);
                } catch (const std::exception& e) {
                    has_failures = true;

                    printf(color_red);
                    std::cout << "[Test " << name << "]: FAILED\n\t" << e.what() << "\n";
                    printf(color_restore);
                }
            }

            return has_failures ? EXIT_FAILURE : EXIT_SUCCESS;
        }
    private:
        struct test_data {
            fn<void> test;
            std::string name;
        };

        std::vector<test_data> tests;
    };
}


/** Test framework entry point. Runs all registered tests. */
int main(void) {
    return graphle::test::test_registry::instance().run_tests();
}
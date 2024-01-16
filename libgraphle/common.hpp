#pragma once


// Doxygen's Clang version is old enough to trigger errors in the MSVC STL, since it requires Clang 16 or newer.
#if defined(GRAPHLE_DOXYGEN) && defined(_MSC_VER)
    #define _ALLOW_COMPILER_AND_STL_VERSION_MISMATCH
#endif


#include <ranges>
#include <utility>


#define GRAPHLE_FWD(...) std::forward<decltype(__VA_ARGS__)>(__VA_ARGS__)


namespace graphle {
    namespace rng = std::ranges;

    namespace views {
        using namespace std::ranges::views;
    }
}
ADD_LIBRARY(LibGraphle INTERFACE)
TARGET_INCLUDE_DIRECTORIES(LibGraphle INTERFACE ${CMAKE_CURRENT_SOURCE_DIR})


INCLUDE(create_include_header)
CREATE_INCLUDE_HEADER(LibGraphle "libgraphle"           "libgraphle.hpp")
CREATE_INCLUDE_HEADER(LibGraphle "libgraphle/algorithm" "../algorithm.hpp")
CREATE_INCLUDE_HEADER(LibGraphle "libgraphle/graph"     "../graph.hpp")
CREATE_INCLUDE_HEADER(LibGraphle "libgraphle/meta"      "../meta.hpp")
CREATE_INCLUDE_HEADER(LibGraphle "libgraphle/search"    "../search.hpp")
CREATE_INCLUDE_HEADER(LibGraphle "libgraphle/storage"   "../storage.hpp")
CREATE_INCLUDE_HEADER(LibGraphle "libgraphle/utility"   "../utility.hpp")
CREATE_INCLUDE_HEADER(LibGraphle "libgraphle/views"     "../views.hpp")


# Require /Zc:preprocessor for MSVC.
IF (MSVC)
    TARGET_COMPILE_OPTIONS(LibGraphle INTERFACE "/Zc:preprocessor")
ENDIF()
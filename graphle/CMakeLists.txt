ADD_LIBRARY(Graphle INTERFACE)
TARGET_INCLUDE_DIRECTORIES(Graphle INTERFACE ${CMAKE_CURRENT_SOURCE_DIR})


INCLUDE(create_include_header)
CREATE_INCLUDE_HEADER(LibGraphle "graphle"           "graphle.hpp")
CREATE_INCLUDE_HEADER(LibGraphle "graphle/algorithm" "../algorithm.hpp")
CREATE_INCLUDE_HEADER(LibGraphle "graphle/graph"     "../graph.hpp")
CREATE_INCLUDE_HEADER(LibGraphle "graphle/meta"      "../meta.hpp")
CREATE_INCLUDE_HEADER(LibGraphle "graphle/search"    "../search.hpp")
CREATE_INCLUDE_HEADER(LibGraphle "graphle/storage"   "../storage.hpp")
CREATE_INCLUDE_HEADER(LibGraphle "graphle/utility"   "../utility.hpp")
CREATE_INCLUDE_HEADER(LibGraphle "graphle/views"     "../views.hpp")


# Require /Zc:preprocessor for MSVC.
IF (MSVC)
    TARGET_COMPILE_OPTIONS(Graphle INTERFACE "/Zc:preprocessor")
ENDIF()
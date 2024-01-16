ADD_LIBRARY(Graphle INTERFACE)
TARGET_INCLUDE_DIRECTORIES(Graphle INTERFACE ${CMAKE_CURRENT_SOURCE_DIR})


INCLUDE(create_include_header)
CREATE_INCLUDE_HEADER(Graphle "graphle"           "graphle.hpp")
CREATE_INCLUDE_HEADER(Graphle "graphle/algorithm" "../algorithm.hpp")
CREATE_INCLUDE_HEADER(Graphle "graphle/graph"     "../graph.hpp")
CREATE_INCLUDE_HEADER(Graphle "graphle/meta"      "../meta.hpp")
CREATE_INCLUDE_HEADER(Graphle "graphle/search"    "../search.hpp")
CREATE_INCLUDE_HEADER(Graphle "graphle/storage"   "../storage.hpp")
CREATE_INCLUDE_HEADER(Graphle "graphle/utility"   "../utility.hpp")
CREATE_INCLUDE_HEADER(Graphle "graphle/views"     "../views.hpp")


# Require /Zc:preprocessor for MSVC.
IF (MSVC)
    TARGET_COMPILE_OPTIONS(Graphle INTERFACE "/Zc:preprocessor")
ENDIF()

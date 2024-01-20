CMAKE_MINIMUM_REQUIRED(VERSION 3.19)
PROJECT(Graphle CXX)


# Prevent concurrent invocation.
FILE(LOCK cmake.lock)


# Require C++20.
SET(CMAKE_CXX_STANDARD 20)
SET(CMAKE_CXX_STANDARD_REQUIRED ON)


# CMake Options
OPTION(GRAPHLE_TESTS "Enable generation of targets for unit tests." OFF)
OPTION(GRAPHLE_TESTS_THROW "Do not catch exceptions from unit tests so they can be intercepted by the IDE debugger." OFF)


# Use scripts from cmake folder.
SET(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_SOURCE_DIR}/cmake")


ADD_SUBDIRECTORY(graphle)


IF (GRAPHLE_TESTS)
    INCLUDE(CTest)
    ENABLE_TESTING()

    ADD_SUBDIRECTORY(graphle_test)
ENDIF()
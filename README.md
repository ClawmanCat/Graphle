# Graphle
Graphle is a small C++20 header-only graph library.
This library is currently a work-in-progress.

|   |   |   |   |
|---|---|---|---|
| ![Windows | MSVC](https://github.com/ClawmanCat/Graphle/actions/workflows/run-tests-windows-msvc.yml/badge.svg) | ![Windows | Clang](https://github.com/ClawmanCat/Graphle/actions/workflows/run-tests-windows-clang.yml/badge.svg) | ![Linux | GCC](https://github.com/ClawmanCat/Graphle/actions/workflows/run-tests-linux-gcc.yml/badge.svg) | ![Linux | Clang](https://github.com/github/ClawmanCat/Graphle/workflows/run-tests-linux-clang.yml/badge.svg) |


### Motivation & Design
This library came forth out of my frustration trying to use BGL in my game engine project. 
While I'm sure they did their best with what was possible using C++03, it's antiquated and overly-generic design 
combined with the lack of reasonable defaults for anything made it feel very cumbersome to use.  
I ended up re-implementing many of the algorithms from BGL into my own project, but as the number of re-implemented algorithms grew,
it seemed better to spin them off into their own library, which you're looking at now.  

Graphle is based around the following design principles:

###### Compatibility with existing data structures
Graphle is made to work out of the box with any existing data structure you might have. 
Simply construct a `graphle::graph` with some C++20 range-views of your data, and they will work with Graphle's algorithms.

###### Generic but with reasonable defaults
Just like with BGL you can provide custom containers for the graph algorithms to store their data in. Unlike with BGL however, you don't *have to*.  
Using Graphle's `storage_provider` classes, you can provide containers for the algorithms to use, both globally and on a per-function-call basis.

###### Usage of modern C++
Graphle makes heavy use of C++20 features like concepts and ranges, making both the code interacting with the library and any error messages you might get more legible.  
Furthermore, the entire library is constexpr, allowing the usage of the library in compile-time contexts.


### Algorithms
Graphle currently provides the following algorithms:
- Depth first search (DFS) using a visitor.
- Breadth first search (BFS) using a visitor.
- Tarjan's algorithm for finding strongly connected components (cycles).


### Basic Usage & Documentation
The basic usage of the library is to create a `graphle::graph` view of your existing datastructure, which can then be used with the Graphle algorithms.
Graphle uses pointers to the data in your provided data structure as vertices and pairs of vertices as edges.  
`graphle::graph` accepts various function-like objects as its parameters to provide information about the graph.
You need not provide all of these: each Graphle algorithm documents what data it requires.

```c++
// 1. Have some existing data structure:
struct my_data {
    std::vector<my_data*> dependencies;
};

std::unordered_map<some_key, my_data> my_datas;


// 2. Construct a graphle::graph view of the data:
graphle::graph graph_view {
    .deduce_vertex_type = graphle::meta::deduce_as<const my_data>,
    .get_vertices       = [&] { return my_datas | std::views::values | std::views::transform(graphle::util::addressof); },
    .get_out_edges      = [&] (const my_data* vertex) { return vertex.dependencies | graphle::views::edge_from(vertex); }
};


// 3. Use Graphle algorithms with the graph view:
auto cycles = graphle::strongly_connected_components(graph_view, 2);

if (!cycles.empty()) {
    throw std::runtime_error { "Circular dependency found!" };
}
```


###### Note on `graphle::meta::deduce_as`
C++ does not allow deducing the template arguments of a type while also providing explicit values for some other template parameters.
E.g. the following would not work:
```c++
// Error: cannot use designated initializers to deduce template parameters while also providing template parameter explicitly.
graphle::graph<const my_data> g {
    .get_vertices  = [&] { return my_datas | std::views::values | std::views::transform(graphle::util::addressof); },
    .get_out_edges = [&] (const my_data* vertex) { return vertex.dependencies | graphle::views::edge_from(vertex); }
};
```
To work around this, all types which allow template argument deduction through designated initializers have dummy `deduce_*` members,
which can be initialized as `deduce_some_param = graphle::meta::deduce_as<SomeType>` to perform template argument deduction.


###### Documentation
More extensive documentation can be found at the following locations:
- [Graphle API reference (Doxygen)](https://clawmancat.github.io/Graphle/)
- [TODO] Github Wiki


### Installation
This is a header-only library without any dependencies, meaning there is no need to compile it separately. 
You can simply include this library in your project (For example by downloading it or using it as a Git submodule) 
and compile it along the rest of your sources.  
A `conanfile.py` is also included with this project, if you are using the [Conan](https://conan.io) package manager.

To build the tests, run CMake with `-DGRAPHLE_TESTS=ON`. This will generate one executable for every `.cpp` file in the `libgraphle_test` folder,
which can then be invoked using CTest:
```shell
mkdir out
cd out
cmake -DGRAPHLE_TESTS=ON ../
cmake --build . --target all
ctest
```

To build the Doxygen documentation, simply install [Doxygen](https://www.doxygen.nl/) and run `doxygen doxyfile` from the root directory of the project.
The documentation will be generated into the `./out/doxygen/html` folder.


### Compatibility
This library aims to be compatible with all major compilers, as long as you are compiling in C++20 mode or newer.
That being said, each compiler has some unique compatibility problems that need to be accounted for. A table of known issues is shown below.

| Compiler | Known Issues                                                                                                                                                                           |
|----------|----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------|
| MSVC     | `/Zc:preprocessor` is required to enable support for `__VA_OPT__`. This is set automatically, but may cause issues for code that relies on the quirks of the legacy MSVC preprocessor. |
| Clang    | Before version 17, Clang is not able to correctly parse code using designated initializers.                                                                                            |
| GCC      | Not currently working. Will investigate once I have access to a Linux machine.                                                                                                         |

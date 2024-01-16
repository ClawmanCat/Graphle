from conans import ConanFile, CMake


class GraphleConan(ConanFile):
    name            = "graphle"
    version         = "1.0.0"
    license         = "MIT"
    url             = "https://github.com/ClawmanCat/Graphle"
    description     = "A C++20 header-only graph library."
    exports_sources = "graphle/*", "graphle_test/*", "LICENSE", "CMakeLists.txt"
    no_copy_source  = True
    settings        = "os", "compiler", "build_type", "arch"
    options         = { "build_tests": [True, False] }
    default_options = { "build_tests": False }
    generators      = "cmake"


    def build(self):
        if self.options.build_tests:
            cmake = CMake(self)
            cmake.definitions["GRAPHLE_TESTS"] = "ON"

            cmake.configure()
            cmake.build()
            cmake.test()


    def package(self):
        if self.options.build_tests: self.copy("*.cpp")
        self.copy("*.hpp", src = "graphle", dst = "include")


    def package_id(self):
        self.info.clear()
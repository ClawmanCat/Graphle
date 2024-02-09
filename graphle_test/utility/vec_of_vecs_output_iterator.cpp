#include <graphle.hpp>
#include <test_framework.hpp>

#include <vector>


TEST(vec_of_vecs_output_iterator, fill_vector) {
    std::vector<std::vector<std::size_t>> vv;
    graphle::util::vec_of_vecs_output_iterator it { vv };


    for (std::size_t i = 0; i < 4; ++i) {
        auto next = *it++;
        for (std::size_t j = 0; j < 4; ++j) *next++ = j;
    }


    ASSERT_TRUE(vv.size() == 4);

    const std::vector<std::size_t> expected { 0, 1, 2, 3 };
    for (const auto& v : vv) ASSERT_TRUE(std::ranges::equal(v, expected));
}
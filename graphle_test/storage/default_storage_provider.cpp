#include <graphle.hpp>
#include <test_framework.hpp>


namespace gpl = graphle;
namespace gs  = graphle::store;
namespace gm  = graphle::meta;


/**
 * @test default_storage_provider::vector
 * Check that the default storage provider for vector types fulfils the associated requirements.
 */
TEST(default_storage_provider, vector) {
    auto provider = gs::get_default_storage_provider<gs::storage_type::VECTOR, gm::placeholder>();
    ASSERT_TRUE(gs::is_storage_provider_v<decltype(provider), gs::storage_type::VECTOR, gm::placeholder>);
}


/**
 * @test default_storage_provider::deque
 * Check that the default storage provider for deque types fulfils the associated requirements.
 */
TEST(default_storage_provider, deque) {
    auto provider = gs::get_default_storage_provider<gs::storage_type::DEQUE, gm::placeholder>();
    ASSERT_TRUE(gs::is_storage_provider_v<decltype(provider), gs::storage_type::DEQUE, gm::placeholder>);
}


/**
 * @test default_storage_provider::unordered_set
 * Check that the default storage provider for unordered set types fulfils the associated requirements.
 */
TEST(default_storage_provider, unordered_set) {
    auto provider = gs::get_default_storage_provider<gs::storage_type::UNORDERED_SET, int>();
    ASSERT_TRUE(gs::is_storage_provider_v<decltype(provider), gs::storage_type::UNORDERED_SET, int>);
}


/**
 * @test default_storage_provider::unordered_set
 * Check that the default storage provider for unordered set types with manually specified hasher and equality fulfils the associated requirements.
 */
TEST(default_storage_provider, unordered_set_with_user_compare) {
    auto provider = gs::get_default_storage_provider<gs::storage_type::UNORDERED_SET, int, std::hash<int>, std::equal_to<int>>();
    ASSERT_TRUE(gs::is_storage_provider_v<decltype(provider), gs::storage_type::UNORDERED_SET, int, std::hash<int>, std::equal_to<int>>);
}


/**
 * @test default_storage_provider::unordered_map
 * Check that the default storage provider for unordered map types fulfils the associated requirements.
 */
TEST(default_storage_provider, unordered_map) {
    auto provider = gs::get_default_storage_provider<gs::storage_type::UNORDERED_MAP, int, int>();
    ASSERT_TRUE(gs::is_storage_provider_v<decltype(provider), gs::storage_type::UNORDERED_MAP, int, int>);
}


/**
 * @test default_storage_provider::unordered_map
 * Check that the default storage provider for unordered map types with manually specified hasher and equality fulfils the associated requirements.
 */
TEST(default_storage_provider, unordered_map_with_user_compare) {
    auto provider = gs::get_default_storage_provider<gs::storage_type::UNORDERED_MAP, int, int, std::hash<int>, std::equal_to<int>>();
    ASSERT_TRUE(gs::is_storage_provider_v<decltype(provider), gs::storage_type::UNORDERED_MAP, int, int, std::hash<int>, std::equal_to<int>>);
}
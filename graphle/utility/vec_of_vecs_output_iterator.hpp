#pragma once

#include <common.hpp>
#include <storage/storage_provider.hpp>
#include <storage/default_storage_provider.hpp>

#include <concepts>
#include <vector>


namespace graphle::util {
    /**
     * @ingruop Utils
     * An output iterator for writing to a vector-like (back-insertable) type of vector-like (back-insertable) values.
     * This iterator has the following behaviour:
     *  - Incrementing the iterator does not modify the target vector.
     *  - Dereferencing the iterator will fill the target vector by repeatedly invoking the storage provider
     *    once for every time the iterator has been incremented since the last dereference.
     *  - The first time the iterator is dereferenced after construction one extra element will be inserted using the
     *    same method described above.
     */
    template <
        typename VV,
        store::storage_provider_ref<store::storage_type::VECTOR, typename VV::value_type::value_type> PV
            = store::default_provider_t<store::storage_type::VECTOR, typename VV::value_type::value_type>
    > class vec_of_vecs_output_iterator {
        public:
            using value_type        = typename std::back_insert_iterator<typename VV::value_type>;
            using reference         = value_type;
            using pointer           = value_type*;
            using difference_type   = std::ptrdiff_t;
            using iterator_category = std::output_iterator_tag;


            explicit vec_of_vecs_output_iterator(VV& target, PV&& provider = store::get_default_storage_provider<store::storage_type::VECTOR, typename VV::value_type::value_type>()) :
                target(std::addressof(target)),
                provider(GRAPHLE_FWD(provider)),
                count(1)
            {}


            value_type operator*(void) {
                for (std::size_t i = rng::size(*target); i < count; ++i) {
                    target->push_back(provider());
                }

                return std::back_inserter(target->back());
            }


            vec_of_vecs_output_iterator& operator++(void) {
                ++count;
                return *this;
            }


            vec_of_vecs_output_iterator operator++(int) {
                auto old = *this;
                ++count;
                return old;
            }
        private:
            VV* target;
            std::remove_cvref_t<PV> provider;
            std::size_t count;
        };
}
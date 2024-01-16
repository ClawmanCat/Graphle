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
     */
    template <
        typename VV,
        store::storage_provider_ref<store::storage_type::VECTOR, typename VV::value_type> PV
    > class vec_of_vecs_output_iterator {
        public:
            using value_type        = typename std::back_insert_iterator<VV>;
            using reference         = value_type&;
            using pointer           = value_type*;
            using difference_type   = std::ptrdiff_t;
            using iterator_category = std::output_iterator_tag;


            explicit vec_of_vecs_output_iterator(VV& target, PV& provider) :
                target(std::addressof(target)),
                provider(std::addressof(provider))
            {}


            value_type operator*(void) const {
                return std::back_inserter(target->back());
            }


            vec_of_vecs_output_iterator& operator++(void) {
                target->emplace_back((*provider)());
                return *this;
            }


            vec_of_vecs_output_iterator operator++(int) {
                auto old = *this;
                target->emplace_back((*provider)());
                return old;
            }
        private:
            VV* target;
            PV* provider;
        };
}
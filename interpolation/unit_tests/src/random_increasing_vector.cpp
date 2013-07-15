/*
 * random_increasing_vector.cpp
 *
 * \date 11 avr. 2013, 08:54:58
 *  \author cec
 */

#include "random_increasing_vector.hpp"
#include "DataGenerator.hpp"

std::vector<double> a_random_vector_of_doubles_in_increasing_order_of_size(const DataGenerator& a, const size_t& n)
{
    std::vector<double> ret;
    if (n) ret.push_back(a.random<double>());
    for (size_t i = 1 ; i < n ; ++i)
    {
        const double step = a.random<double>().between(0,1000);
        ret.push_back(ret.back()+step);
    }
    return ret;
}
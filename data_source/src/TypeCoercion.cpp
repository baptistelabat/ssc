/*
 * TypeCoercion.cpp
 *
 * \date 18 juin 2013, 16:49:02
 *  \author cec
 */

#include "TypeCoercion.hpp"


void decoerce(std::list<double>& ret, bool& thing_to_convert)
{
    if (ret.empty())
    {
        THROW(__PRETTY_FUNCTION__, CoercionException, "List is empty.");
    }
    thing_to_convert = ret.front();
    ret.pop_front();
}

void decoerce(std::list<double>& ret, std::vector<bool>& thing_to_convert)
{
    if (ret.size() < thing_to_convert.size())
    {
        THROW(__PRETTY_FUNCTION__, CoercionException, "List containing new values should contain at least as many elements as vector to update");
    }
    const size_t n = thing_to_convert.size();
    for (size_t i = 0 ; i < n ; ++i)
    {
        bool b;
        decoerce(ret, b);
        thing_to_convert.at(i) = b;
    }
}



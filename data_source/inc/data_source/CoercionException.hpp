/*
 * CoercionException.hpp
 *
 * \date 18 june 2013, 14:37:43
 *  \author cec
 */

#ifndef COERCIONEXCEPTION_HPP_
#define COERCIONEXCEPTION_HPP_

#include "exception_handling/Exception.hpp"

namespace ssc
{
    namespace data_source
    {
        class CoercionException : public Exception
        {
            public:
                CoercionException(const char* s) :
                        Exception(s)
                {
                }
        };
    }
}


#endif  /* COERCIONEXCEPTION_HPP_ */
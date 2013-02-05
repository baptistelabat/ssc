/*
 * Constant.hpp
 *
 * \date 29 janv. 2013, 09:41:01
 *  \author cec
 */

#ifndef CONSTANT_HPP_
#define CONSTANT_HPP_

#include "Nullary.hpp"

/** \author cec
 *  \brief This class was created to
 *  \details It has the following responsibilities:
 *  - Resp. 1 ...
 *  \section ex1 Example
 *  \snippet MODULE_NAME/unit_tests/src/ConstantTest.cpp ConstantTest example
 *  \section ex2 Expected output
 *  \snippet MODULE_NAME/unit_tests/src/ConstantTest.cpp ConstantTest expected output
 */

class Parameter : public Nullary
{
    public:
        Parameter(const double& val);
        NodePtr diff(const StatePtr& state) const;
        bool operator==(const Parameter& rhs) const;
        bool operator!=(const Parameter& rhs) const;
        void accept(NodeVisitor& v) const;
        NodePtr clone() const;
    protected:
        std::tr1::shared_ptr<double> ptr;
        friend double& operator*(const Parameter& s);
    private:
        Parameter();
};

typedef std::tr1::shared_ptr<Parameter> ConstantPtr;

double& operator*(const Parameter& s);

#endif /* CONSTANT_HPP_ */

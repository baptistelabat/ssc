/*
 * Cos.cpp
 *
 * \date 1 f�vr. 2013, 14:40:27
 *  \author cec
 */

#include "Cos.hpp"
#include "Sin.hpp"
#include "State.hpp"
#include "Multiply.hpp"
#include "Parameter.hpp"
#include "FunctorAlgebra.hpp"
#include <cmath>
#include <string>

Cos::Cos(const NodePtr& n_) : Unary(n_)
{
    set_value([factor,n]()->double {return factor*cos(n->get_lambda()());});
}

std::string Cos::get_operator_name() const
{
    return "cos";
}

NodePtr Cos::diff(const StatePtr& state) const
{
    auto ret = (n->diff(state)*Sin(n->clone()));
    ret->multiply_by(-1);
    return ret;

    //return NodePtr(new Multiply(Mult(new Multiply(n->diff(state),ConstantPtr(new Constant(-1)))),SinPtr(new Sin(n))));
}

NodePtr Cos::clone() const
{
    return NodePtr(new Cos(*this));
}

bool Cos::is_null() const
{
    return false;
}

std::string Cos::get_type() const
{
    return "Cos";
}
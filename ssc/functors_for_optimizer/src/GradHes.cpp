/*
 * GradHes.cpp
 *
 * \date 20 f�vr. 2013, 12:21:18
 *  \author cec
 */

#include "GradHes.hpp"
#include "StateGetter.hpp"
#include "Grad.hpp"
#include "Hes.hpp"
#include "State.hpp"
#include "FunctorAlgebra.hpp"
#include "Multiply.hpp"
#include "Sum.hpp"

StateList get_states(const NodePtr& objective_function, const std::vector<NodePtr>& constraints)
{
    StateGetter retrieve_states;
    retrieve_states(objective_function);
    retrieve_states(constraints);
    return retrieve_states.get();
}

Grad grad(const NodePtr& f, const StateList& states)
{
    Grad ret;
    for (auto state = states.begin() ; state != states.end() ; ++state)
    {
        auto df_dstate = f->diff(*state);
        if (not(df_dstate->is_null()))
        {
            ret.index.push_back((*state)->get_index());
            ret.values.push_back(df_dstate->get_lambda());
        }
    }

    return ret;
}
#include "test_macros.hpp"
#include "Serialize.hpp"
Hes hes(const NodePtr& f, const std::vector<NodePtr>& g, const Parameter& sigma_f, const std::vector<Parameter>& lambda, const StateList& states)
{
    Hes ret;
    const size_t n = states.size();
    for (size_t i = 0 ; i < n ; ++i)
    {
        auto xi = states.at(i);
        for (size_t j = 0 ; j <= i ; ++j)
        {
            auto xj = states.at(j);
            NodePtr d2f_dxidxj = sigma_f*f->diff(xi)->diff(xj);
            bool d2g_is_null = true;
            for (size_t j = 0 ; j < g.size() ; ++j)
            {
                d2f_dxidxj = d2f_dxidxj + lambda.at(j)*g.at(j)->diff(xi)->diff(xj);
                if (not(g.at(j)->diff(xi)->diff(xj)->is_null())) d2g_is_null = false;
            }
            const bool term_is_zero =  (f->diff(xi)->diff(xj)->is_null()) && d2g_is_null;
            if (not(term_is_zero))
            {
                COUT(*f->diff(xi)->diff(xj)->simplify());
                ret.row_index.push_back((xi)->get_index());
                ret.col_index.push_back((xj)->get_index());
                ret.values.push_back(d2f_dxidxj->get_lambda());
            }
        }
    }
    return ret;
}
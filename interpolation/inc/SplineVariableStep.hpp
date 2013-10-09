/*
 * SplineVariableStep.hpp
 *
 * \date 9 sept. 2013, 10:47:42
 *  \author cec
 */

#ifndef SPLINEVARIABLESTEP_HPP_
#define SPLINEVARIABLESTEP_HPP_

#include "VariableStepInterpolation.hpp"
#include <vector>
#include <tr1/memory>

#include "Exception.hpp"

class SplineVariableStepException : public Exception
{
    public:
        SplineVariableStepException(const char* s) :
                Exception(s)
        {
        }
};


/** \author cec
 *  \ingroup interpolation
 *  \brief Performs a cubic spline interpolation for non-equidistant abscissae
 *  \section ex1 Example
 *  \snippet interpolation/unit_tests/src/SplineVariableStepTest.cpp SplineVariableStepTest example
 *  \section ex2 Expected output
 *  \snippet interpolation/unit_tests/src/SplineVariableStepTest.cpp SplineVariableStepTest expected output
 */

class SplineVariableStep : public VariableStepInterpolation
{
    public:
        SplineVariableStep();
        SplineVariableStep(const std::vector<double>& x, const std::vector<double>& y);

    private:
        double get_f() const;
        double get_df(const size_t derivative_order) const;

        class Impl;
        std::tr1::shared_ptr<Impl> pimpl;
};

#endif /* SPLINEVARIABLESTEP_HPP_ */
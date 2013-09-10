/*
 * SplineVariableStepTest.hpp
 *
 * \date 9 sept. 2013, 10:47:23
 *  \author cec
 */

#ifndef SPLINEVARIABLESTEPTEST_HPP_
#define SPLINEVARIABLESTEPTEST_HPP_

#include "gtest/gtest.h"
#include "DataGenerator.hpp"

class SplineVariableStepTest : public ::testing::Test
{
    protected:
        SplineVariableStepTest();
	    virtual ~SplineVariableStepTest();
        virtual void SetUp();
        virtual void TearDown();
		DataGenerator a;

};

#endif  /* SPLINEVARIABLESTEPTEST_HPP_ */

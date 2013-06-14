/*
 * ParabolicInterpolationTest.hpp
 *
 * \date 19 f�vr. 2013, 09:55:50
 *  \author cec
 */

#ifndef PARABOLICINTERPOLATIONTEST_HPP_
#define PARABOLICINTERPOLATIONTEST_HPP_

#include "gtest/gtest.h"
#include "DataGenerator.hpp"

class ParabolicInterpolationTest : public ::testing::Test
{
    protected:
        ParabolicInterpolationTest();
	    virtual ~ParabolicInterpolationTest();
        virtual void SetUp();
        virtual void TearDown();
		DataGenerator a;

};

#endif  /* PARABOLICINTERPOLATIONTEST_HPP_ */
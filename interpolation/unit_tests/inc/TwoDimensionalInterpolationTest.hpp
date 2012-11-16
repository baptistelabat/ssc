/*
 * TwoDimensionalSplinesTest.hpp
 *
 * \date 13 nov. 2012, 16:48:41
 *  \author cec
 */

#ifndef TWODIMENSIONALSPLINESTEST_HPP_
#define TWODIMENSIONALSPLINESTEST_HPP_

#include "gtest/gtest.h"
#include "DataGenerator.hpp"

class TwoDimensionalSplinesTest : public ::testing::Test
{
    protected:
        TwoDimensionalSplinesTest();
	    virtual ~TwoDimensionalSplinesTest();
        virtual void SetUp();
        virtual void TearDown();
		DataGenerator a;

};

#endif  /* TWODIMENSIONALSPLINESTEST_HPP_ */

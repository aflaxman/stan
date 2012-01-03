#include <gtest/gtest.h>
#include "stan/prob/distributions/inv_chi_square.hpp"

TEST(ProbDistributions,InvChiSquare) {
  EXPECT_FLOAT_EQ(-0.3068528, stan::prob::inv_chi_square_log(0.5,2.0));
  EXPECT_FLOAT_EQ(-12.28905, stan::prob::inv_chi_square_log(3.2,9.1));
}
TEST(ProbDistributions,InvChiSquarePropto) {
  EXPECT_FLOAT_EQ(0.0, stan::prob::inv_chi_square_log<true>(0.5,2.0));
  EXPECT_FLOAT_EQ(0.0, stan::prob::inv_chi_square_log<true>(3.2,9.1));
}
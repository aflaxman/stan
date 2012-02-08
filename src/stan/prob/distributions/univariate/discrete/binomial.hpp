#ifndef __STAN__PROB__DISTRIBUTIONS__UNIVARIATE__DISCRETE__BINOMIAL_HPP__
#define __STAN__PROB__DISTRIBUTIONS__UNIVARIATE__DISCRETE__BINOMIAL_HPP__

#include <stan/prob/constants.hpp>
#include <stan/maths/error_handling.hpp>
#include <stan/prob/traits.hpp>

namespace stan {

  namespace prob {

    // Binomial(n|N,theta)  [N >= 0;  0 <= n <= N;  0 <= theta <= 1]
    template <bool propto = false,
              typename T_prob, 
              class Policy = stan::maths::default_policy>
    inline typename boost::math::tools::promote_args<T_prob>::type
    binomial_log(const int n, 
                 const int N, 
                 const T_prob& theta, 
                 const Policy& = Policy()) {

      static const char* function = "stan::prob::binomial_log<%1%>(%1%)";
      
      using stan::maths::check_finite;
      using stan::maths::check_bounded;
      using stan::maths::check_nonnegative;
      using boost::math::tools::promote_args;
      
      typename promote_args<T_prob>::type lp(0.0);
      if (!check_bounded(function, n, 0, N,
                         "Successes, n,",
                         &lp, Policy()))
        return lp;
      if (!check_nonnegative(function, N,
                             "Population size, N,",
                             &lp, Policy()))
        return lp;
      if (!check_finite(function, theta,
                        "Probability, theta,",
                        &lp, Policy()))
        return lp;
      if (!check_bounded(function, theta, 0.0, 1.0,
                         "Probability, theta,",
                         &lp, Policy()))
        return lp;

      using stan::maths::multiply_log;
      using stan::maths::binomial_coefficient_log;
      using stan::maths::log1m;

      if (include_summand<propto>::value)
        lp += binomial_coefficient_log(N,n);
      if (include_summand<propto,T_prob>::value) 
        lp += multiply_log(n,theta)
          + (N - n) * log1m(theta);
      return lp;
    }
  }
}
#endif
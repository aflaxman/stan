#ifndef __STAN__PROB__DISTRIBUTIONS__MULTIVARIATE__CONTINUOUS__WISHART_HPP__
#define __STAN__PROB__DISTRIBUTIONS__MULTIVARIATE__CONTINUOUS__WISHART_HPP__

#include <stan/prob/constants.hpp>
#include <stan/maths/matrix_error_handling.hpp>
#include <stan/maths/error_handling.hpp>
#include <stan/prob/traits.hpp>

namespace stan {

  namespace prob {

    // Wishart(Sigma|n,Omega)  [Sigma, Omega symmetric, non-neg, definite; 
    //                          Sigma.dims() = Omega.dims();
    //                           n > Sigma.rows() - 1]
    /**
     * The log of the Wishart density for the given W, degrees of freedom, 
     * and scale matrix. 
     * 
     * The scale matrix, S, must be k x k, symmetric, and semi-positive definite.
     * Dimension, k, is implicit.
     * nu must be greater than k-1
     *
     * \f{eqnarray*}{
     W &\sim& \mbox{\sf{Wishart}}_{\nu} (S) \\
     \log (p (W \,|\, \nu, S) ) &=& \log \left( \left(2^{\nu k/2} \pi^{k (k-1) /4} \prod_{i=1}^k{\Gamma (\frac{\nu + 1 - i}{2})} \right)^{-1} 
     \times \left| S \right|^{-\nu/2} \left| W \right|^{(\nu - k - 1) / 2}
     \times \exp (-\frac{1}{2} \mathsf{tr} (S^{-1} W)) \right) \\
     &=& -\frac{\nu k}{2}\log(2) - \frac{k (k-1)}{4} \log(\pi) - \sum_{i=1}^{k}{\log (\Gamma (\frac{\nu+1-i}{2}))}
     -\frac{\nu}{2} \log(\det(S)) + \frac{\nu-k-1}{2}\log (\det(W)) - \frac{1}{2} \mathsf{tr} (S^{-1}W)
     \f}
     * 
     * @param W A scalar matrix
     * @param nu Degrees of freedom
     * @param S The scale matrix
     * @return The log of the Wishart density at W given nu and S.
     * @throw std::domain_error if nu is not greater than k-1
     * @throw std::domain_error if S is not square, not symmetric, or not semi-positive definite.
     * @tparam T_y Type of scalar.
     * @tparam T_dof Type of degrees of freedom.
     * @tparam T_scale Type of scale.
     */
    template <bool propto = false,
              typename T_y, typename T_dof, typename T_scale, 
              class Policy = stan::maths::default_policy>
    inline typename boost::math::tools::promote_args<T_y,T_dof,T_scale>::type
    wishart_log(const Eigen::Matrix<T_y,Eigen::Dynamic,Eigen::Dynamic>& W,
                const T_dof& nu,
                const Eigen::Matrix<T_scale,Eigen::Dynamic,Eigen::Dynamic>& S,
                const Policy& = Policy()) {
      static const char* function = "stan::prob::wishart_log<%1%>(%1%)";

      using stan::maths::check_greater_or_equal;
      using stan::maths::check_size_match;
      using boost::math::tools::promote_args;

      unsigned int k = W.rows();
      typename promote_args<T_y,T_dof,T_scale>::type lp;
      if (!check_greater_or_equal(function, nu, k - 1, 
                                  "Degrees of freedom, nu,", &lp, Policy()))
        return lp;
      if (!check_size_match(function, W.rows(), W.cols(), &lp, Policy()))
        return lp;
      if (!check_size_match(function, S.rows(), S.cols(), &lp, Policy()))
        return lp;
      if (!check_size_match(function, W.rows(), S.rows(), &lp, Policy()))
        return lp;
      // FIXME: domain checks
      
      using stan::maths::multiply_log;
      using stan::maths::lmgamma;
      using stan::maths::multiply;
      using stan::maths::inverse;
      using stan::maths::determinant;
      using stan::maths::trace;

      lp = 0.0;
      if (include_summand<propto,T_dof>::value)
        lp += nu * k * NEG_LOG_TWO_OVER_TWO;

      if (include_summand<propto,T_dof>::value)
        lp -= lmgamma(k, 0.5 * nu);

      if (include_summand<propto,T_dof,T_scale>::value)
        lp -= multiply_log(0.5 * nu, determinant(S)); 

      if (include_summand<propto,T_scale,T_y>::value)
        lp -= 0.5 * fabs(trace(multiply(inverse(S), W)));

      if (include_summand<propto,T_y,T_dof>::value) {
        if (nu != (k + 1)) {
          lp += multiply_log(0.5 * (nu - (k + 1.0)), determinant(W));
          // lp += 0.5 * (nu - (k + 1.0)) * log(determinant(W));
        }
      }

      return lp;
    }

  }
}
#endif
#include "ZenithEvaluation.h"

#include <complex>

#include <ceres/ceres.h>
#include <ceres/cubic_interpolation.h>

#include "interp1.h"
#include "MapToMat.h"

cv::Mat
zenith_eval(ReflectionNature method, cv::Mat polarizationDegree, double n,
            int numberOfPoints) {
  switch (method) {
    case ReflectionDiffuse:
      return zenith_eval_diffuse(polarizationDegree, n);
    case ReflectionSpecular:
      return zenith_eval_specular(polarizationDegree, n, numberOfPoints);
    case ReflectionLast:
      return cv::Mat();
  }


  return cv::Mat();
}

cv::Mat zenith_eval_diffuse(cv::Mat polarizationDegree, double n) {
  if (polarizationDegree.type() == CV_64FC1) {
    double n1 = (n - 1. / n) * (n - 1. / n);
    double n2 = (n + 1. / n) * (n + 1. / n);

    double n3 = (n * n + 1);
    double n4 = (n * n - 1) * (n * n - 1);
    return mapToMat<double>(polarizationDegree, [=](double dop) -> double {
      //aa = (n - 1 / n) ^ 2 + DOP * (n + 1 / n) ^ 2;
      //bb = 4 * DOP * (n ^ 2 + 1).*(aa - 4 * DOP);
      //cc = bb.^ 2 + 16 * DOP.^ 2.*(16 * DOP.^ 2 - aa.^ 2) * (n ^ 2 - 1) ^ 2;
      //dd = ((-bb - cc.^ (1 / 2))./ (2 * (16 * DOP.^ 2 - aa.^ 2))).^ (1 / 2);

      //theta = real(asin(dd));

      double dop4 = 4 * dop;
      double dop4Sq = dop4 * dop4;

      std::complex aa = n1 + dop * n2;
      std::complex bb = dop4 * n3 * (aa - dop4);
      std::complex cc = bb * bb + dop4Sq * (dop4Sq - aa * aa) * n4;
      std::complex dd = std::sqrt(
          (-bb - std::sqrt(cc)) / (2. * (dop4Sq - aa * aa)));

      return std::asin(dd).real();
    });
  } else {
    return cv::Mat();
  }
}

cv::Mat zenith_eval_diffuse_fast(cv::Mat polarizationDegree, double n) {
  if (polarizationDegree.type() == CV_64FC1) {
    double n1 = (n - 1. / n) * (n - 1. / n);
    double n2 = (n + 1. / n) * (n + 1. / n);

    double n3 = (n * n + 1);
    double n4 = (n * n - 1) * (n * n - 1);
    return mapToMat<double>(polarizationDegree, [=](double dop) -> double {
      //aa = (n - 1 / n) ^ 2 + DOP * (n + 1 / n) ^ 2;
      //bb = 4 * DOP * (n ^ 2 + 1).*(aa - 4 * DOP);
      //cc = bb.^ 2 + 16 * DOP.^ 2.*(16 * DOP.^ 2 - aa.^ 2) * (n ^ 2 - 1) ^ 2;
      //dd = ((-bb - cc.^ (1 / 2))./ (2 * (16 * DOP.^ 2 - aa.^ 2))).^ (1 / 2);

      //theta = real(asin(dd));

      if (dop < std::numeric_limits<double>::epsilon()) { // as dop == 0
        return std::asin(0);
      }

      double dop4 = 4 * dop;
      double dop4Sq = dop4 * dop4;

      double aa = n1 + dop * n2;
      double bb = dop4 * n3 * (aa - dop4);
      double cc = bb * bb + dop4Sq * (dop4Sq - aa * aa) * n4;
      double dd = std::sqrt((-bb - std::sqrt(cc)) / (2. * (dop4Sq - aa * aa)));
      return std::asin(dd);
    });
  } else {
    return cv::Mat();
  }
}

cv::Mat
zenith_eval_specular(cv::Mat polarizationDegree, double n, int numberOfPoints) {
  cv::Mat result;
  if (polarizationDegree.type() == CV_64FC1) {
    double argMax = argmax(n);
    cv::Mat dopNorm = mapToMat<double>(polarizationDegree,
                                       [=](double x) -> double {
                                         if (x >
                                             std::numeric_limits<double>::epsilon()) {
                                           if (argMax - x >
                                               std::numeric_limits<double>::epsilon())
                                             return x;
                                           else
                                             return argMax;
                                         } else
                                           return 0;
                                       });

    std::vector<double> xLeft, yLeft, xRight, yRight;
    fillInterpolationTables(argMax, n, numberOfPoints, xLeft, yLeft, xRight,
                            yRight);
    cv::Mat leftSolution = interp1(yLeft, xLeft, dopNorm);

    cv::Mat rightSolution = interp1(yRight, xRight, dopNorm);


    cv::merge(std::vector<cv::Mat>{leftSolution, rightSolution}, result);
  }
  return result;
}

struct SpecularReflectionModelCostFunctor {
  SpecularReflectionModelCostFunctor(double n) : n(n) {
  }

  template<typename T>
  bool operator()(const T *const x, T *residual) const {
    residual[0] = specularReflectionModel(n, x[0]) - T(1);
    return true;
  }

private:
  double n;
};

double argmax(double n) {
  double x = M_PI / 4;
  ceres::Problem problem;
  ceres::CostFunction *cost_function =
      new ceres::AutoDiffCostFunction<SpecularReflectionModelCostFunctor, 1, 1>(
          new SpecularReflectionModelCostFunctor(n));
  problem.AddResidualBlock(cost_function, NULL, &x);
  problem.SetParameterLowerBound(&x, 0, 0);
  problem.SetParameterUpperBound(&x, 0, M_PI / 2);
  // Run the solver!
  ceres::Solver::Options options;
  options.minimizer_progress_to_stdout = false;
  ceres::Solver::Summary summary;
  Solve(options, &problem, &summary);
  return x;
}

void fillInterpolationTables(double argMax, double n, int numberOfPoints,
                             std::vector<double> &xLeft,
                             std::vector<double> &yLeft,
                             std::vector<double> &xRight,
                             std::vector<double> &yRight) {


  xLeft.clear();
  yLeft.clear();
  xRight.clear();
  yRight.clear();
  xLeft.reserve(numberOfPoints);
  yLeft.reserve(numberOfPoints);
  xRight.reserve(numberOfPoints);
  yRight.reserve(numberOfPoints);

  xLeft.push_back(0);
  yLeft.push_back(specularReflectionModel(n, 0));
  xRight.push_back(argMax);
  yRight.push_back(specularReflectionModel(n, argMax));

  double xRightDelta = (M_PI / 2 - argMax) / (numberOfPoints - 1);
  double xLefDelta = argMax / (numberOfPoints - 1);
  for (int i = 1; i < numberOfPoints; ++i) {
    xLeft.push_back(i * xLefDelta);
    yLeft.push_back(specularReflectionModel(n, i * xLefDelta));
    xRight.push_back(argMax + i * xRightDelta);
    yRight.push_back(specularReflectionModel(n, argMax + i * xRightDelta));
  }
  std::reverse(std::begin(xRight), std::end(xRight));
  std::reverse(std::begin(yRight), std::end(yRight));
}



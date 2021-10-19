#ifndef ZENITH_EVALUATION_H
#define ZENITH_EVALUATION_H

#include "dclalgo_export.h"
#include <opencv2/core.hpp>

enum ReflectionNature {
  ReflectionDiffuse,
  ReflectionSpecular,
  ReflectionLast
};

DCLALGO_EXPORT cv::Mat
zenith_eval(ReflectionNature method, cv::Mat polarizationDegree, double n = 1.3,
            int numberOfPoints = 2000);
DCLALGO_EXPORT cv::Mat
zenith_eval_diffuse(cv::Mat polarizationDegree, double n);
DCLALGO_EXPORT cv::Mat
zenith_eval_diffuse_fast(cv::Mat polarizationDegree, double n);
DCLALGO_EXPORT cv::Mat
zenith_eval_specular(cv::Mat polarizationDegree, double n, int numberOfPoints);
template<typename T>
DCLALGO_EXPORT T specularReflectionModel(double n, T x) {
  //(2*sin(x).^2 .* cos(x) .* sqrt(n^2 - sin(x).^2)) ./ (n^2 - sin(x).^2 - n^2 * sin(x).^2 + 2 * sin(x).^4)
  T sinSq = sin(x) * sin(x);
  return (T(2) * sinSq * cos(x) * sqrt(n * n - sinSq)) /
         (n * n - sinSq - n * n * sinSq + T(2) * sinSq * sinSq);
}
DCLALGO_EXPORT double argmax(double n);
DCLALGO_EXPORT void
fillInterpolationTables(double argMax, double n, int numberOfPoints,
                        std::vector<double> &xLeft, std::vector<double> &yLeft,
                        std::vector<double> &xRight,
                        std::vector<double> &yRight);
#endif //SFP_ZENITH_EVAL_H

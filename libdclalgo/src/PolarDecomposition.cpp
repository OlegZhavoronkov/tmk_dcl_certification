#include "PolarDecomposition.h"
#include "MapToMat.h"

#include <cmath>

cv::Mat
pixelModel(const cv::Mat &I_max, const cv::Mat &I_min, const cv::Mat &phi,
           const cv::Mat &phi_pol) {
  //(I_max + I_min)/2 + (I_max - I_min)/2 .* cos(2*(phi_pol - phi));
  return (I_max + I_min) / 2
         + ((I_max - I_min) / 2).mul(
      mapToMat<double>(cv::Mat(2 * (phi_pol - phi)),
                       [](double x) -> double { return cos(x); }));
}

cv::Mat diffuseModel(double n, const cv::Mat &theta) {
  //((n-1/n)^2 * sin(x).^2) ./ (2 + 2*n^2 - (n + 1/n)^2 * sin(x).^2 + 4 * cos(x) .* sqrt(n^2 - sin(x).^2));
  return mapToMat<double>(theta, [=](double x) -> double {
    return (pow((n - 1 / n), 2) * pow(sin(x), 2))
           / ((2 + 2 * pow(n, 2) - pow((n + 1 / n), 2) * pow(sin(x), 2) +
               4 * cos(x) * sqrt(pow(n, 2) - pow(sin(x), 2))));
  });
}

cv::Mat specularModel(double n, const cv::Mat &theta) {
  //(2*sin(x).^2 .* cos(x) .* sqrt(n^2 - sin(x).^2)) ./ (n^2 - sin(x).^2 - n^2 * sin(x).^2 + 2 * sin(x).^4);
  return mapToMat<double>(theta, [=](double x) -> double {
    return (2 * pow(sin(x), 2) * cos(x) * sqrt(pow(n, 2) - pow(sin(x), 2)))
           / (pow(n, 2) - pow(sin(x), 2) - pow(n, 2) * pow(sin(x), 2) +
              2 * pow(sin(x), 4));
  });
}

cv::Mat iMin(const cv::Mat &iMax, const cv::Mat rho) {
  //Imax_spec .* (1-rho_spec) ./ (1+rho_spec);
  return iMax.mul(1 - rho) / (1 + rho);
}

std::vector<cv::Mat>
decomposeImages(const cv::Mat iMax, const cv::Mat &phi, const cv::Mat &rho,
                const std::vector<double> angles) {
  std::vector<cv::Mat> result;
  for (double angle: angles) {
    result.push_back(pixelModel(iMax, iMin(iMax, rho), phi,
                                cv::Mat(iMax.size(), CV_64FC1, angle)));
  }
  return result;
}

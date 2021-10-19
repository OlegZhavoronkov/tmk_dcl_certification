#ifndef INTERP1_H
#define INTERP1_H

#include "dclalgo_export.h"

#include <vector>
#include <opencv2/core/mat.hpp>


DCLALGO_EXPORT std::vector<double>
interp1(const std::vector<double> &x, const std::vector<double> &y,
        const std::vector<double> &xNew);
DCLALGO_EXPORT cv::Mat
interp1(const std::vector<double> &x, const std::vector<double> &y,
        const cv::Mat &xNew);
#endif // INTERP1_H

#ifndef _CVFILTER_H_
#define _CVFILTER_H_

#include <stddef.h>

#ifdef __cplusplus

#include <opencv2/core.hpp>
#include <opencv2/highgui.hpp>

float findRadius(cv::Mat image);

extern "C" {
#endif

//c-interface for cv:Mat image
float find_radius(size_t rows, size_t cols, int channels, void *cv_image);

#ifdef __cplusplus
}
#endif

#endif //_CVFILTER_H_



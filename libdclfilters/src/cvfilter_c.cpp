#include <dclfilters/cvfilter.h>
#include <dclfilters/cvtools.h>

extern "C" float find_radius(size_t rows, size_t cols, int channels, void *cv_image) {
    cv::Mat img = restoreImage(rows, cols, channels, cv_image);
    return findRadius(img);
}


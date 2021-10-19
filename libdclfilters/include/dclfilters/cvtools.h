#ifndef _CVTOOLS_H_
#define _CVTOOLS_H_

#include <stddef.h>

#ifdef __cplusplus

namespace cv {
    class Mat;
};

cv::Mat restoreImage(size_t rows, size_t cols, int channels, void *frame_data);

extern "C" {
#endif

void create_CV_8U_m(void **pm, size_t rows, size_t cols, int channels);
void restore_CV_8U_m(void **pm, size_t rows, size_t cols, int channels, void *frame_data);
void delete_m(void **pm);

void save_cv_image(const char* outfname, size_t rows, size_t cols, int channels, void *frame_data);
void convert_from_rawfile(const char *rawfname, size_t rows, size_t cols, int channels, const char *outfname);

#ifdef __cplusplus
}
#endif

#endif // _CVTOOLS_H_

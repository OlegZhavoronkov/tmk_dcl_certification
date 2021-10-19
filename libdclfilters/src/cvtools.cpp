#include <stdio.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <fcntl.h>
#include <unistd.h>

#include <opencv2/core.hpp>
#include <opencv2/highgui.hpp>

cv::Mat restoreImage(size_t rows, size_t cols, int channels, void *frame_data){
    cv::Mat img(rows, cols, CV_MAKETYPE(CV_8U, channels), frame_data);
    return img;
}

/*
    cv::Mat *m = new cv::Mat(cv::Mat::zeros(height, width, CV_32F));
    ...
    delete m;
*/
//---------------------------------------------------------------
extern "C" void create_CV_8U_m(void **pm, size_t rows, size_t cols, int channels) {
    if (pm == NULL) return;
    cv::Mat *m = new cv::Mat(cv::Mat::zeros(rows, cols, CV_MAKETYPE(CV_8U, channels)));
    *pm = m;
}

extern "C" void restore_CV_8U_m(void **pm, size_t rows, size_t cols, int channels, void *frame_data) {
    if (pm == NULL) return;
    cv::Mat *m = new cv::Mat(rows, cols, CV_MAKETYPE(CV_8U, channels), frame_data);
    *pm = m;
}

extern "C" void delete_m(void **pm) {
    if (pm == NULL || *pm == NULL ) return;
    cv::Mat *m = (cv::Mat *)(*pm);
    delete m;
    *pm = NULL;
}
//-----------------------------------------------------

extern "C" void save_cv_image(const char* outfname, size_t rows, size_t cols, int channels, void *frame_data) {
    cv::Mat img = restoreImage(rows, cols, channels, frame_data);
    cv::imwrite(outfname, img);
}

//TODO: use filereader(hide version) and convert from dir with openmp
extern "C" void convert_from_rawfile(const char *rawfname, size_t rows, size_t cols, int channels, const char *outfname) {
    struct stat st;
    stat(rawfname, &st);
    char *frame_data = (char*)malloc(st.st_size);
    int in_fd = open(rawfname, O_RDONLY, 0600);
    if (in_fd > 0) {
        read(in_fd, frame_data, st.st_size);
        close(in_fd);
    }
    save_cv_image(outfname,rows, cols, channels, frame_data);
}


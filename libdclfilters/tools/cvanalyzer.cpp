#include <opencv2/core.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/imgproc.hpp>
#include <opencv2/core/types.hpp>

#include <iostream>

std::string type2str(int type) {
  std::string r;

  uchar depth = type & CV_MAT_DEPTH_MASK;
  uchar chans = 1 + (type >> CV_CN_SHIFT);

  switch ( depth ) {
    case CV_8U:  r = "8U"; break;
    case CV_8S:  r = "8S"; break;
    case CV_16U: r = "16U"; break;
    case CV_16S: r = "16S"; break;
    case CV_32S: r = "32S"; break;
    case CV_32F: r = "32F"; break;
    case CV_64F: r = "64F"; break;
    default:     r = "User"; break;
  }

  r += "C";
  r += (chans+'0');

  return r;
}

int main(int argc, char *argv[]) {
    if (argc < 2) {
        std::cout << "Usage: " << argv[0] << " in" << std::endl; 
    }
    
    cv::Mat orig = cv::imread(argv[1], cv::IMREAD_ANYDEPTH |
                                                  cv::IMREAD_ANYCOLOR);
    int origSize = orig.total() * orig.elemSize();
    std::cout << "origSize: " << origSize << std::endl; 

    std::cout << "cols: "<< orig.cols << " rows: " <<  orig.rows << std::endl;

    cv::Size s = orig.size();
    std::cout << "width: " <<  s.width << " height: "<< s.height << std::endl;

    std::string ty =  type2str(orig.type());
    std::cout << "type: " << ty.c_str() << std::endl;

#if 0
    cv::imwrite("../data/origin.bmp", orig);
#endif

    return 0;
}

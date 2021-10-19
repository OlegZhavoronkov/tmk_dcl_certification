#include <opencv2/core.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/imgproc.hpp>
#include <opencv2/core/types.hpp>

#include <opencv2/opencv.hpp>

#include <iostream>

int main(int argc, char *argv[]) {
    if (argc < 2) {
        std::cout << "Usage: " << argv[0] << " in file {scale_percent}" << std::endl; 
    }
    
    cv::Mat orig = cv::imread(argv[1], cv::IMREAD_ANYDEPTH |
                                                  cv::IMREAD_ANYCOLOR);

    //cv::namedWindow(argv[1], cv::WINDOW_NORMAL | cv::WINDOW_KEEPRATIO | cv::WINDOW_GUI_EXPANDED);

    int scale_percent = 20;
    if (argc > 2) {
        scale_percent = atoi(argv[2]);
    }
    int down_width = orig.cols * scale_percent / 100;
    int down_height = orig.rows * scale_percent / 100;
    cv::Mat resized_down;
    cv::resize(orig, resized_down, cv::Size(down_width, down_height), cv::INTER_LINEAR);

    // Display the image.
    cv::imshow(argv[1], resized_down);
    // Wait for a keystroke.  
    cv::waitKey(0); 
    // Destroys all the windows created
    cv::destroyAllWindows();

#if 0
    cv::imwrite("../data/origin.bmp", orig);
#endif

    return 0;
}


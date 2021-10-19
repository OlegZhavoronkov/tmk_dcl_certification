//------------------------------------------------------------------------------
//  Created     : 29.04.2021
//  Author      : Victor Kats
//  Description : Demonstration of image capture thread with aravis
//------------------------------------------------------------------------------

#include"arv_capture_thread.h"

#include <opencv2/core.hpp>
#include <opencv2/highgui.hpp>

#include <X11/Xlib.h>

#include <cstdio>
#include <strstream>


const char window[] = "arv_capture_thread Demo";

void show_frame(ArvBuffer *buffer, CaptureThreadStatistics stat) {
    printf("Processing frame %llu\n", stat.camulative_buffer_count+stat.buffer_count);
    printf("%lu\n", arv_buffer_get_timestamp(buffer));
    size_t bytes;
    void * data= (void*)(arv_buffer_get_data(buffer, &bytes));
    cv::Mat img(arv_buffer_get_image_height(buffer), arv_buffer_get_image_width(buffer), CV_8UC1, data);
    //cv::imshow(window, img);
    std::stringstream s;
    s << "/tmp/tmk/" << arv_buffer_get_system_timestamp(buffer) << ".pgm";
    cv::imwrite(s.str(), img);
    //cv::waitKey(1);
}

int main(int argc, char *argv[]) {
    XInitThreads();
    GError *error = NULL;

    ArvCamera *camera = arv_camera_new (NULL, &error);

    if (!ARV_IS_CAMERA (camera)) {
        fprintf (stderr, "No camera found%s%s\n",
                error != NULL ? ": " : "",
                error != NULL ? error->message : "");
        g_clear_error (&error);
    }

    cv::namedWindow(window);
    struct CaptureThreadData *data = arv_start_capture_thread(camera, show_frame, NULL, NULL);

    int go = 1;
    while(go) {
        int key = cv::waitKey(1);
        printf("key:%d\n", key);
        if (key == 27) {
            arv_stop_capture_thread(data);
            go = 0;
        }
    }

    data->camera = NULL;
    g_object_unref (data->stream);

    return 0;
}
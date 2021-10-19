#include "MainWindow.h"

#include <QApplication>

#include <opencv2/highgui.hpp>
#include <opencv2/calib3d.hpp>
#include <opencv2/imgproc.hpp>

#include <camera_models/PinHoleCamera.h>
#include <flat_pattern_calibration/CameraCalibration.h>
#include <flat_pattern_calibration/FlatPattern.h>

int main(int argc, char *argv[])
{
    if (false) {
        QApplication a(argc, argv);
        MainWindow w;
        w.show();
        return a.exec();
    }
    FlatPatterns pattern;
    PinHoleCalibrationParams pinholeParams;
    return 0;
}

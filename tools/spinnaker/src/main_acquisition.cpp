//------------------------------------------------------------------------------
//  Created     : 06.08.21
//  Author      : Victor Kats
//  Description : Import Spinnnaker-based capturer into project (see TMKDCL-380)
//------------------------------------------------------------------------------

#include "captureManager.h"
#include "configparser.h"
#include "configureCameras.h"

#include <QApplication>

#include <ctime>
#include <filesystem>
#include <iostream>
#include <iomanip>
#include <thread>
#include <vector>

#include <unistd.h>
#include <termios.h>

using namespace Spinnaker;
using namespace Spinnaker::GenApi;
using namespace Spinnaker::GenICam;

char *getCmdOption(char **begin, char **end, const std::string &option) {
    char **itr = std::find(begin, end, option);
    if (itr != end && ++itr != end) {
        return *itr;
    }
    return 0;
}

bool cmdOptionExists(char **begin, char **end, const std::string &option) {
    return std::find(begin, end, option) != end;
}

bool parseCommandLine(int argc, char *argv[], std::string &configurationName, std::string &storagePath
                      , bool &weCalibrate, bool &showGUI, int &guiFPS) {
    if (cmdOptionExists(argv, argv + argc, "-h")
    || !cmdOptionExists(argv, argv + argc, "-c")
    || !cmdOptionExists(argv, argv + argc, "-p")) {
        std::cout << "Use -c $CONFIGNAME$ to specify configuration name" << std::endl;
        std::cout << "Use -p $PATH$ to specify location to store frames" << std::endl;
        std::cout << "Use --calibrate to run 'calibration' mode: preview GUI enabled"
                     " no frame will be stored until Enter pressed" << std::endl;
        std::cout << "Use --preview to enable preview GUI" << std::endl;
        std::cout << "Use --preview-fps $FPS$ to limit preview refresh fps" << std::endl;
        return false;
    }
    weCalibrate = false;
    showGUI = false;
    if(cmdOptionExists(argv, argv + argc, "--calibrate")) {
        weCalibrate = true;
        showGUI = true;
    }
    if(cmdOptionExists(argv, argv + argc, "--preview")) {
        showGUI = true;
    }
    if(showGUI && cmdOptionExists(argv, argv + argc, "--preview-fps")) {
        //std::string fps(getCmdOption(argv, argv + argc, "--preview-fps"));
        try {
            guiFPS = std::stoi(getCmdOption(argv, argv + argc, "--preview-fps"));
        }
        catch(...) {
            guiFPS = -1;
        }
    }

    std::filesystem::path confPath(getCmdOption(argv, argv + argc, "-c"));
    std::filesystem::path storPath(getCmdOption(argv, argv + argc, "-p"));
    std::error_code ec;
    if(!std::filesystem::exists(confPath, ec)) {
        std::cout << "Configuration file is not exists or is unacceptable" << std::endl;
        std::cout << "Error code is " << ec.value() << std::endl;
        return false;
    }
    if(!std::filesystem::is_regular_file(confPath, ec)) {
        std::cout << "Configuration file path is not pointing to the file" << std::endl;
        std::cout << "Error code is " << ec.value() << std::endl;
        return false;
    }

    if(!std::filesystem::exists(storPath, ec)) {
        std::cout << "Storage path is not exists or is unacceptable" << std::endl;
        std::cout << "Error code is " << ec.value() << std::endl;
        return false;
    }
    if(!std::filesystem::is_directory(storPath, ec)) {
        std::cout << "Specified storage path is not pointing to the directory" << std::endl;
        std::cout << "Error code is " << ec.value() << std::endl;
        return false;
    }
    configurationName = std::filesystem::canonical(confPath, ec);
    storagePath = std::filesystem::canonical(storPath, ec);
    return true;
}

char getch() {
    char buf = 0;
    struct termios old = {0};
    if (tcgetattr(0, &old) < 0)
        perror("tcsetattr()");
    old.c_lflag &= ~ICANON;
    old.c_lflag &= ~ECHO;
    old.c_cc[VMIN] = 1;
    old.c_cc[VTIME] = 0;
    if (tcsetattr(0, TCSANOW, &old) < 0)
        perror("tcsetattr ICANON");
    if (read(0, &buf, 1) < 0)
        perror ("read()");
    old.c_lflag |= ICANON;
    old.c_lflag |= ECHO;
    if (tcsetattr(0, TCSADRAIN, &old) < 0)
        perror ("tcsetattr ~ICANON");
    return (buf);
}

#include <X11/Xlib.h>
int main(int argc, char **argv) {
    int result = 0;
    int frameNum = 1;
    std::string config;//= parseCommandLine(argc, argv, frameNum, <#initializer#>);
    std::string storage;
    bool calibrationMode;
    bool showGUI;
    int guiFPS;
    if(!parseCommandLine(argc, argv, config, storage, calibrationMode, showGUI, guiFPS)) {
        return 1;
    }
    const std::vector<CameraConfig> camerasList = ConfigParser::getCamerasList(config);
    const std::vector<TriggerConfig> triggersList = ConfigParser::getTriggerList(config);

    CaptureManager capMan(!calibrationMode);
    auto t = std::time(nullptr);
    auto tm = *std::localtime(&t);

    std::ostringstream oss;
    oss << std::put_time(&tm, "%Y%m%d%H%M%S");

    storage = storage + std::filesystem::path::preferred_separator + oss.str();
    auto backup = storage;
    int tryNo = 0;
    while (std::filesystem::exists(storage)) {
        storage = backup + "_" + std::to_string(tryNo++);
    }
    std::filesystem::create_directory(storage);
    std::cout << "Frames will be stored to \n\t" << storage << std::endl;
    for (int i = 0; i < camerasList.size(); ++i) {
        std::string storPath =
                storage + std::filesystem::path::preferred_separator + std::to_string(camerasList[i].serial);
        auto backup = storPath;
        int tryNo = 0;
        while (std::filesystem::exists(storPath)) {
            storPath = backup + "_" + std::to_string(tryNo++);
        }
        std::filesystem::create_directory(storPath);
        capMan.addCamera(camerasList[i], storPath);
    }
    CaptureThread::DoCapture.store(true);
    //std::thread managerThread([&](){capMan();});
    if(!showGUI) {
        std::thread managerThread([&]() { capMan.run(); });
        while (true) {
            auto x = getch(); ///TODO: here should be listener to UI
            if (x == 27) {
                break;
            }
        }
        CaptureThread::DoCapture.store(false);
        std::this_thread::sleep_for(std::chrono::seconds(2));
        managerThread.join();
    }
    else {
        XInitThreads();
        QApplication a(argc, argv);
        PreviewWidget *preview = new PreviewWidget(capMan.getCameraNumber());
        preview->show();
        preview->dropPreview();

        preview->connect(preview, &PreviewWidget::doCapture, [&](){ capMan.storeOnce(); });

        preview->connect(preview, &PreviewWidget::doTerminate, [&](){ QApplication::quit(); });

        preview->setWindowTitle(calibrationMode ? "Press Enter to capture or Esc to exit":"Press Esc to exit");

        capMan.setGUI(preview);

        if(guiFPS >= 1) {
            capMan.setGUIRefreshFPS(guiFPS);
        }
        std::thread managerThread([&]() { capMan.run(); });
        QApplication::exec();
        CaptureThread::DoCapture.store(false);
        std::this_thread::sleep_for(std::chrono::seconds(2));
        managerThread.join();
        delete preview;
    }
    CaptureThread::SpinCamList.Clear();
    CaptureThread::SpinSystem->ReleaseInstance();
}

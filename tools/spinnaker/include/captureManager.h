//------------------------------------------------------------------------------
//  Created     : 06.08.21
//  Author      : Victor Kats
//  Description : Import Spinnnaker-based capturer into project (see TMKDCL-380)
//------------------------------------------------------------------------------

//
// Created by victor on 28.01.2020.
//

#ifndef GTS_IDENTIFICATION_CAPTUREMANAGER_H
#define GTS_IDENTIFICATION_CAPTUREMANAGER_H

#include <atomic>
#include <memory>
#include <thread>
#include <vector>

#include <Spinnaker.h>
#include <SpinGenApi/SpinnakerGenApi.h>

#include "captureThread.h"
#include "configparser.h"

#include "PreviewWidget.h"


class CaptureManager {
public:
    CaptureManager(bool storeStream);
    ~CaptureManager();

    //virtual ~CaptureManager();

    bool addCamera(const CameraConfig& newCamera, const std::string& storage);

    void runThreads();

    void run();

    void storeOnce();

    void setGUI(PreviewWidget *ui);

    int getCameraNumber();

    void setGUIRefreshFPS(int fps);

    void stop();
private:
    bool mCapturing;

    std::vector<CameraConfig> mCameras;
    std::vector<std::string> mStorages;

    std::vector<std::shared_ptr<CaptureThread>> mThreads;
    std::vector<std::thread> mThreadKeepers;

    std::mutex mBeatGuard;

    std::vector<bool> mFalses;
    std::vector<bool> mTrues;

    PreviewWidget *mUi;

    bool mPredVal;

    std::atomic<bool> mStoreStream;
    std::atomic<bool> mStoreThisBeat;

    int mFps;
};


#endif //GTS_IDENTIFICATION_CAPTUREMANAGER_H

//------------------------------------------------------------------------------
//  Created     : 06.08.21
//  Author      : Victor Kats
//  Description : Import Spinnnaker-based capturer into project (see TMKDCL-380)
//------------------------------------------------------------------------------

//
// Created by victor on 28.01.2020.
//

#ifndef GTS_IDENTIFICATION_CAPTURETHREAD_H
#define GTS_IDENTIFICATION_CAPTURETHREAD_H

#include <atomic>
#include <condition_variable>
#include <thread>

#include <Spinnaker.h>
#include <SpinGenApi/SpinnakerGenApi.h>

#include "configparser.h"

class CaptureThread {
public:
    explicit CaptureThread(const CameraConfig& camera, const std::string& storage, uint index = 0);
    //static std::atomic<uint> CapturedThisTime;
    //static std::atomic<uint> ToBeCapturedAtOnce;

    //static std::atomic<bool> CaptureAll;
    static Spinnaker::SystemPtr SpinSystem;
    static Spinnaker::CameraList SpinCamList;

    static std::atomic<bool> StoreFrames;
    static std::atomic<bool> DoCapture;

    static std::vector<bool> CaptureScheduled;
    static std::mutex ScheduleGuard;

    static std::atomic<unsigned long long> BeatNo;


    static std::condition_variable_any MasterBeatWait;

    void operator()();
    void wakeUp();
private:
    CameraConfig mCameraConfig;
    //Spinnaker::CameraPtr mCamera;



    ulong mCycles;

    uint mIndex;
    std::string mStorage;

    std::mutex mBeatGuard;

    std::condition_variable mBeatWait;

    bool mPredVal;
};


#endif //GTS_IDENTIFICATION_CAPTURETHREAD_H

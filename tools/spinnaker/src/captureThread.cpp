//------------------------------------------------------------------------------
//  Created     : 06.08.21
//  Author      : Victor Kats
//  Description : Import Spinnnaker-based capturer into project (see TMKDCL-380)
//------------------------------------------------------------------------------

//
// Created by victor on 28.01.2020.
//

#include "captureThread.h"
#include "configureCameras.h"

#include <iomanip>
#include <filesystem>

Spinnaker::SystemPtr CaptureThread::SpinSystem = Spinnaker::System::GetInstance();
Spinnaker::CameraList CaptureThread::SpinCamList = SpinSystem->GetCameras();

std::atomic<bool> CaptureThread::StoreFrames = std::atomic<bool>(false);
std::atomic<bool> CaptureThread::DoCapture = std::atomic<bool>(true);

std::vector<bool> CaptureThread::CaptureScheduled = {};
std::mutex CaptureThread::ScheduleGuard = std::mutex();

std::condition_variable_any CaptureThread::MasterBeatWait;

//std::mutex CaptureThread::mBeatGuard;

//std::condition_variable_any CaptureThread::mBeatWait;


std::atomic<unsigned long long> CaptureThread::BeatNo = std::atomic<unsigned long long>(0);

CaptureThread::CaptureThread(const CameraConfig& camera, const std::string& storage, uint index)
        : mCameraConfig(camera)
        , mIndex(index)
        , mStorage(storage)
        , mCycles(0) {
    std::cout << mIndex << " -> " << mCameraConfig.serial << std::endl;
    //CaptureScheduled[index] = true;
}

void CaptureThread::operator()() {
    if (mCameraConfig.sdk == "spin") {
        Spinnaker::CameraPtr mCamera = SpinCamList.GetBySerial(std::to_string(mCameraConfig.serial));
        std::cout << "Waiting " << mIndex << " to wake up" << std::endl;
        {
            std::unique_lock<std::mutex> scheduleLock(ScheduleGuard);
            CaptureScheduled[mIndex] = false;
        }
        while (DoCapture.load()) {
            std::unique_lock<std::mutex> lk(mBeatGuard);
            mBeatWait.wait(lk, [this]() {
                std::cout << "Try unlock " << mIndex << "  " << mCameraConfig.serial << std::endl;
                {
                    std::unique_lock<std::mutex> scheduleLock(ScheduleGuard);
                    mPredVal = CaptureScheduled[mIndex];
                }
                std::cout <<  mIndex << " " << mPredVal << std::endl;
                return mPredVal;
            });

            if(!mCamera->IsInitialized())
            {
                mCamera->Init();

                for (auto prop: mCameraConfig.properties) {

                    if (prop.type == "enum")
                        setEnumerationProperty(prop.name, prop.value, mCamera);

                    if (prop.type == "float")
                        setFloatProperty(prop.name, stof(prop.value), mCamera);

                    if (prop.type == "bool")
                        setBoolProperty(prop.name, stob(prop.value), mCamera);

                    if (prop.type == "int")
                        Spinnaker::GenICam::setIntProperty(prop.name, std::stoi(prop.value), mCamera);
                }

                mCamera->BeginAcquisition();
            }

            Spinnaker::ImagePtr pResultImage = mCamera->GetNextImage();

            int incomplete_counter = 0;
            while (pResultImage->IsIncomplete()) {
                incomplete_counter += 1;
                if (incomplete_counter < 2) {
                    std::cout << "Image is incomplete";
                }
                pResultImage->Release();
                pResultImage = mCamera->GetNextImage();
            }
            if (StoreFrames.load()) {
                Spinnaker::ImagePtr detouchedImage = pResultImage->Convert(mCamera->PixelFormat());
                pResultImage->Release();
                std::stringstream path;
                path << mStorage << std::filesystem::path::preferred_separator
                     << std::setw(10) << std::setfill('0') << BeatNo
                     << ".jpg";
                detouchedImage->Save(path.str().c_str());
            } else {
                pResultImage->Release();
            }
            {
                std::unique_lock<std::mutex> scheduleLock(ScheduleGuard);
                //for(int i = 0; i < CaptureThread::CaptureScheduled.size(); ++i)
                //    std::cout << CaptureThread::CaptureScheduled[i];
                //std::cout << std::endl;
                CaptureScheduled[mIndex] = false;
            }
            MasterBeatWait.notify_one();
        }
        mCamera->EndAcquisition();
        mCamera->DeInit();
    }
}

void CaptureThread::wakeUp() {
    std::cout << "Triggered " << mIndex << std::endl;
    mBeatWait.notify_one();
}

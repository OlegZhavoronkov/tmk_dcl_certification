//------------------------------------------------------------------------------
//  Created     : 06.08.21
//  Author      : Victor Kats
//  Description : Import Spinnnaker-based capturer into project (see TMKDCL-380)
//------------------------------------------------------------------------------

//
// Created by victor on 28.01.2020.
//

#include "captureManager.h"

#include <chrono>
#include <filesystem>

#include "configureCameras.h"

CaptureManager::CaptureManager(bool storeStream)
        : mCapturing(false)
        , mStoreStream(storeStream)
        , mUi(nullptr)
        , mFps(-1)
        {}

bool CaptureManager::addCamera(const CameraConfig& newCamera, const std::string& storage) {
    if (mCapturing)
        return false;
    mCameras.push_back(newCamera);
    mStorages.push_back(storage);
    return true;
}

void CaptureManager::runThreads() {
    if(!mCameras.size())
        return;
    mCapturing = true;
    mFalses = std::vector<bool>(mCameras.size(), false);
    mTrues = mFalses;
    mTrues.flip();

    {
        std::unique_lock<std::mutex> scheduleLock(CaptureThread::ScheduleGuard);
        CaptureThread::CaptureScheduled = mTrues;
    }

    for(int i = 0; i < mCameras.size(); ++i)  {
        mThreads.emplace_back(new CaptureThread(mCameras[i], mStorages[i], i));
        mThreadKeepers.emplace_back([this, i](){(*mThreads[i])();});
    }

    for(int i = 0; i < 3*mCameras.size(); ++i){
        std::this_thread::sleep_for(std::chrono::seconds(1));
        {
            std::unique_lock<std::mutex> scheduleLock(CaptureThread::ScheduleGuard);
            mPredVal = CaptureThread::CaptureScheduled == mFalses;
        }
        if(mPredVal)
            break;
        else if(i == 3*mCameras.size() -1) {
            std::stringstream ss;
            ss << "One of the cameras was not initiated. Waiting flags are: ";
            for (int i = 0; i < CaptureThread::CaptureScheduled.size(); ++i)
                ss << CaptureThread::CaptureScheduled[i];
            throw (std::logic_error(ss.str()));
        }
    }

    for(int j = 0; j< mCameras.size(); ++j)
    {
        {
            std::unique_lock<std::mutex> scheduleLock(CaptureThread::ScheduleGuard);
            CaptureThread::CaptureScheduled = mFalses;
        }
        std::cout << "Starting camera " << mCameras[j].serial << std::endl;
        for(int i = 0; i < 3; ++i) {
            std ::cout << mCameras[j].serial << " Beat " << j << std::endl;
            {
                std::unique_lock<std::mutex> scheduleLock(CaptureThread::ScheduleGuard);
                CaptureThread::CaptureScheduled[j] = true;
            }

            mThreads[j]->wakeUp();

            std::unique_lock<std::mutex> lk(mBeatGuard);
            CaptureThread::MasterBeatWait.wait(lk, [this](){
                {
                    std::unique_lock<std::mutex> scheduleLock(CaptureThread::ScheduleGuard);
                    mPredVal = CaptureThread::CaptureScheduled == mFalses;
                    //std::cout << "Master pred! ";
                    //for (int i = 0; i < CaptureThread::CaptureScheduled.size(); ++i)
                    //    std::cout << CaptureThread::CaptureScheduled[i];
                    //std::cout << std::endl;
                }
                return mPredVal;
            });
        }
    }
    {
        std::unique_lock<std::mutex> scheduleLock(CaptureThread::ScheduleGuard);
        CaptureThread::CaptureScheduled = mFalses;
    }
    for(int i = 0; i < 3; ++i) {
        {
            std::unique_lock<std::mutex> scheduleLock(CaptureThread::ScheduleGuard);
            CaptureThread::CaptureScheduled[0] = true;
        }

        mThreads[0]->wakeUp();

        std::unique_lock<std::mutex> lk(mBeatGuard);
        CaptureThread::MasterBeatWait.wait(lk, [this](){
            {
                std::unique_lock<std::mutex> scheduleLock(CaptureThread::ScheduleGuard);
                mPredVal = CaptureThread::CaptureScheduled == mFalses;
                //std::cout << "Master pred! ";
                //for (int i = 0; i < CaptureThread::CaptureScheduled.size(); ++i)
                //    std::cout << CaptureThread::CaptureScheduled[i];
                //std::cout << std::endl;
            }
            return mPredVal;
        });
    }
    CaptureThread::StoreFrames.store(true);
    //CaptureThread::CaptureAll.store(true);
    //CaptureThread::CapturedThisTime.store(0);
    while(CaptureThread::DoCapture.load()) {
        {
            std::unique_lock<std::mutex> scheduleLock(CaptureThread::ScheduleGuard);
            CaptureThread::CaptureScheduled = mTrues;
        }
        //CaptureThread::mBeatWait.notify_all();
        for(auto t: mThreads)
            t->wakeUp();
        std::unique_lock<std::mutex> lk(mBeatGuard);
        CaptureThread::MasterBeatWait.wait(lk, [this](){
            {
                std::unique_lock<std::mutex> scheduleLock(CaptureThread::ScheduleGuard);
                mPredVal = CaptureThread::CaptureScheduled == mFalses;
                //std::cout << "Master pred! ";
                //for (int i = 0; i < CaptureThread::CaptureScheduled.size(); ++i)
                //    std::cout << CaptureThread::CaptureScheduled[i];
                //std::cout << std::endl;
            }
            return mPredVal;
        });
        CaptureThread::BeatNo++;
        std::cout << CaptureThread::BeatNo.load() << std::endl;
    }
    for(auto t: mThreads)
        t->wakeUp();

}

void CaptureManager::stop() {
    {
        std::unique_lock<std::mutex> scheduleLock(CaptureThread::ScheduleGuard);
        CaptureThread::CaptureScheduled = mFalses;
    }
    CaptureThread::StoreFrames.store(false);
    //CaptureThread::CaptureAll.store(false);
    CaptureThread::DoCapture.store(false);
    std::cout << "Notify stop!" << std::endl;
    CaptureThread::MasterBeatWait.notify_one();
    for(auto &t: mThreadKeepers) {
        t.join();
    }
    mThreadKeepers.clear();
    mThreads.clear();
    mCapturing = false;
}

void CaptureManager::run() {
    if (!mCameras.size())
        return;
    mCapturing = true;
    mStoreThisBeat.store(mStoreStream);

    //cv::Mat previews(480, 640*mCameras.size(), CV_8UC3, {0, 0, 0});

    auto sec =  std::chrono::milliseconds{mFps > 0 ? int(1000/mFps) : 0};
    auto timePoint = std::chrono::system_clock::now() - sec;
    //std::vector<cv::Mat> previewRois(0);

    //for(int i = 0; i < mCameras.size(); ++i) {
    //    previewRois.push_back(previews(cv::Rect(i*640, 0, 640, 480)));
    //}

    std::vector<Spinnaker::CameraPtr> pCameras;
    for (int i = 0; i < mCameras.size(); ++i) {
        pCameras.push_back(CaptureThread::SpinCamList.GetBySerial(std::to_string(mCameras[i].serial)));
        if (!pCameras[i]->IsInitialized()) {
            pCameras[i]->Init();

            std::cout << mCameras[i].serial << ":" << std::endl;

            for (auto prop: mCameras[i].properties) {

                std::cout << "\t" << prop.name << "<---" << prop.value << std::endl;

                if (prop.type == "enum")
                    setEnumerationProperty(prop.name, prop.value, pCameras[i]);

                if (prop.type == "float")
                    setFloatProperty(prop.name, stof(prop.value), pCameras[i]);

                if (prop.type == "bool")
                    setBoolProperty(prop.name, stob(prop.value), pCameras[i]);

                if (prop.type == "int")
                    Spinnaker::GenICam::setIntProperty(prop.name, std::stoi(prop.value), pCameras[i]);
            }
        }
        if (!pCameras[i]->IsStreaming())
            pCameras[i]->BeginAcquisition();
    }
    std::vector<Spinnaker::ImagePtr> frames;

    for(int i = 0; i < mCameras.size(); ++i) {
        std::cout << "Camera " << i << " format " << pCameras[i]->Height() << "x" << pCameras[i]->Width() << "@" << pCameras[i]->PixelFormat() << std::endl;
    }

    for(int j = 0; j < 3; ++j){
        Spinnaker::ImagePtr pResultImage = pCameras[0]->GetNextImage();
        int incomplete_counter = 0;
        while (pResultImage->IsIncomplete()) {
            incomplete_counter += 1;
            if (incomplete_counter < 2) {
                std::cout << "Image is incomplete";
            }
            pResultImage->Release();
            pResultImage = pCameras[0]->GetNextImage();
        }
        pResultImage->Release();
    }

    for(int i = 0; i < mCameras.size(); ++i) {
        Spinnaker::ImagePtr pResultImage = pCameras[i]->GetNextImage();
        int incomplete_counter = 0;
        while (pResultImage->IsIncomplete()) {
            incomplete_counter += 1;
            if (incomplete_counter < 2) {
                std::cout << "Image is incomplete";
            }
            pResultImage->Release();
            pResultImage = !pCameras[i]->GetNextImage();
        }
        frames.push_back(pResultImage->Convert(pCameras[i]->PixelFormat()));
        pResultImage->Release();
    }
    unsigned long long j = 0;
    while(CaptureThread::DoCapture.load()) {
        for(int i = 0; i < mCameras.size(); ++i)
        {
            Spinnaker::ImagePtr pResultImage = pCameras[i]->GetNextImage();
            int incomplete_counter = 0;
            while (pResultImage->IsIncomplete()) {
                incomplete_counter += 1;
                if (incomplete_counter < 2) {
                    std::cout << "Image is incomplete";
                }
                pResultImage->Release();
                pResultImage = pCameras[i]->GetNextImage();
            }
            pResultImage->Convert(frames[i], pCameras[i]->PixelFormat());
            pResultImage->Release();
        }
        if(mUi && std::chrono::system_clock::now() - timePoint >= sec) {
            timePoint = std::chrono::system_clock::now();
            mUi->dropPreview(false);
            for (int i = 0; i < mCameras.size(); ++i) {
                auto image = frames[i]->Convert(Spinnaker::PixelFormatEnums::PixelFormat_RGB8);
                //QImage part =  ;
                mUi->addFrame(QImage((uchar*)image->GetData(), image->GetWidth(), image->GetHeight()
                                     , image->GetStride(), QImage::Format_RGB888));
                //cv::resize(ConvertToCVmat(image, CV_8UC3), previewRois[i], {640, 480}); ///TODO: here should call to UI
            }
            mUi->addLabel(QString::number(j));
            mUi->showPreview();
            mUi->show();
            //cv::putText(previews, std::to_string(j), {30, 30}, cv::HersheyFonts::FONT_HERSHEY_COMPLEX_SMALL, 0.8, {255, 255, 255}); ///TODO: here should call to UI
            //cv::imshow("Preview", previews); ///TODO: here should call to UI
            //cv::waitKey(20);//std::this_thread::sleep_for(std::chrono::milliseconds{20}); ///TODO: here should be call to UI
        }
        if(mStoreThisBeat) {
            for (int i = 0; i < mCameras.size(); ++i) {
                std::stringstream path;
                path << mStorages[i] << std::filesystem::path::preferred_separator
                     << std::setw(10) << std::setfill('0') << j
                     << ".raw";
                frames[i]->Save(path.str().c_str());
            }
            mStoreThisBeat.store(mStoreStream);
            ++j;
            std::cout << j << std::endl;
        }
        if(!CaptureThread::DoCapture.load())
            break;
    }
    std::cout << "Captured " << j << " frames" << std::endl;
    for (int i = 0; i < mCameras.size(); ++i) {
        std::cout << mCameras[i].serial;
        pCameras[i]->EndAcquisition();
        pCameras[i]->DeInit();
        std::cout << " is no more" << std::endl;
    }
    std::cout << "Bye!" << std::endl;
}

void CaptureManager::storeOnce() {
    mStoreThisBeat.store(true);
}

CaptureManager::~CaptureManager() {
}

void CaptureManager::setGUI(PreviewWidget *ui) {
    mUi = ui;
}

int CaptureManager::getCameraNumber() {
    return  mCameras.size();
}

void CaptureManager::setGUIRefreshFPS(int fps) {
    mFps = fps;
}





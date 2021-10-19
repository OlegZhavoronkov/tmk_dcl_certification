//------------------------------------------------------------------------------
//  Created     : 06.08.21
//  Author      : Victor Kats
//  Description : Import Spinnnaker-based capturer into project (see TMKDCL-380)
//------------------------------------------------------------------------------

//
// Created by victor on 24.01.2020.
//

#include <Spinnaker.h>

#include <iostream>

// This function prints the device information of the camera from the transport
// layer; please see NodeMapInfo example for more in-depth comments on printing
// device information from the nodemap.
int PrintDeviceInfo(Spinnaker::GenApi::INodeMap& nodeMap, unsigned int camNum)
{
    int result = 0;
    std::cout << "Printing device information for camera " << camNum << "..." << std::endl << std::endl;
    Spinnaker::GenApi::FeatureList_t features;
    Spinnaker::GenApi::CCategoryPtr category = nodeMap.GetNode("DeviceInformation");
    if (IsAvailable(category) && IsReadable(category))
    {
        category->GetFeatures(features);
        Spinnaker::GenApi::FeatureList_t::const_iterator it;
        for (it = features.begin(); it != features.end(); ++it)
        {
            Spinnaker::GenApi::CNodePtr pfeatureNode = *it;
            std::cout << pfeatureNode->GetName() << " : ";
            Spinnaker::GenApi::CValuePtr pValue = (Spinnaker::GenApi::CValuePtr)pfeatureNode;
            std::cout << (IsReadable(pValue) ? pValue->ToString() : "Node not readable");
            std::cout << std::endl;
        }
    }
    else
    {
        std::cout << "Device control information not available." << std::endl;
    }
    std::cout << std::endl;
    return result;
}

int main(int argc, char **argv) {
    Spinnaker::SystemPtr system = Spinnaker::System::GetInstance();
    Spinnaker::CameraList camList = system->GetCameras();

    uint n = camList.GetSize();

    std::cout << "Found " << n << " Flir cameras" << std::endl;

    for(uint i = 0; i < n; ++i) {
        Spinnaker::CameraPtr pCam = camList.GetByIndex(i);
        // Retrieve TL device nodemap
        Spinnaker::GenApi::INodeMap& nodeMapTLDevice = pCam->GetTLDeviceNodeMap();
        // Print device information
        PrintDeviceInfo(nodeMapTLDevice, i);;
    }
            return 0;
}

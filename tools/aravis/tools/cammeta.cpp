//------------------------------------------------------------------------------
//  Created     : 04.05.2021
//  Author      : Victor Kats
//  Description : Utility to obtain configuration metadata from camera with aravis lib
//------------------------------------------------------------------------------
#include "CameraMetaData.h"

#include <arv.h>

#include <boost/program_options.hpp>

#include <nlohmann/json.hpp>

#include <cstdlib>
#include <fstream>
#include <iostream>
#include <string>
#include <vector>

int main(int argc, char *argv[]) {
    //int opt;
    std::vector<std::string> features;

    ArvCamera *camera = nullptr;
    GError *arvError = nullptr;

    boost::program_options::options_description desc("Command line options");
    desc.add_options()
            ("help,h", "produce help message.")
            ("features-file,f", boost::program_options::value<std::string>(), "file with list of camera features.")
            ("output-file,o", boost::program_options::value<std::string>(), "output metadata file.")
            ("camera,c", boost::program_options::value<std::string>(),
                    "camera identifier. For GigE Vision camara it may be:\n"
                                                   "\t<vendor>-<model>-<serial>\n"
                                                   "\t<vendor_alias>-<serial>\n"
                                                   "\t<vendor>-<serial>\n"
                                                   "\t<user_id>\n"
                                                   "\t<ip_address>\n"
                                                   "\t<mac_address>\n"
                                                   "\tFirst camara will be used if option skipped");
    boost::program_options::positional_options_description p;
    p.add("camera", 1);
    p.add("features-file", 1);
    p.add("output-file", 1);

    boost::program_options::variables_map vm;
    boost::program_options::store(boost::program_options::command_line_parser(argc, argv).
            options(desc).positional(p).run(), vm);
    boost::program_options::notify(vm);

    if (vm.count("help")) {
        std::cout << desc << std::endl;
        return EXIT_SUCCESS;
    }

    if(vm.count("features-file") != 1){
        std::cerr << "File with features list is mandatory" << std::endl;
        std::cerr << desc << std::endl;
        return EXIT_FAILURE;
    }

    arv_update_device_list();
    arv_update_device_list();
    unsigned int n = arv_get_n_devices();
    if (n == 0) {
        std::cerr << "No GigEVision cameras detected." << std::endl;
        return EXIT_FAILURE;
    }

    std::string cameraId;

    if(vm.count("camera") == 0) {
        std::cout << "No camera specified. Taking first discovered camera " << arv_get_device_id(0)
        << " MAC: " << arv_get_device_physical_id(0) << " IP address: " << arv_get_device_address(0)
        << std::endl;
        cameraId = arv_get_device_physical_id(0);
    }

    camera = arv_camera_new(cameraId.c_str(), &arvError);

    if(!ARV_CAMERA(camera)) {
        std::cerr << "Can not initialise camera object for " << cameraId;
        if(arvError) {
            std::cerr << ": [" << arvError->code << "] " << arvError->message;
        }
        std::cerr << std::endl;
        g_clear_object (&arvError);
        return EXIT_FAILURE;
    }

    std::ifstream featuresFile(vm["features-file"].as<std::string>());

    std::string feature;

    while (std::getline(featuresFile, feature))
    {
        // Line contains string of length > 0 then save it in vector
        if(feature.size() > 0 && feature[0] != '#'
        && arv_camera_is_feature_available(camera, feature.c_str(), nullptr))
            features.push_back(feature);
    }

    if(!features.size()) {
        std::cerr << "Provided list of requested features has no valid entries."<< std::endl;
        g_clear_object (&camera);
        g_clear_object (&arvError);
        return EXIT_FAILURE;
    }

    std::cout << "Looking for " << features.size()  << " feature" << (features.size() == 1 ? "":"s")
    << " from camera " << cameraId << std::endl;


    nlohmann::json meta = serialiseCameraFeatures(camera, features);
    meta["identity"] = cameraId;

    if (vm.count("output-file") == 1) {
        std::ofstream res(vm["output-file"].as<std::string>());
        if(res.is_open()) {
            res << meta.dump(4) << std::endl;
            res.flush();
            res.close();
        }
        else {
            std::cout << meta.dump(4) << std::endl;
        }
    }
    else {
        std::cout << meta.dump(4) << std::endl;
    }

    g_clear_object (&camera);
    g_clear_object (&arvError);

    std::cout << "\nLine for arv_device_set_features_from_string () (C++ function):\n" << std::endl;

    std::cout << cameraFeaturesToCommandString(meta) << std::endl << std::endl;

    std::cout << "\nLine for arv_device_set_features_from_string () (C wrapper, from c string, non-indented JSON):\n" << std::endl;

    std::cout << getCameraCommandString(meta.dump(-1).c_str()) << std::endl << std::endl;

    std::cout << "\nLine for arv_device_set_features_from_string () (C wrapper, from c string, nice-indented JSON):\n" << std::endl;

    std::cout << getCameraCommandString(meta.dump(1).c_str()) << std::endl << std::endl;

    return EXIT_SUCCESS;
}
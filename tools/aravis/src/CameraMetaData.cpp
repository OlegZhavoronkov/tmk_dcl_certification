//------------------------------------------------------------------------------
//  Created     : 04.05.2021
//  Author      : Victor Kats
//  Description : Functions to manipulate with camera metadata
//------------------------------------------------------------------------------

#include "CameraMetaData.h"

#include <boost/algorithm/string/join.hpp>
#include <boost/algorithm/string/classification.hpp>
#include <boost/algorithm/string/replace.hpp>
#include <boost/algorithm/string/split.hpp>
#include <boost/algorithm/string/trim.hpp>
#include <boost/filesystem.hpp>
#include <boost/program_options.hpp>

#include <iostream>

void getEnumEntriesListToJson(ArvGcNode *node, nlohmann::json &nodeData, GError **arvError) {
    std::stringstream values;
    GSList *iter;

    for (iter = const_cast<GSList *>(arv_gc_enumeration_get_entries(ARV_GC_ENUMERATION(node)));
         iter != nullptr;
         iter = iter->next) {
        if(arv_gc_feature_node_is_implemented (static_cast<ArvGcFeatureNode *>(iter->data), arvError)
           && arv_gc_feature_node_is_available (static_cast<ArvGcFeatureNode *>(iter->data), arvError)) {
            values << arv_gc_feature_node_get_name(static_cast<ArvGcFeatureNode *>(iter->data))
                   << (iter->next != nullptr ? "; " : "");
        }
    }
    nodeData["PossibleValues"] = values.str();
}

nlohmann::json serialiseCameraFeatures(ArvCamera *camera, std::vector<std::string> features) {
    ArvDevice *device = arv_camera_get_device(camera);
    GError *arvError = nullptr;

    nlohmann::json meta;
    nlohmann::json featuresList;
    std::vector<std::string> readable = {};
    for(std::string &feature:features) {
        ArvGcNode *node = arv_device_get_feature(device, feature.c_str());

        if (ARV_IS_GC_FEATURE_NODE (node)
            && arv_gc_feature_node_is_implemented (ARV_GC_FEATURE_NODE (node), nullptr)
            && !ARV_IS_GC_CATEGORY (node)
            && arv_gc_feature_node_is_available (ARV_GC_FEATURE_NODE (node), nullptr)
                ) {
            nlohmann::json nodeData;
            //GError *error = nullptr;
            bool hasValue = false;

            if (ARV_IS_GC_STRING(node) ||
                ARV_IS_GC_ENUMERATION(node)) {
                nodeData["Value"] = arv_gc_string_get_value(ARV_GC_STRING(node), &arvError);
                if(ARV_IS_GC_ENUMERATION(node)) {
                    getEnumEntriesListToJson(node, nodeData, &arvError);
                }
                else {
                    nodeData["MaxLength"] = arv_gc_string_get_max_length(ARV_GC_STRING(node), &arvError);
                }
                hasValue = true;
            }
            else if (ARV_IS_GC_INTEGER(node)) {
                if (ARV_IS_GC_ENUMERATION(node)) {
                    nodeData["Value"] = arv_gc_string_get_value(ARV_GC_STRING(node), &arvError);
                    getEnumEntriesListToJson(node, nodeData, &arvError);
                }
                else {
                    nodeData["Value"] = arv_gc_integer_get_value(ARV_GC_INTEGER(node), &arvError);
                    std::stringstream bounds;
                    bounds << arv_gc_integer_get_min(ARV_GC_INTEGER(node), &arvError)
                           << ".." << arv_gc_integer_get_max(ARV_GC_INTEGER(node), &arvError);
                    nodeData["Bounds"] = bounds.str();
                }
                hasValue = true;
            }
            else if (ARV_IS_GC_FLOAT(node)) {
                nodeData["Value"] = arv_gc_float_get_value(ARV_GC_FLOAT(node), &arvError);
                std::stringstream bounds;
                bounds << arv_gc_float_get_min(ARV_GC_FLOAT(node), &arvError)
                       << ".." << arv_gc_float_get_max(ARV_GC_FLOAT(node), &arvError);
                nodeData["Bounds"] = bounds.str();
                hasValue = true;
            }
            else if (ARV_IS_GC_BOOLEAN(node)) {
                nodeData["Value"] = static_cast<bool>(arv_gc_boolean_get_value(ARV_GC_BOOLEAN(node), &arvError));
                hasValue = true;
            }

            if (hasValue && ARV_IS_GC_SELECTOR(node) && arv_gc_selector_is_selector(ARV_GC_SELECTOR(node))) {
                GSList *iter;
                std::stringstream values;

                for (iter = const_cast<GSList *>(arv_gc_selector_get_selected_features(ARV_GC_SELECTOR(node)));
                     iter != nullptr;
                     iter = iter->next) {
                    values << arv_gc_feature_node_get_name((ArvGcFeatureNode *)iter->data)
                    << (iter->next != nullptr ? "; " : "");
                }
                nodeData["PossibleValues"] = values.str();
            }
            if (arvError) {
                std::cerr << arvError->message << std::endl;
                g_clear_error(&arvError);
            }
            else if(hasValue) {
                nodeData["Type"] = arv_dom_node_get_node_name(ARV_DOM_NODE(node));
                featuresList[feature] = nodeData;
                readable.push_back(feature);
            }
            else {
                std::cerr << "Node " << feature << " has no readable value" << std::endl;
            }
        }
    }
    meta["features"] = featuresList;
    meta["access_sequence"] = boost::algorithm::join(readable, ";");
    g_clear_object (&arvError);
    return meta;
}

std::string cameraFeaturesToCommandString(nlohmann::json &meta) {
    if(meta.find("access_sequence") == meta.end() || meta.find("features") == meta.end()) {
        std::cerr << "No acceptable metadata for command string generation provided: incorrect JSON" << std::endl;
        return std::string();
    }
    std::string access = meta["access_sequence"];
    if (!access.length()) {
        std::cerr << "No acceptable metadata for command string generation provided: empty 'access_sequence'"
        << std::endl;
        return std::string();
    }

    std::vector<std::string> features;
    std::stringstream commands;
    boost::split(features, access, boost::is_any_of(",;"), boost::token_compress_on);
    nlohmann::json featuresList = meta["features"];
    for(std::string &featureName:features) {
        if(featuresList.find(featureName) == featuresList.end()) {
            std::cerr << "Incorrect feature name '" << featureName << "' in access_sequence'" << std::endl;
            continue;
        }


        nlohmann::json feature = featuresList[featureName];
        if((feature.find("ReadOnly") == feature.end() || feature["ReadOnly"] == false)) {
            commands << featureName;
            if (feature.find("Value") != feature.end()) {
                commands << "=" << feature["Value"] << " ";
            }
        }
    }

    std::string result = commands.str();

    boost::replace_all(result, "\"", "'");

    boost::trim(result);

    return result;
}

char *getCameraCommandString(const char *data) {
    std::string serialisedData(data);
    try {
        if (boost::filesystem::exists(serialisedData)) {
            std::ifstream file(serialisedData);
            serialisedData = std::string((std::istreambuf_iterator<char>(file)),
                                         std::istreambuf_iterator<char>());
        }
        else {
            std::cout << "getCameraCommandString: input is not a file name. Try to parse it as JSON "<< __FILE__ << ": " << __LINE__ << std::endl;
        }
    }
    catch(...) {
        std::cout << "getCameraCommandString: input is not a file name. Try to parse it as JSON "<< __FILE__ << ": " << __LINE__ << std::endl;
    }
    std::stringstream dataStream;
    dataStream << serialisedData;
    std::string command;
    try {
        nlohmann::json meta = nlohmann::json::parse(serialisedData);;
        command = cameraFeaturesToCommandString(meta);
    }
    catch(...) {
        std::cout << "getCameraCommandString: no JSON detected. "<< __FILE__ << ": " << __LINE__ << std::endl;
    }
    if(command.size()) {
        char *result = (char *) malloc(command.size() + 1);
        command.copy(result, command.size() + 1);
        result[command.size()] = '\0';
        return result;
    }
    else {
        std::cout << "getCameraCommandString: no camera metadata JSON object found. "<< __FILE__ << ": " << __LINE__ << std::endl;
        return NULL;
    }
}

#include "processor/dclprocessor_old.h"
#include <storage/storage_interface.h>
#include <storage/ObjectDescriptor.h>

#include <glog/logging.h>

#include <opencv2/core.hpp>
#include <opencv2/highgui.hpp>

using namespace tmk;
using namespace tmk::storage;

void WorkWithObject(ObjectDescriptor &obj, char *boost_path, char *obj_name) {
    LOG(INFO) << "[middle]: Hello, object " << obj_name << " from " << boost_path << std::endl;
    fflush(stdout);
    //this is a place for maniplation with the object
    storage_interface_analyze_object_with_framesets(boost_path, obj_name);
}

int main(int argc, char *argv[]){
    connect_to_pipeline(PC_MIDDLE, NULL, WorkWithObject);
    return 0;
}


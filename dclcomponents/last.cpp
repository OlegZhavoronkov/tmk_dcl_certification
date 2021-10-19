#include "processor/dclprocessor_old.h"
#include <storage/ObjectDescriptor.h>

#include <glog/logging.h>

using namespace tmk;
using namespace tmk::storage;

void WorkWithObject(ObjectDescriptor &obj, char *boost_path, char *obj_name) {
    LOG(INFO) << "[last]: Hello, object " << obj_name << " from " << boost_path << std::endl;
    fflush(stdout);
    //this is a place for maniplation with the object
}

int main(int argc, char *argv[]){
    connect_to_pipeline(PC_LAST, NULL, WorkWithObject);
    return 0;
}


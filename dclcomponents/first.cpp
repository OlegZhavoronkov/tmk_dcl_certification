#include "processor/dclprocessor_old.h"

#include <storage/ObjectDescriptor.h>
#include <glog/logging.h>

#include <misc/log.h>
#include <collector/collector.h>
#include <collector/collector_description.h>

using namespace tmk;
using namespace tmk::storage;

void WorkWithObject(ObjectDescriptor &obj, char *boost_path, char *obj_name) {
    (void)obj;
    LOG(INFO) << "[first]: Hello, I'm object " << obj_name << " from " << boost_path << std::endl;
    fflush(stdout);
    //this is a place for maniplation with the object
}

int main(int argc, char *argv[]){
    LOG_FD_WHERE("");
    struct collector_description *desc = NULL;
#if 1 //with n tcp-fake cameras
    create_test_collector_description(1, &desc);
#else //with to sources at tmk-serv
    create_demo_collector_description(&desc); //-- 2 cameras, problems after 10-13 objects full-size
    //create_default_collector_description(&desc); //-- empty collector, not work in this version
#endif

    struct data_collector *collector = NULL;
    collector = collector_init(desc);
    if (!collector) {
        fprintf(stdout, "[%s]: collector init return NULL, may be camera isn't ready. Please, try again or work without stream.\n", argv[0]);
        fflush(stdout);
    }

    //gdb -p `ps aux | grep tmk-first | grep -v grep | awk '{ print $2 }'`
    //sleep(30);  //-- use it for attach to first process before it will be defunct: gdb -p <pid>

#if 1
    fprintf(stdout, "[%s]: connect to pipeline with collector\n", argv[0]);
    fflush(stdout);
    connect_to_pipeline(PC_FIRST, (void*)collector, WorkWithObject);
#else
    fprintf(stdout, "[%s]: connect to pipeline with null-collector\n", argv[0]);
    fflush(stdout);
    connect_to_pipeline(PC_FIRST, NULL, WorkWithObject);
#endif

    if (collector) collector_destroy(collector);
    return 0;
}


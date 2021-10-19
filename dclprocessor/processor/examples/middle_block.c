#include <stdlib.h>
#include <string.h>

#include <misc/log.h>
#include <processor/dclprocessor.h>
//#include <processor/dclprocessor_old.h>

void work_with_object(void *user_data_ptr, char *boost_path, char *obj_name) {
    if (!user_data_ptr) {
        LOG_FD_INFO("[middle]: Hello, object '%s' from '%s' and no user data", obj_name,  boost_path);
    }
    else {
        LOG_FD_INFO("[middle]: Hello, object '%s' from '%s' and user_data '%s'", obj_name,  boost_path, (char *)user_data_ptr);
    }
    //this is a place for maniplation with the object
}

int main(){
    char *user_data_ptr = strdup("user data");
    struct dclprocessor *proc = get_dclprocessor();
    attach_to_pipeline(proc, NULL, work_with_object);
    //connect_to_pipeline(PC_MIDDLE, (void*)user_data_ptr, work_with_object);
    free(user_data_ptr);
    return 0;
}


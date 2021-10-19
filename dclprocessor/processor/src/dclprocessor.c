#include <stdlib.h>
#include <string.h>

#include <misc/log.h>
#include <pipeline/pipeline_client.h>
#include <pipeline/pipeline_server.h>
#include <processor/dclprocessor.h>
#include <processor/dclprocessor_description.h>

void dclprocessor_init(struct dclprocessor **p_dclproc, struct dclprocessor_description *desc) {
    int rc;

    struct dclprocessor *dclproc = (struct dclprocessor *)malloc(sizeof(struct dclprocessor));
    if (!dclproc) {
        LOG_FD_ERROR("can't allocte dclprocessor\n");
        return;
    }

    memset(dclproc, 0, sizeof(struct dclprocessor));

    if (!desc) {
        LOG_FD_INFO("no dclprocessor description, create default\n");
        struct dclprocessor_description *desc = NULL;
        create_default_dclprocessor_description(&desc);
    }

    LOG_FD_INFO("Check dclprocessor configuration...\n");
    out_dclprocessor_description(desc);

    rc = pipeline_init(&dclproc->pipeline, desc->pipeline_desc);
    if (rc != 0) {
        LOG_FD_ERROR("couldn't init pipeline\n");
    }

    *p_dclproc = dclproc;
}

void dclprocessor_destroy(struct dclprocessor **p_dclproc) {
    if (p_dclproc == NULL || *p_dclproc == NULL )  return;
    struct dclprocessor *dclproc = *p_dclproc;
    if (dclproc->pipeline) pipeline_destroy(dclproc->pipeline);
    if (dclproc->collector) collector_destroy(dclproc->collector);
    //TODO: process subsystem
    free(dclproc);
    *p_dclproc = NULL;
}

//up full system structure from shared memory or default
struct dclprocessor *get_dclprocessor() {
    struct dclprocessor *dclproc = NULL; //take from shmem
    //if no in the shmem => server and need to create
    return dclproc;
}

void out_dclprocessor_status(struct dclprocessor *dclproc){
    (void)dclproc;
    LOG_FD_INFO("It will be system status. Not now.\n");
}

void attach_to_pipeline (
         struct dclprocessor *dclproc, void* user_data_ptr,
         void (*work_with_object)(void * user_data_ptr, char *boost_path, char *obj_name)
){
    dclproc->user_data_ptr = user_data_ptr;
    dclproc->client_pipeline_action = work_with_object;
    //ask from env and process_info
    //enum PC_TYPE type = get_module_type();
    //pipeline_process((void *)(&dclproc), dclproc->ops->first_action);
}



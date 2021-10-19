#ifndef _DCLPROCESSOR_H_
#define _DCLPROCESSOR_H_

#ifdef __cplusplus
extern "C" {
#endif

#include <processor/dclprocessor_description.h>
#include <pipeline/pipeline_client.h>
#include <pipeline/pipeline.h>
#include <collector/collector.h>
#include <tcp/server.h>
#include <tcp/client.h>
#include <tcp/proxy.h>

//ATTENTION: this parameters are NOT for configuration -- number of bit in system description (see status and message api)
#define DCL_MAX_MODULES_NUMBER 16 //max possible modules in pipeline = number of children processes in pipepeline
#define DCL_MAX_SOURCES_NUMBER 8  //max possible sources in collector = number of capturing threads in first module

struct dclprocessor {
    struct dclprocessor_description *pdesc;

    uint64_t status[4]; //4 * sizeof(uint64_t) * 8 status bits for describe all DCL-system

    struct dclprocessor_ops *ops;

    struct pipeline *pipeline;
    void *user_data_ptr; //pointer to private pipeline module data
#if 0
    struct dclprocessor_ops *ops; //TODO:
#else
    //TODO: this is one which set as callback into one of 3 actions: first_action, middle_action and last_action
    void (*client_pipeline_action)(void *user_data_ptr, char *boost_path, char *obj_name);
#endif
    struct data_collector *collector;
#if 0
    struct proxy *proxy;
#else
    struct server *cmd_server; //main command server = main process of tmk-server
    struct client *cmd_client; //client for command server = main process of tmk-manager
    struct server *module_servers[DCL_MAX_MODULES_NUMBER]; //tcp servers of all modules in pipeline
    struct server *source_servers[DCL_MAX_SOURCES_NUMBER]; //tcp servers of all sources in collector
#endif
    //signal_subsystem
};

void dclprocessor_init(struct dclprocessor **p_dclproc, struct dclprocessor_description *desc);
void dclprocessor_destroy(struct dclprocessor **p_dclproc);

//create dclprocessor = create all subsystems
struct dclprocessor *create_dclprocessor(char *init_string);
//get dclprocessor structure information from shared memory
struct dclprocessor *get_dclprocessor();
void out_dclprocessor_status(struct dclprocessor *dclproc);

void attach_to_pipeline(
         struct dclprocessor *dclproc, void* user_data_ptr,
         void (*work_with_object)(void * user_data_ptr, char *boost_path, char *obj_name));

#ifdef __cplusplus
}
#endif

#endif //_DCLPROCESSOR_H_


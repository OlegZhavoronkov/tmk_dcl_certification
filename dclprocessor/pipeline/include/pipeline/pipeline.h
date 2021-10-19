#ifndef _PIPELINE_H
#define _PIPELINE_H

#ifdef __cplusplus
extern "C" {
#endif

#include <pthread.h>
#include <misc/list.h>

#include <pipeline/pipeline_module.h>

struct pipeline_description;

//pipeline modules methods
void *initial_thread(void *arg); //for work "in fly"
void *source_thread(void *arg);  //source thread should be first and operation most long by time of all modules
void *process_thread(void *arg);
void *finalization_thread(void *arg);

struct pipeline {
    struct pipeline_description *pdesc;

    //main pipeline thread
    pthread_t thread_id;
    int exit_flag;

    //for process_info description
    int proc_cnt; //at least 1 (real: modules_cnt + 1)
    char *shmpath; // should start with '/'
    void *shmem;

    //ring buffer parameters for store active data between processes
    int data_cnt; //number of data chunks
    int data_size; //size of each data chunk

    int modules_cnt;
    list_t modules_list; //list of pipeline_module structures

    enum MD_STATE state; //using MD_STATE, think about PL_STATE, but it will implement not now

    int need_to_free;
};

#ifdef __cplusplus
}
#endif

#endif //_PIPELINE_H

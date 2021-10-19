#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>

#include <misc/ring_buffer.h>

#include <dclprocessor/process_mngr.h>
#include <dclprocessor/shared_memory.h>
#include "dclprocessor/shared_memory_map.h"

#include <pipeline/pipeline.h>

void *finalization_thread(void *arg){
    struct pipeline_module *module = arg;
    size_t dataoffset;
    char *data;
    int cmd;

    module->idx = -1;
    while(1){ //not by exit flag because finalization should be always
        sem_wait(&module->new_cmd);
        cmd = module->cmd; //TODO: queue
        dataoffset = module->dataoffset;
        sem_post(&module->result);
        if (cmd == PROCESS_CMD_STOP){
            //previous process stop their job, finish pipeline
            break;
        }
        data = (char*)module->shmem + dataoffset;
        data[0] = '\0';
        fprintf(stdout, "[finalization_thread, %lx]: get data %p, data: %s\n", module->thread_id, data, data);
        fflush(stdout);
        //no child process, only thread
        ring_buffer_push(module->data_ring, dataoffset);
    }
    fprintf(stdout, "[finalization_thread, %lx]: stop\n", module->thread_id);
    fflush(stdout);
    return NULL;
}

void *source_thread(void *arg){
    struct pipeline_module *module = arg;
    size_t dataoffset;
    char *data;
    int cmd;

    module->idx = process_start(module->argv);
    module->proc_info = (struct process_info *)((char*)module->shmem + memory_get_processoffset(module->shmem)) + module->idx;

    fprintf(stdout, "[source_thread, %lx]: when %s is started\n", module->thread_id, module->proc_info->process_name);
    fflush(stdout);

    while(!(*(module->exit_flag))){
        cmd = module->cmd; //TODO: queue
        dataoffset = ring_buffer_pop(module->data_ring);
        data = (char*)module->shmem + dataoffset;
        fprintf(stdout, "[source_thread, %lx]: capture data %p, data %s\n", module->thread_id, data, data);
        module->proc_info->cmd = cmd;
        module->proc_info->cmd_offs = dataoffset;
        sem_post(&module->proc_info->sem_job);
        sem_wait(&module->proc_info->sem_result);
        fprintf(stdout, "[source_thread, %lx]: captured data %p, data: %s\n", module->thread_id, data, data);
        sem_wait(&module->next->result);
        module->next->cmd = PROCESS_CMD_JOB;
        module->next->dataoffset = dataoffset;
        sem_post(&module->next->new_cmd);
    }

    fprintf(stdout, "[source_thread, %lx]: stop\n", module->thread_id);
    sem_wait(&module->next->result);
    module->next->cmd = PROCESS_CMD_STOP;
    sem_post(&module->next->new_cmd);

    module->proc_info->cmd = PROCESS_CMD_STOP;
    sem_post(&module->proc_info->sem_job);
    sem_wait(&module->proc_info->sem_result);

    return NULL;
}

void *process_thread(void *arg){
    struct pipeline_module *module = arg;
    size_t dataoffset;
    char *data;
    int cmd;

    module->idx = process_start(module->argv);
    module->proc_info = (struct process_info *)((char*)module->shmem + memory_get_processoffset(module->shmem)) + module->idx;   

    fprintf(stdout, "[process_thread, %lx]: when %s is started\n", module->thread_id, module->proc_info->process_name);
    fflush(stdout);

    while(1){
        sem_wait(&module->new_cmd);
        cmd = module->cmd; //TODO: queue???
        dataoffset = module->dataoffset;
        sem_post(&module->result);
        if (cmd == PROCESS_CMD_STOP){
            // previous process stop their job, finish pipeline
            break;
        }
        data = (char*)module->shmem + dataoffset;
        fprintf(stdout, "[process_thread, %lx]: get data %p, data: %s\n", module->thread_id, data, data);
        fflush(stdout);
        module->proc_info->cmd = cmd; 
        module->proc_info->cmd_offs = dataoffset;
        sem_post(&module->proc_info->sem_job);
        sem_wait(&module->proc_info->sem_result);

        if (module->proc_info->cmd_result != 0){
            // previous process report an error, drop frame and continue
            fprintf(stdout, "[process_thread, %lx]: processor status %d, drop data %p\n", module->thread_id, module->proc_info->cmd_result, data);
            fflush(stdout);
            //pushfree buffer
            ring_buffer_push(module->data_ring, dataoffset); 
            continue;
        }
        fprintf(stdout, "[process_thread, %lx]: processed data %p, data: %s\n", module->thread_id, data, data);
        fflush(stdout);
        sem_wait(&module->next->result);
        module->next->cmd = PROCESS_CMD_JOB;
        module->next->dataoffset = dataoffset;
        sem_post(&module->next->new_cmd);
    }
    fprintf(stdout, "[process_thread, %lx]: stop\n", module->thread_id);
    fflush(stdout);
    sem_wait(&module->next->result);
    module->next->cmd = PROCESS_CMD_STOP;
    sem_post(&module->next->new_cmd);

    module->proc_info->cmd = PROCESS_CMD_STOP;
    sem_post(&module->proc_info->sem_job);
    sem_wait(&module->proc_info->sem_result);

    return NULL;
}



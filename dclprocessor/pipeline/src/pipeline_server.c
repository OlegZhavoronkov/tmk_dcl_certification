#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>

#include "pipeline/shared_memory.h"
#include "pipeline/shared_memory_map.h"
#include "pipeline/process_mngr.h"

#include <misc/ring_buffer.h>

#include <pipeline/pipeline.h>
#include <pipeline/pipeline_description.h>
#include <pipeline/pipeline_module.h>
#include <pipeline/pipeline_server.h>

void* pipeline_thread(void *arg) {
    struct pipeline *pl = (struct pipeline*)(arg);
    void *shmem = pl->shmem;
    int i;

    struct memory_map *map = (struct memory_map *)(shmem);

    struct process_info *proc_info = (struct process_info *)(shmem + memory_get_processoffset(shmem));
    process_subsystem_init(proc_info, map->process_cnt);

    struct ring_buffer *data_ring = ring_buffer_create(map->data_cnt, 0);
    for (i = 0; i < map->data_cnt; i++)
        data_ring->buffer[i] = memory_get_dataoffset(shmem) + i * map->data_size;

    //Kat's paranoya: check that pipeline is correct 
    if (pl->modules_cnt < 2) {
        fprintf(stderr, "[pipeline_thread]: very strange -- initial and finalization thread should be always\n");
        fflush(stderr);
    }

    list_t *elem = NULL;
    sem_t start_control;
    sem_init(&start_control, 0, 0);

    pl->state = MD_STATE_UNKNOWN;

    //initialize modules (pipeline part)
    int real_started = 0;
    elem = list_first_elem(&pl->modules_list);
    while (list_is_valid_elem(&pl->modules_list, elem)) {
        struct pipeline_module *module = list_entry(elem, struct pipeline_module, entry);
        fprintf(stdout, "[%s]: module->exit_flag = %p, exit_flag = %d[%p]\n", __FUNCTION__, module->exit_flag, pl->exit_flag, &pl->exit_flag);
        fflush(stdout);
        pipeline_module_init(module, shmem, data_ring, &pl->exit_flag, &start_control);
        pthread_create(&(module->thread_id), NULL, module->thread_method, module);
        fprintf(stdout, "[%s]: module %lx on method %p waitting start\n", __FUNCTION__, module->thread_id, module->thread_method);
        fflush(stdout);
        sem_wait(module->start_control);
        fprintf(stdout, "[%s]: module with id = %d, idx = %d started\n", __FUNCTION__, module->id, module->idx);
        fflush(stdout);
        if (module->idx == -1) {
            //TODO: check module state
            pthread_join(module->thread_id, NULL);
            pipeline_module_set_state(MD_STATE_ERROR, module);
            pl->state = MD_STATE_ERROR;
            break;
        }
        pipeline_module_set_state(MD_STATE_INIT, module);
        real_started++;
        elem = elem->next;
    }
    sem_destroy(&start_control);
    if (real_started >= 2) {
        pl->state = MD_STATE_INIT;
    }
    else {
        pl->state = MD_STATE_ERROR;
    }

    pipeline_modules_list_debug_out(&pl->modules_list);

    //move it to pipeline_start
    int wait_on_start = pl->pdesc->wait_on_start;
    if (wait_on_start > 0) {
        fprintf(stdout, "[%s]: please, wait %ds before pipeline started 2\n", __FUNCTION__, wait_on_start);
        fflush(stdout);
        //sleep(wait_on_start);
    }

    if (pl->state == MD_STATE_INIT) {
        pipeline_modules_set_state(MD_STATE_LIVE, &pl->modules_list);
        pl->state = MD_STATE_LIVE;
    }

    //when pipeline stopped we should free memory
    elem = list_first_elem(&pl->modules_list);
    while (list_is_valid_elem(&pl->modules_list, elem)) {
        struct pipeline_module *module = list_entry(elem, struct pipeline_module, entry);
        if (module->idx != -1) pthread_join(module->thread_id, NULL); //else thread already joined 
        pipeline_module_destroy(module);
        elem = elem->next;
    }

    ring_buffer_destroy(data_ring);
    process_subsystem_destroy();
    return NULL;
}

int pipeline_init(struct pipeline **p_pl, struct pipeline_description *pdesc) {
    int rc = 0; //returned value = ok

    if (!pdesc) { 
        // TODO: create empty pipeline as default for add new modules "on fly"
        // not implemented now: init/start/stop/destroy in this case
        fprintf(stderr, "[%s]: can't init pipeline without any description (\"on fly\" not supported yet)\n", __FUNCTION__);
        fflush(stderr);
        return -1;
    }

    struct pipeline *pl = *p_pl;
    if (!pl) {
        pl = (struct pipeline *)malloc(sizeof(struct pipeline));
        memset(pl, 0, sizeof(struct pipeline));
        pl->need_to_free = 1;
    }
    if (!pl) {
        fprintf(stderr, "[%s]: can't allocate memory for pipeline\n", __FUNCTION__);
        fflush(stderr);
        return -1;
    }
    fprintf(stdout, "[%s]: Check pipeline configuration:\n", __FUNCTION__);
    fflush(stdout);
    out_pipeline_description(pdesc);
    //only link to external pdesc
    pl->pdesc = pdesc;

    pl->state = MD_STATE_UNKNOWN;
    pl->exit_flag = 0;
    pl->proc_cnt = pdesc->proc_cnt;
    pl->data_cnt = pdesc->data_cnt;
    pl->data_size = pdesc->data_size;
    pl->shmpath = strdup(pdesc->shmpath);

    void *map = shared_memory_map_create(pl->proc_cnt, PROCESS_INFO_SIZE, pl->data_cnt, pl->data_size);
    shared_memory_unlink(pl->shmpath);
    void *shmem = shared_memory_server_init(pl->shmpath, map);
    free(map);
    if (!shmem) {
        fprintf(stderr, "[%s]: can't init shared memory\n", __FUNCTION__);
        fflush(stderr);
        *p_pl = NULL;
        return -1;
    }
    shared_memory_setenv(pl->shmpath, shmem);
    pl->shmem = shmem;

    list_init(&pl->modules_list);
    for (int i = 0; i < pdesc->modules_cnt; i++) {
        pipeline_module_add(&pl->modules_list, &pl->modules_cnt, pdesc->mdescs[i]);
    }
    rc = pipeline_modules_update(&pl->modules_list);
    if (rc != 0) {
        fprintf(stderr, "[%s]: problems with modules configuration\n", __FUNCTION__);
        fflush(stderr);
        pipeline_destroy(pl);
        *p_pl = NULL;
        return -1;
    }

    fprintf(stdout, "[%s]: added %d modules to pipeline\n", __FUNCTION__, pl->modules_cnt);
    fflush(stdout);

    rc = pthread_create(&pl->thread_id, NULL, pipeline_thread, pl);
    if (rc != 0) {
        fprintf(stdout, "[%s]: pthread create error\n", __FUNCTION__);
        fflush(stdout);
    }

    *p_pl = pl;
    return 0;
}

int pipeline_start(struct pipeline *pl) {
    (void)pl;
    fprintf(stdout, "[%s]: unsupported, always start when init\n", __FUNCTION__);
    fflush(stdout);
    return 0;
}

int pipeline_stop(struct pipeline *pl) {
    (void)pl;
    fprintf(stdout, "[%s]: unsupported yet\n", __FUNCTION__);
    fflush(stdout);
    return 0;
}

void pipeline_destroy(struct pipeline *pl) {
    struct pipeline_module *module;

    pthread_join(pl->thread_id, NULL);
    fprintf(stdout, "[%s]: main pipeline thread 0x%lx finished\n", __FUNCTION__, pl->thread_id);
    fflush(stdout);

    pipeline_modules_set_state(MD_STATE_UNKNOWN, &pl->modules_list);
    pl->state = MD_STATE_UNKNOWN;
    pl->exit_flag = 1;

    shared_memory_destroy(pl->shmpath, pl->shmem);
    free(pl->shmpath);

    //pdesc is pointer to external pdesc
    pl->pdesc = NULL;

    //delete modules from list
    list_t *item;
    while(!list_is_empty(&pl->modules_list)) {
        item = list_first_elem(&pl->modules_list);
        module = list_entry(item, struct pipeline_module, entry);
        pipeline_module_delete(&pl->modules_list, &pl->modules_cnt, module);
        pipeline_module_destroy(module);
    }

    if (pl->need_to_free) free(pl);
}


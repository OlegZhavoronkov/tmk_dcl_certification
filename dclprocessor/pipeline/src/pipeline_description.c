#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <pipeline/pipeline_module_description.h>
#include <pipeline/pipeline_description.h>


#if 0
//we want at least 64K for any module as default
#define DEFAULT_MODULE_SIZE 0x10000

static size_t calculate_pipeline_size(int max_modules, size_t max_module_data) {
    if (max_module_data < DEFAULT_MODULE_SIZE) {
        return max_modules * DEFAULT_MODULE_SIZE;
    }
    return  max_module_data * max_modules + sizeof(int) * 2 + sizeof(size_t);
}void *process_thread(void *arg);
#endif

int pipeline_description_init(int wait_on_start, char *shmpath, int data_cnt, int data_size, int modules_cnt, struct pipeline_description **p_pdesc) {
    struct pipeline_description *desc = *p_pdesc;
    if (!desc) desc = (struct pipeline_description *)malloc(sizeof(struct pipeline_description));
    memset(desc, 0, sizeof(struct pipeline_description));

    desc->wait_on_start = wait_on_start;

    desc->shmpath = strdup(shmpath);
    desc->data_cnt = data_cnt;
    desc->data_size = data_size;

    desc->modules_cnt = modules_cnt; //number of modules-processer (initial and finalization thread are included)
    desc->proc_cnt = desc->modules_cnt + 1; //number element in process table: (initial and finalization thread are included) + pipeline_process

    desc->mdescs = (struct pipeline_module_description **)malloc(desc->modules_cnt * sizeof(struct pipeline_module_description *));
    memset(desc->mdescs, 0, desc->modules_cnt * sizeof(struct pipeline_module_description *));

    *p_pdesc = desc;
    return 0;
}

void out_pipeline_description(struct pipeline_description *desc) {
    if (!desc) {
        fprintf(stderr, "[%s]: desription is NULL\n", __FUNCTION__);
        fflush(stderr);
        return;
    }
    fprintf(stdout, "Pipeline description:\n");
    fprintf(stdout, "\tshmem region path = \"%s\"\n", desc->shmpath);
    //TODO: check that path is started with '\'
    fprintf(stdout, "\tshmem region size = %zd\n", desc->shmem_size);
    fprintf(stdout, "\tdata ring buffer count = %d\n", desc->data_cnt);
    //TODO: check that path is started with '\'
    fprintf(stdout, "\tdata size = %zd\n", desc->data_size);
    fprintf(stdout, "\tnumber of modules = %d\n", desc->modules_cnt);
    for (int i = 0; i < desc->modules_cnt; i++) {
        out_pipeline_module_description(desc->mdescs[i]);
    }
    fflush(stdout);
}

void pipeline_description_destroy(struct pipeline_description *pdesc) {
    if (!pdesc) return;
    for (int i = 0; i < pdesc->modules_cnt; i++) {
        struct pipeline_module_description *mdesc = pdesc->mdescs[i];
        clean_pipeline_module_description(mdesc);
        pdesc->mdescs[i] = NULL;
    }
    free(pdesc->mdescs);
    free(pdesc);
}


#ifndef _PIPELINE_DESCRIPTION_H_
#define _PIPELINE_DESCRIPTION_H_

#ifdef __cplusplus
extern "C" {
#endif

#include <misc/list.h>

struct pipeline_module_description;

struct pipeline_description {
    char *shmpath;
    size_t shmem_size;

    int modules_cnt;
    struct pipeline_module_description **mdescs;

    int proc_cnt;
    int data_cnt;
    size_t data_size;

    int wait_on_start; //in seconds, 0 is possible
};

int pipeline_description_init(int wait_on_start, char *shmpath, int data_cnt, int data_size, int modules_cnt, struct pipeline_description **p_pdesc);
void out_pipeline_description(struct pipeline_description *desc);
void pipeline_description_destroy(struct pipeline_description *pdesc);

#ifdef __cplusplus
}
#endif

#endif //_PIPELINE_DESCRIPTION_H_


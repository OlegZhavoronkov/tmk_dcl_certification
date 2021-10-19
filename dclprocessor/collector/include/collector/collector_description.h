#ifndef _COLLECTOR_DESCRIPTION_H_
#define _COLLECTOR_DESCRIPTION_H_

#ifdef __cplusplus
extern "C" {
#endif

#define MAX_SHMEM_NAME_LEN 256

struct collector_description {
    int sources_cnt;
    struct source_description **sdescs;
    int use_boost_shmem;
    char boost_path[MAX_SHMEM_NAME_LEN];
    size_t boost_size; //optional, may be calculated by collector and sources description ???
};

void create_default_collector_description(struct collector_description **pdesc);
void create_test_collector_description(int sources_cnt, struct collector_description **pdesc);
void create_demo_collector_description(struct collector_description **pdesc);

void out_collector_description(struct collector_description *desc);
void collector_description_destroy(struct collector_description **p_desc);

#ifdef __cplusplus
}
#endif

#endif //_COLLECTOR_DESCRIPTION_H_

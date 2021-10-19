#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <collector/collector_description.h>
#include <collector/source_description.h>

static size_t calculate_boostmem_size(int max_sources, size_t max_frame_size/*w*h*bps*/, int max_frames) {
    //we want 1M for any source
    return  0x100000 * max_sources + max_sources * max_frame_size * max_frames;
}

void create_default_collector_description(struct collector_description **pdesc) {
    struct collector_description *desc = *pdesc;
    if (!desc) desc = (struct collector_description *)malloc(sizeof(struct collector_description));
    memset(desc, 0, sizeof(struct collector_description));
    //desc->sources_cnt = 0;
    //desc->use_boost_shmem = 0;
    *pdesc = desc;
};

void create_test_collector_description(int sources_cnt, struct collector_description **pdesc) {
    struct collector_description *desc = *pdesc;
    if (!desc) desc = (struct collector_description *)malloc(sizeof(struct collector_description));
    memset(desc, 0, sizeof(struct collector_description));
    desc->sources_cnt = sources_cnt;
    if (!desc->sdescs) {
        desc->sdescs = (struct source_description **)malloc(desc->sources_cnt * sizeof(struct source_description *));
        memset(desc->sdescs, 0, desc->sources_cnt * sizeof(struct source_description *));
        for (int i = 0; i < desc->sources_cnt; i++) {
            char port[8];
            sprintf(port, "%d", 9000 + i * 10);
            create_tcp_source_description("localhost", port, &(desc->sdescs[i]));
        }
    }
    desc->use_boost_shmem = 1;
    if (desc->use_boost_shmem) {
        strcpy(desc->boost_path, "boostsegment");
        //desc->boost_size = 5000000000; //~5Gb
        desc->boost_size = calculate_boostmem_size(desc->sources_cnt, 2048 * 2448 * 1, 1000);
    }
    *pdesc = desc;
}

void create_demo_collector_description(struct collector_description **pdesc) {
    struct collector_description *desc = *pdesc;
    if (!desc) desc = (struct collector_description *)malloc(sizeof(struct collector_description));
    memset(desc, 0, sizeof(struct collector_description));
    desc->sources_cnt = 1;
    if (!desc->sdescs) {
        desc->sdescs = (struct source_description **)malloc(desc->sources_cnt*sizeof(struct source_description *));
        memset(desc->sdescs, 0, desc->sources_cnt * sizeof(struct source_description *));
        create_gige_source_description("10.0.0.2", &(desc->sdescs[0])); //flir-color2
        //create_gige_source_description("169.254.2.134", &(desc->sdescs[1])); //flir-color
        //create_gige_source_description("169.254.0.1", &(desc->sdescs[1])); //lucid polar
        create_gige_source_description("10.0.0.3", &(desc->sdescs[1])); //lucid polar
    }
    desc->use_boost_shmem = 1;
    if (desc->use_boost_shmem) {
        strcpy(desc->boost_path, "boostsegment");
        desc->boost_size = 30000000000; //~30Gb
        //desc->boost_size = 58048974720; //~60Gb
    }
    *pdesc = desc;
}

void out_collector_description(struct collector_description *desc) {
    fprintf(stdout, "Collector description:\n");
    fprintf(stdout, "\tnumber of sources = %d\n", desc->sources_cnt);
    for (int i = 0; i < desc->sources_cnt; i++) {
        out_source_description(desc->sdescs[i]);
    }
    if (desc->use_boost_shmem) {
        fprintf(stdout, "Use boost shared memory:\tYES\n");
        fprintf(stdout, "\tboost region path = \"%s\"\n", desc->boost_path);
        fprintf(stdout, "\tboost region size = \"%zd\"\n", desc->boost_size);
    }
    else {
        fprintf(stdout, "Use boost shared memory:\tNO\n");
    }
    fflush(stdout);
}

void collector_description_destroy(struct collector_description **p_desc) {
    if (!p_desc || *p_desc == NULL) return;
    struct collector_description *desc = *p_desc;
    free(desc->sdescs);
    free(desc);
    *p_desc = NULL;
}




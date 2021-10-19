#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <pipeline/pipeline_module_description.h>
#include <pipeline/pipeline_description.h>

#include "pipeline_examples.h"

#if 0
#include <pipeline/config/config.h>
#endif

#define DEFAULT_WAIT_ON_START 3
//#define DEFAULT_WAIT_ON_START 0

//Examples of code for construct pipeline

//Empty pipeline should include at least 2 threads
void create_empty_pipeline_description(struct pipeline_description **p_pdesc) {
    struct pipeline_description *desc = *p_pdesc;

    int modules_cnt = 1 + 1;
    pipeline_description_init(DEFAULT_WAIT_ON_START, "/procsegment", 512, 256, modules_cnt, &desc);

    //initial and finalization stream always in pipeline and last
    create_initial_module_description(0, 1, &desc->mdescs[0]);
    create_last_module_description(modules_cnt - 1, -1, &desc->mdescs[modules_cnt-1]);

    *p_pdesc = desc;
}

//Minimal pipeline should include at least 2 modules: pipeline has only capture part
void create_minimal_pipeline_description(struct pipeline_description **p_pdesc) {
    struct pipeline_description *desc = *p_pdesc;

    int modules_cnt = 1 + 2;
    pipeline_description_init(DEFAULT_WAIT_ON_START, "/procsegment", 512, 256, modules_cnt, &desc);
    //place for initial stream
    //finalization stream always in pipeline
    create_last_module_description(modules_cnt - 1, -1, &desc->mdescs[modules_cnt-1]);

    char **argv = NULL;

    argv = (char **)malloc(2 * sizeof(char*));
    argv[0] = strdup("bin/psource");
    argv[1] = NULL;
    create_first_module_description(0, 1, argv, &(desc->mdescs[0]));
    free(argv[0]);
    free(argv);

    argv = (char **)malloc(2 * sizeof(char*));
    argv[0] = strdup("bin/phandler");
    argv[1] = NULL;
    create_middle_module_description(1, 2, argv, &(desc->mdescs[1]));
    free(argv[0]);
    free(argv);

    *p_pdesc = desc;
}

//strange work of this pipeline -- not real input (' ' or '\n'?)
void create_demo_pipeline_description(struct pipeline_description **p_pdesc) {
    struct pipeline_description *desc = *p_pdesc;

    int modules_cnt = 1 + 2;
    pipeline_description_init(DEFAULT_WAIT_ON_START, "/procsegment", 512, 256, modules_cnt, &desc);
    //place for initial stream
    //finalization stream always in pipeline
    create_last_module_description(modules_cnt - 1, -1, &desc->mdescs[modules_cnt-1]);

    char **argv = NULL;

    argv = (char **)malloc(2 * sizeof(char*));
    argv[0] = strdup("bin/tmk-demo-capturer");
    argv[1] = NULL;
    create_first_module_description(0, 1, argv, &(desc->mdescs[0]));
    free(argv[0]);
    free(argv);

    argv = (char **)malloc(2 * sizeof(char*));
    argv[0] = strdup("bin/tmk-demo-filter");
    argv[1] = NULL;
    create_middle_module_description(1, 2, argv, &(desc->mdescs[1]));
    free(argv[0]);
    free(argv);

    *p_pdesc = desc;
}

void create_default_pipeline_description(struct pipeline_description **p_pdesc) {
    struct pipeline_description *desc = *p_pdesc;

    int modules_cnt = 1 + 3;
    pipeline_description_init(DEFAULT_WAIT_ON_START, "/procsegment", 512, 256, modules_cnt, &desc);
    //place for initial stream
    //finalization stream always in pipeline
    create_last_module_description(modules_cnt - 1, -1, &desc->mdescs[modules_cnt-1]);

    char **argv = NULL;

    argv = (char **)malloc(2 * sizeof(char*));
    argv[0] = strdup("bin/tmk-first");
    argv[1] = NULL;
    create_first_module_description(0, 1, argv, &(desc->mdescs[0]));
    free(argv[0]);
    free(argv);

    argv = (char **)malloc(2 * sizeof(char*));
    argv[0] = strdup("bin/tmk-middle");
    argv[1] = NULL;
    create_middle_module_description(1, 2, argv, &(desc->mdescs[1]));
    free(argv[0]);
    free(argv);

    argv = (char **)malloc(2 * sizeof(char*));
    argv[0] = strdup("bin/tmk-last");
    argv[1] = NULL;
    create_middle_module_description(2, 3, argv, &(desc->mdescs[2]));
    free(argv[0]);
    free(argv);

    *p_pdesc = desc;
}

void create_pg_pipeline_description(struct pipeline_description **p_pdesc) {
    struct pipeline_description *desc = *p_pdesc;

    int modules_cnt = 1 + 3;
    pipeline_description_init(DEFAULT_WAIT_ON_START, "/procsegment", 512, 256, modules_cnt, &desc);
    //place for initial stream
    //finalization stream always in pipeline
    create_last_module_description(modules_cnt - 1, -1, &desc->mdescs[modules_cnt-1]);

    char **argv = NULL;

    argv = (char **)malloc(2 * sizeof(char*));
    argv[0] = strdup("bin/tmk-first");
    argv[1] = NULL;
    create_first_module_description(0, 1, argv, &(desc->mdescs[0]));
    free(argv[0]);
    free(argv);

    argv = (char **)malloc(2 * sizeof(char*));
    argv[0] = strdup("bin/tmk-pg");
    argv[1] = NULL;
    create_middle_module_description(1, 2, argv, &(desc->mdescs[1]));
    free(argv[0]);
    free(argv);

    argv = (char **)malloc(2 * sizeof(char*));
    argv[0] = strdup("bin/tmk-last");
    argv[1] = NULL;
    create_middle_module_description(2, 3, argv, &(desc->mdescs[3]));
    free(argv[0]);
    free(argv);

    *p_pdesc = desc;
}

void create_pipeline_description(char *init_string, struct pipeline_description **p_pdesc) {
    struct pipeline_description *pdesc = *p_pdesc;
    if (strcmp(init_string, "empty") == 0) {
        create_empty_pipeline_description(&pdesc);
    }
    else if (strcmp(init_string, "minimal") == 0) {
        create_minimal_pipeline_description(&pdesc);
    }
    else if (strcmp(init_string, "demo") == 0) {
        create_demo_pipeline_description(&pdesc);
    }
    else if (strcmp(init_string, "default") == 0) {
        create_default_pipeline_description(&pdesc);
    }
    else if (strcmp(init_string, "pg") == 0) {
        create_default_pipeline_description(&pdesc);
    }
    else {
        fprintf(stdout, "Read pipeline from file: %s\n", init_string);
        fflush(stdout);
#if 1
        fprintf(stderr, "Reading from file %s is not supported yet\n", init_string);
        fflush(stderr);
        return;
#else
        struct pipeline_module_description *modules;
        int modules_cnt = read_modules_configuration(init_string, &modules);

        pipeline_description_init("/procsegment", 512, 256, modules_cnt, &pdesc);
        for (int i = 0; i < modules_cnt; i++) {
            pdesc->mdescs[i] = &modules[i];
        }
#endif
    }
    *p_pdesc = pdesc;
}

void free_pipeline_description(struct pipeline_description **p_pdesc) {
    if (!p_pdesc) return;
    pipeline_description_destroy(*p_pdesc);
    free(*p_pdesc);
    *p_pdesc = NULL;
}

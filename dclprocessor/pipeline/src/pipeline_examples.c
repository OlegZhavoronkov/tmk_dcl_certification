#include <pipeline/pipeline_examples.h>

//Examples of code for construct pipeline

void create_minimal_pipeline_description(struct pipeline_description **p_pdesc) {
    struct pipeline_description *desc = *p_pdesc;

    pipeline_description_init("/procsegment", 512, 256, 1+1, &desc);

    //finalization stream always in pipeline and last
    create_last_module_description(&desc->mdescs[0]);
#if 1
    char **argv = (char **)malloc(2 * sizeof(char*));
    argv[0] = strdup("bin/pclient");
    argv[1] = NULL;
#else
    char *argv[] = { "bin/pclient", NULL };
#endif
    create_first_module_description(argv, &(desc->mdescs[1]));
    desc->mdescs[1]->id = 1;
    desc->mdescs[1]->next = 0;
#if 1
    free(argv[0]);
    free(argv);
#endif
    *p_pdesc = desc;
}

void create_demo_pipeline_description(struct pipeline_description **p_pdesc) {
    struct pipeline_description *desc = *p_pdesc;

    pipeline_description_init("/procsegment", 512, 256, 2+1, &desc);

    //finalization stream always in pipeline and last
    create_last_module_description(&(desc->mdescs[0]));

    char **argv = NULL;

    argv = (char **)malloc(2 * sizeof(char*));
    argv[0] = strdup("bin/tmk-demo-capturer");
    argv[1] = NULL;
    create_first_module_description(argv, &(desc->mdescs[1]));
    desc->mdescs[1]->id = 1;
    desc->mdescs[1]->next = 2;
    free(argv[0]);
    free(argv);

    argv = (char **)malloc(2 * sizeof(char*));
    argv[0] = strdup("bin/tmk-demo-filter");
    argv[1] = NULL;
    create_middle_module_description(argv, &(desc->mdescs[2]));
    desc->mdescs[2]->id = 2;
    desc->mdescs[2]->next = 0;
    free(argv[0]);
    free(argv);

    *p_pdesc = desc;
}

void create_default_pipeline_description(struct pipeline_description **p_pdesc) {
    struct pipeline_description *desc = *p_pdesc;

    pipeline_description_init("/procsegment", 512, 256, 3+1, &desc);

    //finalization stream always in pipeline and last
    create_last_module_description(&(desc->mdescs[0]));

    char **argv = NULL;

    argv = (char **)malloc(2 * sizeof(char*));
    argv[0] = strdup("bin/tmk-first");
    argv[1] = NULL;
    create_first_module_description(argv, &(desc->mdescs[1]));
    desc->mdescs[1]->id = 1;
    desc->mdescs[1]->next = 2;
    free(argv[0]);
    free(argv);

    argv = (char **)malloc(2 * sizeof(char*));
    argv[0] = strdup("bin/tmk-middle");
    argv[1] = NULL;
    create_middle_module_description(argv, &(desc->mdescs[2]));
    desc->mdescs[2]->id = 2;
    desc->mdescs[2]->next = 3;
    free(argv[0]);
    free(argv);

    argv = (char **)malloc(2 * sizeof(char*));
    argv[0] = strdup("bin/tmk-last");
    argv[1] = NULL;
    create_middle_module_description(argv, &(desc->mdescs[3]));
    desc->mdescs[3]->id = 3;
    desc->mdescs[3]->next = 0;
    free(argv[0]);
    free(argv);

    *p_pdesc = desc;
}

void create_pg_pipeline_description(struct pipeline_description **p_pdesc) {
    struct pipeline_description *desc = *p_pdesc;

    pipeline_description_init("/procsegment", 512, 256, 3+1, &desc);

    //finalization stream always in pipeline and last
    create_last_module_description(&(desc->mdescs[0]));
    desc->mdescs[1]->id = 0;
    desc->mdescs[1]->next = -1;

    char **argv = NULL;
    
    argv = (char **)malloc(2 * sizeof(char*));
    argv[0] = strdup("bin/tmk-first");
    argv[1] = NULL;
    create_middle_module_description(argv, &(desc->mdescs[1]));
    desc->mdescs[1]->id = 1;
    desc->mdescs[1]->next = 2;
    free(argv);

    argv = (char **)malloc(2 * sizeof(char*));
    argv[0] = strdup("bin/tmk-pg");
    argv[1] = NULL;
    create_middle_module_description(argv, &(desc->mdescs[2]));
    desc->mdescs[2]->id = 2;
    desc->mdescs[2]->next = 3;
    free(argv);

    argv = (char **)malloc(2 * sizeof(char*));
    argv[0] = strdup("bin/tmk-last");
    argv[1] = NULL;
    create_middle_module_description(argv, &(desc->mdescs[3]));
    desc->mdescs[3]->id = 3;
    desc->mdescs[3]->next = 0;
    free(argv);

    *p_pdesc = desc;
}




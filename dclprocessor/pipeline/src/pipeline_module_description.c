#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <pipeline/pipeline_module_description.h>

int create_initial_module_description(int id, int next, struct pipeline_module_description **p_mdesc){
    struct pipeline_module_description *mdesc = *p_mdesc;
    if (!mdesc) mdesc = (struct pipeline_module_description *)malloc(sizeof(struct pipeline_module_description));
    mdesc->id = id;
    mdesc->argv = NULL;
    //mdesc->argv = (char **)malloc(1 * sizeof(char*));
    //mdesc->argv[0] = NULL;
    mdesc->type = MT_FIRST;
    mdesc->mode = MT_MODE_INTERNAL;
    mdesc->next = next;
    *p_mdesc = mdesc;
    return 1;
}

int create_first_module_description(int id, int next, char *argv[], struct pipeline_module_description **p_mdesc){
    int i;
    struct pipeline_module_description *mdesc = *p_mdesc;
    if (!mdesc) mdesc = (struct pipeline_module_description *)malloc(sizeof(struct pipeline_module_description));
    mdesc->id = id;
    int argc = 0;
    for (argc = 0; argv[argc] != NULL; argc++);
    mdesc->argv = (char **)malloc((argc + 1) * sizeof(char *));
    for (i = 0; argv[i] != NULL; mdesc->argv[i] = strdup(argv[i]), i++);
    mdesc->argv[i] = NULL;
    mdesc->type = MT_FIRST;
    mdesc->mode = MT_MODE_EXTERNAL;
    mdesc->next = next;
    *p_mdesc = mdesc;
    return 1;
}

int create_middle_module_description(int id, int next, char *argv[], struct pipeline_module_description **p_mdesc) {
    struct pipeline_module_description *mdesc = *p_mdesc;
    int i;
    if (!mdesc) mdesc = (struct pipeline_module_description *)malloc(sizeof(struct pipeline_module_description));
    mdesc->id = id;
    int argc = 0;
    for (argc = 0; argv[argc] != NULL; argc++);
    mdesc->argv = (char **)malloc((argc + 1) * sizeof(char *));
    for (i = 0; argv[i] != NULL; mdesc->argv[i] = strdup(argv[i]), i++);
    mdesc->argv[i] = NULL;
    mdesc->type = MT_MIDDLE;
    mdesc->mode = MT_MODE_EXTERNAL;
    mdesc->next = next;
    *p_mdesc = mdesc;
    return 1;
}

int create_last_module_description(int id, int next, struct pipeline_module_description **p_mdesc) {
    struct pipeline_module_description *mdesc = *p_mdesc;
    if (!mdesc) mdesc = (struct pipeline_module_description *)malloc(sizeof(struct pipeline_module_description));
    mdesc->id = id;
    mdesc->argv = (char **)malloc(1 * sizeof(char*));
    mdesc->argv[0] = NULL;
    mdesc->type = MT_LAST;
    mdesc->mode = MT_MODE_INTERNAL;
    mdesc->next = next;
    *p_mdesc = mdesc;
    return 1;
}

void out_pipeline_module_description(struct pipeline_module_description *mdesc) {
    if (!mdesc) {
        fprintf(stderr, "[%s]: desription is NULL\n", __FUNCTION__);
        fflush(stderr);
        return;
    }
    fprintf(stdout, "[%d] Command string: ", mdesc->id);
    for (int i = 0; mdesc->argv[i] != NULL; fprintf(stdout, "argv[%d]=%s ", i, mdesc->argv[i]), i++);
    fprintf(stdout, "id: %d, type: %d, next_id: %d\n", mdesc->id, mdesc->type, mdesc->next);
    fflush(stdout);
}

void clean_pipeline_module_description(struct pipeline_module_description *mdesc){
    if (!mdesc || !mdesc->argv) return;
    for (int i = 0; mdesc->argv[i] != NULL; i++) {
            free(mdesc->argv[i]);
    }
};


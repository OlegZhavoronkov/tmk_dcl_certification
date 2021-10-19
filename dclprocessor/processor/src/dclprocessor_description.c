#include <string.h>
#include <stdlib.h>
#include <stdio.h>

#include <misc/log.h>
#include <processor/dclprocessor_description.h>

void create_default_dclprocessor_description(struct dclprocessor_description **pdesc) {
    if (!pdesc || *pdesc == NULL) return;
    struct dclprocessor_description *desc = *pdesc;
    if (!desc) desc = (struct dclprocessor_description *)malloc(sizeof(struct dclprocessor_description));
    if (!desc) {
        LOG_FD_ERROR("can't allocte dclprocessor\n");
        return;
    }
    memset(desc, 0, sizeof(struct dclprocessor_description));
    //collector_init("tcp", &pdesc->collector_desc); //tcp collector as 1 source on 9000
    //pipeline_init("default", &pdesc->pipeline_desc); //3 modules: first with collector, middle, last
    //TODO: tcp subsystem description
}

void create_dclprocessor_description(char *init_string, struct dclprocessor_description **pdesc) {
    (void)init_string;
    (void)pdesc;
}

void out_dclprocessor_description(struct dclprocessor_description *desc) {
    fprintf(stdout, "Dclprocessor description:\n");
    out_pipeline_description(desc->pipeline_desc);
    out_collector_description(desc->collector_desc);
    fflush(stdout);
}

void destroy_dclprocessor_description(struct dclprocessor_description **p_desc) {
    if (p_desc == NULL || *p_desc == NULL )  return;
    struct dclprocessor_description *desc = *p_desc;
    if (desc->pipeline_desc) pipeline_description_destroy(desc->pipeline_desc);
    if (desc->collector_desc) collector_description_destroy(&desc->collector_desc);
    free(desc);
    *p_desc = NULL;
}


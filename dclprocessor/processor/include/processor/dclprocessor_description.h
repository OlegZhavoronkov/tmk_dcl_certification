#ifndef _DCLPROCESSOR_DESCRIPTION_H_
#define _DCLPROCESSOR_DESCRIPTION_H_

#ifdef __cplusplus
extern "C" {
#endif

#include <pipeline/pipeline_description.h>
#include <collector/collector_description.h>
//#include <tcp/messenger_description.h>

struct dclprocessor_description {
    struct pipeline_description *pipeline_desc;
    struct collector_description *collector_desc;
    //struct messenger_description *messenger_desc;
};

void create_default_dclprocessor_description(struct dclprocessor_description **pdesc);

void create_dclprocessor_description(char *init_string, struct dclprocessor_description **pdesc);
void out_dclprocessor_description(struct dclprocessor_description *desc);
void destroy_dclprocessor_description(struct dclprocessor_description **p_desc);

#ifdef __cplusplus
}
#endif

#endif //_DCLPROCESSOR_DESCRIPTION_H_


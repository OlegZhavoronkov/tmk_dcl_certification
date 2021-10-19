#ifndef _PIPELINE_SERVER_H
#define _PIPELINE_SERVER_H

#ifdef __cplusplus
extern "C" {
#endif

#include <misc/list.h>

struct pipeline;
struct pipeline_description;

int pipeline_init(struct pipeline **p_pl, struct pipeline_description *pdesc);
void pipeline_destroy(struct pipeline *pl);

//TODO: implement this commands
int pipeline_start(struct pipeline *pl);
int pipeline_stop(struct pipeline *pl);

#ifdef __cplusplus
}
#endif

#endif //_PIPELINE_SERVER_H

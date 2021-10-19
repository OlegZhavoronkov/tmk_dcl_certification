#ifndef _PIPELINE_EXAMPLES_H_
#define _PIPELINE_EXAMPLES_H_

#ifdef __cplusplus
extern "C" {
#endif

struct pipeline_description;

//example: only source module and final thread
void create_minimal_pipeline_description(struct pipeline_description **p_pdesc);
//default pipeline: first, middle, last without parametes
void create_default_pipeline_description(struct pipeline_description **p_pdesc);
//demo with real data
void create_pg_pipeline_description(struct pipeline_description **p_pdesc);
//only 3 modules: first module waiting on input
void create_demo_pipeline_description(struct pipeline_description **p_pdesc);

#ifdef __cplusplus
}
#endif

#endif //_PIPELINE_EXAMPLES_H_


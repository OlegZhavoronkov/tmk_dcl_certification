#ifndef _PIPELINE_EXAMPLES_H_
#define _PIPELINE_EXAMPLES_H_

#ifdef __cplusplus
extern "C" {
#endif

struct pipeline_description;

void create_pipeline_description(char *init_string, struct pipeline_description **p_pdesc);
void free_pipeline_description(struct pipeline_description **p_pdesc);

//"on fly" empty pipeline: only initial and final threads
void create_empty_pipeline_description(struct pipeline_description **p_pdesc);
//example: 2 custom modules with autogenerated objects
void create_minimal_pipeline_description(struct pipeline_description **p_pdesc);
//default pipeline: 3 custom modules first, middle, last without parametes
void create_default_pipeline_description(struct pipeline_description **p_pdesc);
//default pipeline with real data
void create_pg_pipeline_description(struct pipeline_description **p_pdesc);
//two custom modules: capturing and filter modules waiting on input
void create_demo_pipeline_description(struct pipeline_description **p_pdesc);

#ifdef __cplusplus
}
#endif

#endif //_PIPELINE_EXAMPLES_H_


#ifndef _PIPELINE_MODULE_DESCRIPTION_H_
#define _PIPELINE_MODULE_DESCRIPTION_H_

#ifdef __cplusplus
extern "C" {
#endif

enum MT_MODE {
    MT_MODE_INTERNAL = 0,
    MT_MODE_EXTERNAL = 1
};

enum MT_TYPE {
    MT_UNDEFINED = -1,
    MT_FIRST = 1,
    MT_MIDDLE = 2,
    MT_LAST = 0
};

struct pipeline_module_description {
    int id;       //idx of process in the process table
    enum MT_TYPE type; //for choose a type of the thread (source/middle/final)
    int mode;     //MD_MODE of struct pipeline_module, set with initial/first
    int next;     //idx of the next in the pipeline model
    char **argv;  //command string for start module
};

//last module is internal thread of pipeline, argv[0] = NULL, id = -1
int create_last_module_description(int id, int next, struct pipeline_module_description **p_mdesc);
//first module as internal thread of pipeline, argv[0] = NULL, id = 0 -- only for empty pipeline
int create_initial_module_description(int id, int next, struct pipeline_module_description **p_mdesc);
//first module as external process -- for predefined pipeline
int create_first_module_description(int id, int next, char *argv[], struct pipeline_module_description **p_mdesc);
//any number of middle allowed, id should be number in [1 ; cnt-2] 
int create_middle_module_description(int id, int next, char *argv[], struct pipeline_module_description **p_mdesc);

void out_pipeline_module_description(struct pipeline_module_description *mdesc);
void clean_pipeline_module_description(struct pipeline_module_description *mdesc);

#ifdef __cplusplus
}
#endif


#endif //_PIPELINE_MODULE_DESCRIPTION_H_

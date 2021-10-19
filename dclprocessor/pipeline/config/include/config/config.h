//-----------------------------------------------------------------------
// Created : 26.03.2021
// Author : Alin42
// Description : Json configuration file parser to "struct module_t[n]"
//               return value: < 0 if it failed, n if not
//-----------------------------------------------------------------------

#ifndef _PIPELINE_CONFIG_H
#define _PIPELINE_CONFIG_H

#include <pipeline/pipeline_module_description.h>
#include <pipeline/pipeline_description.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef struct pipeline_module_description module_t;

static inline void module_description_free(module_t *target){
#if 0
    if (!target || !target->argv) return;
    for (int i = 0; target->argv[i] != NULL; i++)
        free(target->argv[i]);
#else
    clean_pipeline_module_description(target);
#endif
};

int read_modules_configuration(const char *fileName, module_t **target);
int read_pipeline_configuration(const char *fileName, struct pipeline_description **p_desc);

//TODO: think about this place -- modules are part of pipeline
#include "stdlib.h"
inline void module_t_clean_up(module_t **modules, int size, int until) {
    if (until == -1) {
        until = size;
    }
    for (int i = 0; i < until; ++i) {
        module_description_free(&(*modules)[i]);
    }
    if (modules && size != -1) {
        free(*modules);
    }
    *modules = NULL;
}

#define CONFIG_EXTRA_CHECK

#ifdef __cplusplus
}
#endif

#endif // _PIPELINE_CONFIG_H_

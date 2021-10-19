#ifndef _NAME_GENERATOR_H_
#define _NAME_GENERATOR_H_

#ifdef __cplusplus
extern "C" {
#endif

#include <stdio.h>
#include <time.h>
#include <sys/time.h>
#include <string.h>

//%04d-%02d-%02d_%02d-%02d-%02d.%09ld
#define DT_POSTFIX_SIZE          34
//this const should be LESS then data_size of pipeline_descriptor
#define MAX_GENERATED_NAME_LEN  248

static inline void generate_dt_postfix(char *postfix) {
   struct tm tm;
   struct timeval tv;
   gettimeofday(&tv, NULL);
   localtime_r(&tv.tv_sec, &tm);
   snprintf(postfix, DT_POSTFIX_SIZE, "%04d-%02d-%02d_%02d-%02d-%02d.%06ld",
       1900 + tm.tm_year, tm.tm_mon + 1, tm.tm_mday, tm.tm_hour, tm.tm_min, tm.tm_sec, tv.tv_usec);
}

static inline void generate_new_object_name(char *name) {
   char postfix[DT_POSTFIX_SIZE];
   generate_dt_postfix(postfix);
   snprintf(name, MAX_GENERATED_NAME_LEN, "object_%s", postfix);
}

static inline void generate_name(char *name, char *base, char *postfix) {
   snprintf(name, MAX_GENERATED_NAME_LEN, "%s_%s", base, postfix);
}

static inline void generate_name_with_id(char *name, char *base, int id, char *postfix) {
   snprintf(name, MAX_GENERATED_NAME_LEN, "%s_%d_%s", base, id, postfix);
}

static inline void generate_name_with_src_and_id(char *name, char *base, int src_id, int id, char *postfix) {
   snprintf(name, MAX_GENERATED_NAME_LEN, "%s_%d_%d_%s", base, src_id, id, postfix);
}

static inline void get_object_postfix(char *name, char *postfix) {
   char *tmp = strchr(name, '_');
   strcpy(postfix, ++tmp);
}

#ifdef __cplusplus
}
#endif

#endif //_NAME_GENERATOR_H_

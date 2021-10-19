#ifndef _STORAGE_INTERFACE_H_
#define _STORAGE_INTERFACE_H_

#ifdef __cplusplus
extern "C" {
#endif

#include <stddef.h>
//TODO BOOST_SEGMENT_NAME_ENV - config
#define BOOST_SEGMENT_NAME_ENV  "BOOST_SEGMENT_NAME"
#define SHMEM_NAME_LEN 255

size_t storage_interface_init(char* segment_name, size_t size);
size_t storage_interface_get_object(void **obj_ptr, char* segment_name, char *obj_name);
size_t storage_interface_new_object(char* segment_name, char *obj_name);
size_t storage_interface_new_object_with_framesets(char* segment_name, char *obj_name, int source_number);
size_t storage_interface_add_frame_to_object_with_framesets(char *segment_name, char *obj_name, int source_id, int frame_id, void* img);
void storage_interface_analyze_object_with_framesets(char *segment_name, char *obj_name);
size_t storage_interface_free_object(char* segment_name, char *obj_name);
void storage_interface_destroy(char* segment_name);

#ifdef __cplusplus
}
#endif

#endif //_STORAGE_INTERFACE_H_

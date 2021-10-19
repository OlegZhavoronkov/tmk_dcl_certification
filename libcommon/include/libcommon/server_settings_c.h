#pragma once
#ifndef __SERVER_SETTINGS_C_H__
#define __SERVER_SETTINGS_C_H__


#include <stddef.h>
#ifdef __cplusplus
extern "C" {
#endif

typedef struct server_settings
{
    const char* address;                //NULL for IFADDR_ANY
    unsigned int port;      
    int command_queue_size;             //COMMAND_QUEUE_SIZE
    const char* shmem_data_name;        //SHMEM_DATA_NAME_ENV
    size_t shmem_data_size_env;         //SHMEM_DATA_SIZE_ENV;
    const char* boost_segment_name_env; //BOOST_SEGMENT_NAME_ENV
    size_t minimal_boost_size;          //MINIMAL_BOOST_SIZE;
} server_settings_t;

#ifdef __cplusplus
}
#endif

#endif // __SERVER_SETTINGS_C_H__
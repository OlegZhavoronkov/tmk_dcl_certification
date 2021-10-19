#ifndef _TCP_PROCESS_H_
#define _TCP_PROCESS_H_

#ifdef __cplusplus
extern "C" {
#endif

#include <misc/list.h>
#define MAX_BUFFER_SIZE 512

#if 0
struct connection{
    list_t entry;
    //int broken;
    char request[MAX_BUFFER_SIZE];
    size_t request_used;
    char reply[MAX_BUFFER_SIZE];
    size_t reply_used;
    int fd;
};

//srv == NULL for client
struct connection *connection_add(void *ptr, int fd);
void connection_delete(void *ptr, struct connection *connection);
#else
#include "connection.h"
#endif

typedef int(*reciever_func_t) (struct connection *connection, int *exit_flag);
typedef int(*sender_func_t) (struct connection *connection, int *exit_flag);

//prepared request from client to server before write (usually this wait on input)
int default_client_sender(struct connection *connection, int *exit_flag);
//analized reply from server to client after read (usually this is analyzer for server replyes)
int default_client_reciever(struct connection *connection, int *exit_flag);
//prepared reply for write from server to client (usually this is callback for long actions ???)
int default_server_sender(struct connection *connection, int *exit_flag);
//analized request from client to server after read (usually this is command executor)
int default_server_reciever(struct connection *connection, int *exit_flag);

int _tcp_server_process(char *port, int(*server_reciever) (struct connection *, int *exit_flag), int(*server_sender) (struct connection *, int *exit_flag));
int _tcp_client_process(char* addr, char *port, int (*client_reciever) (struct connection *, int *exit_flag), int(*client_sender) (struct connection *, int *exit_flag));

#ifdef __cplusplus
}
#endif

#endif// _TCP_PROCESS_H_


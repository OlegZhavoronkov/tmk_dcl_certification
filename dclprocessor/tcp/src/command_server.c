#define _GNU_SOURCE //should be first

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>

#include <pthread.h>

#include <misc/ring_buffer.h>
#include <tcp/command_server.h>
#include <pipeline/pipeline_server.h>
#include <pipeline/config/config.h>

//#include "api/api.h"

static struct pipeline *pl = NULL;
static struct ring_buffer *cmd_ring = NULL;
static sem_t command_ready;

int executeAPICommand(const char *cmd, int *exit_flag) {
    fprintf(stdout, "[command_server]: TODO real execute command by api\n");
    fflush(stdout);
    if (strcasecmp(cmd, "STOP\n") == 0 || strcasecmp(cmd, "S\n") == 0 ||
        strcasecmp(cmd, "STOP") == 0 || strcasecmp(cmd, "S") == 0) { 
        fprintf(stdout, "[command_server]: STOP command recieved\n");
        fflush(stdout);
        *exit_flag = 1;
        if (pl) pipeline_destroy(pl);
        pl= NULL;
    }
    else if (strcasecmp(cmd, "INIT\n") == 0 || strcasecmp(cmd, "I\n") == 0 ||
             strcasecmp(cmd, "INIT") == 0 || strcasecmp(cmd, "I") == 0 ) { 
        fprintf(stdout, "[command_server]: INIT command recieved, not supported\n");
        fflush(stdout);
    }
    else if (strcasestr(cmd, "CONFIG") != 0) {
        fprintf(stdout, "[command_server]: CONFIG command recieved\n");
        fflush(stdout);
        //waits on input is not good idea for any server solution
        //it waits IN the SERVER terminal and get answer too...
        //TODO: use api which can support parameters as filename with the path
        struct pipeline_description *pdesc = NULL;
        read_pipeline_configuration(cmd + strlen("CONFIG:"), &pdesc);
        pipeline_init(&pl, pdesc);
        if (pl == NULL) {
            fprintf(stdout, "[command_server]: command '%s' failed\n", cmd);
            fflush(stdout);
        }
        //else {
            //TODO: make callback ???
            //we needed in callback on reply after init because this reply is not informative about real status
            //fprintf(stdout, "[command_server]: starting model from %s\n", fname);
            //fflush(stdout);
        //}
    }
    else {
        fprintf(stdout, "[command_server]: Received unknown command '%s'\n", cmd);
        fflush(stdout);
        return -1;
    }
    return 0;
}

int push_new_command(int fd, const char *request) {
    //Create command and add into queue
    command_t *new_cmd = (command_t *)malloc(sizeof(command_t));
    memset(new_cmd, 0, sizeof(command_t));
    strcpy(new_cmd->full_command, request);
    size_t n = strlen(new_cmd->full_command - 1);
    if (new_cmd->full_command[n - 1] == '\n') new_cmd->full_command[n - 1] = '\0';
    new_cmd->fd = fd;
    fprintf(stdout, "[command_server]: put to queue new command %s\n", new_cmd->full_command);
    fflush(stdout);
    ring_buffer_push(cmd_ring, (size_t)new_cmd);
    sem_post(&command_ready);
    return 0;
}

command_t *pop_next_command() {
    sem_wait(&command_ready);
    return (command_t*)ring_buffer_pop(cmd_ring);
}

void* command_thread(void *arg) {
    //pthread_mutex_lock(&stop_mutex);
    //??? exit_flag
    //pthread_mutex_unlock(&stop_mutex);
    struct command_server *cmdsrv = (struct command_server *)arg;
    command_t *cmd;
    char command[COMMAND_BUF_LEN];
    int rc;

    fprintf(stdout, "[command_server]: Starting command server\n");
    fflush(stdout);

    while (!(*(cmdsrv->exit_flag))) {
        cmd = pop_next_command();
        if (cmd == NULL) {
            sleep(1);
            continue;
        }
        fprintf(stdout, "[command_server]: Received command '%s'\n", cmd->full_command);
        fflush(stdout);
        //Execute command by API
        executeAPICommand(cmd->full_command, cmdsrv->exit_flag);
        if (cmd) free(cmd);
        cmd = NULL;
    }

    fprintf(stdout, "[command_server]: Stopping command server...\n");
    fflush(stdout);
    return NULL;
}

struct command_server *command_server_init(int *exit_flag){
    struct command_server *cmdsrv = (struct command_server *)malloc(sizeof(struct command_server));
    //TODO COMMAND_QUEUE_SIZE config
    cmd_ring = ring_buffer_create(COMMAND_QUEUE_SIZE, 0);
    if (cmd_ring == NULL) {
        fprintf(stdout, "[command_server]: can't create ring buffer\n");
        fflush(stdout);
        *exit_flag = 1;
        free(cmdsrv);
        return NULL;
    }
    cmdsrv->exit_flag = exit_flag;
    pthread_create(&cmdsrv->thread_id, NULL, command_thread, (void *)cmdsrv);
    return cmdsrv;
}

int command_server_destroy(struct command_server *cmdsrv) {
    //TODO: fake command for this aim
    *(cmdsrv->exit_flag) = 1;
    push_new_command(0, "STOP\n");
    //TODO: wait for finish
    pthread_join(cmdsrv->thread_id, NULL);
    ring_buffer_destroy(cmd_ring);
    free(cmdsrv);
    return 0;
}


#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#if 0
#include <locale.h>
#include <pthread.h>
#include <misc/signal_reaction.h>
#endif

    #include <tcp/connection.h>
#if 1
    #include <tcp/command_server.h>
    #include <tcp/tcp_process.h>
#else
    #include <tcp/messenger.h>
#endif


#include <libcommon/configuration_c.h>
#if 0
void takeCommand(command_t *cmd, int *exit_flag, void (*callback)(command_t *)) {
    (void)callback;
#if 1
    //this function
    defaultExecuteCommand(cmd, exit_flag, defaultSendReply);
    //executeSimpleCommand(cmd, exit_flag);
#else
    //parse commands and execute it with defaultSendReply or your own callback
    //if more then one action, please, group it executors to this function
    executeAPICommand(cmd, exit_flag, defaultSendReply);
#endif
}
#endif

//think about *exit_flag = -1/0/1 with -1 at start, 0 on init() and 1 on stop()
int putCommand(struct connection *connection, int *exit_flag) {
    char *request = connection->request;
    //TODO: queue of commands, move command_server to libtcp ???
    //this place is main stop for command server (it sets exit flag and then put stop command to queue)
    if (strcasecmp(request, "STOP\n") == 0 || strcasecmp(request, "S\n") == 0 ||
        strcasecmp(request, "STOP") == 0 || strcasecmp(request, "S") == 0 ||
        strcasecmp(request, "QUIT\n") == 0 || strcasecmp(request, "Q\n") == 0 ||
        strcasecmp(request, "QUIT") == 0 || strcasecmp(request, "Q") == 0 ||
        strcasecmp(request, "EXIT\n") == 0 || strcasecmp(request, "EXIT") == 0 ) {
        fprintf(stdout, "[command_server]: STOP/QUIT command recieved from %d\n", connection->fd);
        fflush(stdout);
        *exit_flag = 1;
    }
//------------------------------------------------------------------------------
// This is a place if we want do something before add command to queue
//------------------------------------------------------------------------------
    //Create command and add into queue
    push_new_command(connection->fd, connection->request);
    //TODO: answer the time when command was sent
    strcpy(connection->reply, "command succesfully sent");
    connection->reply_used = strlen(connection->reply);
    return strlen(connection->reply);
}

int main(int argc, char *argv[]) {
    int exit_flag = 0;
    const char* configPath = NULL;
    char* portFromCmdLine = NULL;
    const struct server_settings* pSettings = NULL;

    //TODO: get_opt_long
    if (argc > 1) {
        if(strcasecmp(argv[1],"--config") == 0 && argc > 2){
            configPath = argv[2];
        }
        else{
            portFromCmdLine = argv[1];
        }
    }
    if(configPath != NULL){
        pSettings = loadServerSettings(configPath);
        if(pSettings == NULL){
            saveDefaultConfig(configPath);
            configPath = NULL;
        }
    }
    if(portFromCmdLine == NULL && configPath == NULL){
        fprintf(stderr, "Usage: %s port or %s --config [path_to_config]\n", argv[0],argv[0]);
        exit(EXIT_FAILURE);
    }

#if 0
    //backtrace for exception
    setlocale(LC_ALL, "");
    set_signal_reactions();
    signal_thread_init(&exit_flag);
#endif

#if 1
    struct command_server *cmdsrv = command_server_init(&exit_flag);
    if (cmdsrv != NULL) {
        if (pSettings != NULL)
        {
            char port_string[8]={0};
            snprintf(port_string , 7,"%d",pSettings->port);
            _tcp_server_process(port_string, putCommand, 0);
        }
        else{
            _tcp_server_process(portFromCmdLine, putCommand, 0);
        }

        command_server_destroy(cmdsrv);
    }
#else
    if (argc != 2) {
        fprintf(stderr, "Usage: %s port\n", argv[0]);
        exit(EXIT_FAILURE);
    }

    struct server *srv = server_create(argv[1], 10, NULL, NULL);
    while (!srv->exit_flag) {
        sleep(2);
    }

    server_destroy(srv);
#endif

#if 0
    signal_thread_destroy();
#endif

    return 0;
}


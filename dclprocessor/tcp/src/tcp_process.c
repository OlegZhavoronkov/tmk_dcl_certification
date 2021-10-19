#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include <errno.h>
#include <fcntl.h>
#include <unistd.h>

#include <sys/socket.h>
#include <sys/select.h>
#include <netdb.h>

#include <unistd.h>

#include <misc/log.h>
#include <tcp/tcp_process.h>
#include <libcommon/configuration_c.h>

#include <tcp/server.h>

#define MESSAGE_LEN 512

//char *default_command_executor(struct connection *connection, int *exit_flag) {
int default_server_reciever(struct connection *connection, int *exit_flag) {
    fprintf(stdout, "[default_server_reciever]: we are IN the server reciever\n");
    fflush(stdout);
    if (!connection) return 0;
    char *buf = connection->request;
    if (strcasecmp(buf, "STOP") == 0 || strcasecmp(buf, "S") == 0 ||
        strcasecmp(buf, "QUIT") == 0 || strcasecmp(buf, "Q") == 0 ||
        strcasecmp(buf, "EXIT") == 0) {
        fprintf(stdout, "[default_command_executor]: stop server command recieved\n");
        fflush(stdout);
        *exit_flag = 1;
        sprintf(connection->reply, "RE%lx{0:%d, stop command recieved}\n", (ssize_t)28, connection->fd);
    }
    else {
        sprintf(connection->reply, "RE%lx{0:%d,%s,recieved ok}", strlen(connection->request) + 24 + 2, connection->fd, connection->request);
    }
    connection->reply_used = strlen(connection->reply);
    return strlen(connection->reply);
}

//int sample_callback(struct connection *connection, int *exit_flag) 
int default_server_sender(struct connection *connection, int *exit_flag) {
    (void)exit_flag;
    fprintf(stdout, "[default_server_sender]: we are IN the server sender\n");
    fflush(stdout);
#if 0
    int i = 0;
    //TODO: ???? something not good with this idea
    strcpy(connection->reply, "{sample_callback} please, wait...");
    while (strcmp(connection->reply, "") != 0 && ++i < 10) {
        write(connection->fd, connection->reply, strlen(connection->reply));
        sleep(1);
    }
    strcpy(connection->reply, "{sample_callback} reciever is ok");
#else
    sprintf(connection->reply, "RE%lx{0:%d,%s,sent from sender ok}", strlen(connection->request) + 32 + 2, connection->fd, connection->request);
    connection->reply_used = strlen(connection->reply);
#endif
    return strlen(connection->reply);
}

int _tcp_server_process(char *port, int(*reciever) (struct connection *, int *exit_flag), int(*sender) (struct connection *, int *exit_flag)) {
    int exit_flag = 0;
    int fd;

    struct server *srv = NULL; 
    server_init(&srv, port);

    struct addrinfo hints;
    struct addrinfo *result, *rp;
    int rc, reuse_addr;

    memset(&hints, 0, sizeof(struct addrinfo));
    hints.ai_family = AF_UNSPEC;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_flags = AI_PASSIVE;
    hints.ai_protocol = 0;
    hints.ai_canonname = NULL;
    hints.ai_addr = NULL;
    hints.ai_next = NULL;

    //THIS IS SERVER: always listen on localhost (not NULL value is only one of available interface)
#if 0
    //with this usage connection is closed immediately after open
    const struct server_settings* pServerSettings = getServerSettings();
    rc = getaddrinfo(pServerSettings == NULL ?  NULL : pServerSettings->address,
                     port, &hints, &result);
#else
    rc = getaddrinfo(NULL, port, &hints, &result);
#endif
    if (rc != 0) {
        fprintf(stderr, "[tcp_server_process]: getaddrinfo error %s\n", gai_strerror(rc));
        fflush(stderr);
        exit(EXIT_FAILURE);
    }

    for (rp = result; rp != NULL; rp = rp->ai_next) {
        fd = socket(rp->ai_family, rp->ai_socktype, rp->ai_protocol);
        if (fd == -1) continue;

        reuse_addr=1;
        if (setsockopt(fd, SOL_SOCKET, SO_REUSEADDR, &reuse_addr, sizeof(reuse_addr)) != 0) {
            close(fd);
            continue;
        }

        if (bind(fd, rp->ai_addr, rp->ai_addrlen) == 0) break;
        close(fd);
    }

    if (rp == NULL) {
        fprintf(stderr, "[tcp_server_process]: could not bind\n");
        fflush(stderr);
        exit(EXIT_FAILURE);
    }

    freeaddrinfo(result);

    if (listen(fd, 10) != 0){
        fprintf(stderr, "[tcp_server_process]: could not listen with error: %s\n", strerror(errno));
        fflush(stderr);
        exit(EXIT_FAILURE);
    }

    while(!exit_flag){
        fd_set rfds;
        int retval, max;
        list_t *item;
        struct connection *connection;
        FD_ZERO(&rfds);

        max = fd;
        FD_SET(fd, &rfds);

        item = list_first_elem(&srv->connection_list);
        while(list_is_valid_elem(&srv->connection_list, item)){
            connection = list_entry(item, struct connection, entry);
            item = item->next;
            if (connection->fd > max) max = connection->fd;
            FD_SET(connection->fd, &rfds);
        }

        retval = select(max + 1, &rfds, NULL, NULL, NULL);
        if (retval == -1){
            fprintf(stderr, "[tcp_server_process]: could not select with error: %s\n", strerror(errno));
            fflush(stderr);
            exit(EXIT_FAILURE);
        }

        if (FD_ISSET(fd, &rfds)){
            //new connection
            struct sockaddr_storage peer_addr;
            socklen_t peer_addr_len;
            char host[NI_MAXHOST], service[NI_MAXSERV];
            int fd1;

            peer_addr_len = sizeof(peer_addr);
            fd1 = accept(fd, (struct sockaddr *) &peer_addr, &peer_addr_len);
            if (fd1 != -1){
                retval = getnameinfo((struct sockaddr *) &peer_addr, peer_addr_len, host, NI_MAXHOST, service, NI_MAXSERV, NI_NUMERICSERV);
                if (retval == 0) {
                    fprintf(stdout, "[tcp_server_process]: received connection from %s:%s\n", host, service);
                    fflush(stdout);
                }
                else {
                    fprintf(stderr, "[tcp_server_process]: getnameinfo error %s\n", gai_strerror(retval));
                    fflush(stderr);
                }

                if (connection_add(&srv->connection_list, &srv->connection_cnt, fd1) == NULL){
                    //close connection
                    fprintf(stderr, "[tcp_server_process]: max connections reached, drop new connection\n");
                    fflush(stderr);
                    close(fd1);
                }
            }
        }

        item = list_first_elem(&srv->connection_list);
        while (!exit_flag && list_is_valid_elem(&srv->connection_list, item)){
            ssize_t bytes;
            int remainder;
            char *endl;

            connection = list_entry(item, struct connection, entry);
            item = item->next;

            //if (connection->broken) continue;
            if (!FD_ISSET(connection->fd, &rfds)) continue;
            //data by connection connection->fd
            remainder = sizeof(connection->request) - connection->request_used;
            if (remainder <= 0){
                //connection_mark_broken(&srv, connection);
                continue;
            }
            bytes = read(connection->fd, connection->request + connection->request_used, remainder);
            if (bytes <= 0){
                //connection_mark_broken(&srv, connection);
                if (bytes < 0){
                    fprintf(stdout, "[tcp_server_process]: read error %s\n", strerror(errno));
                    fflush(stdout);
                }
                fprintf(stdout, "[tcp_server_process]: close connection %d\n", connection->fd);
                fflush(stdout);
                connection_delete(&srv->connection_list, &srv->connection_cnt, connection);
                continue;

            }
            connection->request_used += bytes;
            //working with recieved data, find the end of line
            endl = memchr(connection->request, '\n', connection->request_used);
            if (endl) {
                *endl = '\0';
                fprintf(stdout, "[tcp_server_process]: {connection %d} %s\n", connection->fd, connection->request);
                fflush(stdout);
                if (reciever == NULL) {
                    fprintf(stdout, "[tcp_server_process]: we are with the null server reciever\n");
                    fflush(stdout);
                    default_server_reciever(connection, &exit_flag);
                }
                else { //make something with connection->request
                    fprintf(stdout, "[tcp_server_process]: we are with the NOT null server reciever\n");
                    fflush(stdout);
                    reciever(connection, &exit_flag);
                }
                if (sender == NULL) {
                    fprintf(stdout, "[tcp_server_process]: we are with the null server sender\n");
                    fflush(stdout);
                    sprintf(connection->reply, "RE%lx{0:%d,%s,sent ok}\n", strlen(connection->request) + 20 + 2, connection->fd, connection->request);
                    connection->reply_used = strlen(connection->reply);
                }
                else { //make something with connection->reply
                    fprintf(stdout, "[tcp_server_process]: we are with the NOT null server sender\n");
                    fflush(stdout);
                    sender(connection, &exit_flag);
                }
                write(connection->fd, connection->reply, strlen(connection->reply));
                endl++;
                memmove(connection->request, endl, sizeof(connection->request) - (endl - connection->request));
                connection->request_used -= (endl - connection->request);
            }
       }
    }
    close(fd);
    server_destroy(srv);
    return 0;
}

int default_client_sender(struct connection *connection, int *exit_flag) { 
    fprintf(stdout, "[default_client_sender]: we are IN the client sender\n");
    fflush(stdout);
    if (!connection) return 0;
    (void)exit_flag;
    fprintf(stdout, "[default_client_sender]$ ");
    fflush(stdout);
    scanf("%[^\n]%*c", connection->request);
    strcat(connection->request, "\n");
    return strlen(connection->request);
}

int default_client_reciever(struct connection *connection, int *exit_flag) {
    (void)exit_flag;
    fprintf(stdout, "[default_client_reciever]: we are IN the client reciever\n");
    fflush(stdout);
    if (!connection) return 0;
    fprintf(stdout, "[default_client_reciever]: %s\n", connection->reply);
    fflush(stdout);
    return strlen(connection->reply);
}

int _tcp_client_process(char* addr, char *port, int(*reciever) (struct connection *, int *exit_flag), int(*sender) (struct connection *, int *exit_flag)){
    int fd;
    struct addrinfo hints;
    struct addrinfo *result, *rp;
    int rc;
    int exit_flag = 0;

    memset(&hints, 0, sizeof(struct addrinfo));
    hints.ai_family = AF_UNSPEC;     //IPv4 && IPv6
    hints.ai_socktype = SOCK_STREAM; //TCP socket
    hints.ai_flags = 0;
    hints.ai_protocol = 0;           //Any protocol

    rc = getaddrinfo(addr, port, &hints, &result);
    if (rc != 0) {
        fprintf(stderr, "[tcp_client_process]: getaddrinfo error %s\n", gai_strerror(rc));
        fflush(stderr);
        exit(EXIT_FAILURE);
    }

    for (rp = result; rp != NULL; rp = rp->ai_next) {
        fd = socket(rp->ai_family, rp->ai_socktype, rp->ai_protocol);
        if (fd == -1) continue;
        if (connect(fd, rp->ai_addr, rp->ai_addrlen) != -1) break;
        close(fd);
    }

    if (rp == NULL) {
        fprintf(stderr, "[tcp_client_process]: could not connect\n");
        fflush(stderr);
        exit(EXIT_FAILURE);
    }

    freeaddrinfo(result);

    struct connection *connection = connection_add(NULL, NULL, fd);
    while(!exit_flag){
        ssize_t bytes;
        int remainder;
        char *endl;

        if (sender == NULL) sender = default_client_sender;
        sender(connection, &exit_flag);

        bytes = write(connection->fd, connection->request, strlen(connection->request));
        if ((size_t)bytes != strlen(connection->request)) {
            if (bytes < 0) {
                fprintf(stderr, "[tcp_client_process]: write error %s\n", strerror(errno));
                fflush(stderr);
            }
            break;
        }

        remainder = sizeof(connection->reply) - connection->reply_used;
        if (remainder <= 0){
            //TODO:
            fprintf(stderr, "[tcp_client_process]: reply_used %ld more then reply length %ld\n", connection->reply_used, sizeof(connection->reply));
            fflush(stderr);
            continue;
        }
        bytes = read(connection->fd, connection->reply + connection->reply_used, remainder);
        if (bytes <= 0){
            fprintf(stdout, "[tcp_client_process]: read error %s\n", strerror(errno));
            fflush(stdout);
            exit_flag = 1;
            break;
        }
        connection->reply_used += bytes;
        endl = memchr(connection->reply, '\n', connection->reply_used);
        if (endl) {
            *endl = '\0';
        }
        else {
            fprintf(stdout, "[tcp_client_process]: %s may be not full reply\n", connection->reply);
            fflush(stdout);
        }
        fprintf(stdout, "[tcp_client_process]: current reply buffer is %s\n", connection->reply);
        fflush(stdout);
        //working with recieved data, find the end of line
        if (reciever == NULL) {
            fprintf(stdout, "[tcp_client_process]: we are with the null client reciever\n");
            fflush(stdout);
            default_client_reciever(connection, &exit_flag);
        }
        else { //make something with connection->request
            fprintf(stdout, "[tcp_client_process]: we are with the NOT null client reciever\n");
            fflush(stdout);
            ssize_t rc = (ssize_t)reciever(connection, &exit_flag);
            if (rc == -1) {
                //TODO:
                //connection_mark_broken(&srv, connection);
                continue;
            }
        }
        endl++;
        memmove(connection->reply, endl, sizeof(connection->reply) - (endl - connection->reply));
        connection->reply_used -= (endl - connection->reply);
    }
    free(connection);
    close(fd);
    return EXIT_SUCCESS;
}

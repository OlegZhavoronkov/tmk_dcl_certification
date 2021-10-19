#include <stdio.h>
#include <stdlib.h>

#if 1
    #include <tcp/tcp_process.h>
#else
    #include <tcp/client.h>
#endif

int main(int argc, char *argv[]){
#if 1
    if (argc != 3) {
        fprintf(stderr, "Usage: %s host port\n", argv[0]);
        exit(EXIT_FAILURE);
    }
    _tcp_client_process(argv[1], argv[2], 0, 0);
#else
    //TODO: think about command queue (real comfort idea for test system)
    struct client *cli = client_create(NULL); //TODO: think about command queue (real comfort idea for test system)
    tcp_client_process((void *)cli);
    client_destroy(cli);
#endif

    return 0;
}


#include <stdio.h>

#include <processor/dclprocessor.h>
#if 0
#include <processor/config.h>
#endif

int main(int argc, char *argv[]) {
    if (argc < 2) {
        fprintf(stdout, "Usage: %s init_argv\n", argv[0]);
        fprintf(stdout, "Examples:\n");
#if 0
        fprintf(stdout, "\t%s proxy:4444 pipeline:default collector:udp_default:lucid_xxx,flir_yyy\n", argv[0]);
        fprintf(stdout, "\t%s server:4444 pipeline:default collector:tcp_default:camera\n", argv[0]);
#endif
        fflush(stdout);
    }

    struct dclprocessor *dclproc = get_dclprocessor();
    (void)dclproc;

/*
    if (dproc->messenger_type == DP_MSG_PROXY) {
        struct proxy *px = NULL;
        proxy_init(init_argv);
        dproc->messenger = (void*)px;
        dproc->messenger_method = processor_proxy_thread;
        dproc->state_flags |= DPF_MSG_INIT;
    }
    else (dproc->messenger_type == DP_MSG_SERVER) {
        struct server *srv = NULL;
        server_init(init_argv);
        dproc->messenger = (void*)srv;
        dproc->messenger_method = processor_server_thread;
        dproc->state_flags |= DPF_MSG_INIT;
    }

    //if (dproc->state_)
*/

    return 0;
}

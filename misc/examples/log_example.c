#include <misc/log.h>

int main() {
    LOG_FD_WHERE();
    LOG_FD_INFO("This is INFO message\n");
    LOG_FD_ERROR("This is ERROR message\n");
    TRACE_N_CONSOLE(INFO, "This is TRACE for INFO\n");
    TRACE_N_CONSOLE(ERROR, "This is TRACE for ERROR\n");
    return 0;
}
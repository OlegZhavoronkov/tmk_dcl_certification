#ifndef _TRACE_H_
#define _TRACE_H_

#ifdef __cplusplus
extern "C" {
#endif

#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

extern int log_fd;

#define INFO 0
#define WARNING 1
#define ERROR 2

#define DEBUG 1
#define FD_CONSOLE 1

#define LOG_FD_WHERE(...) { \
    if (FD_CONSOLE == 1) { fprintf(stdout, "Place in source:\n\tfile = \"%s\",\n\tline = %d,\n\tfunc = \"%s\"\n", __FILE__, __LINE__, __FUNCTION__); fflush(stdout); } \
    else { if (log_fd == -1) log_fd = open("/tmp/dclprocessor.log", O_CREAT | O_TRUNC | O_RDWR, 0664); \
    if (log_fd != -1) { dprintf(log_fd, "Place in source:\n\tfile = \"%s\",\n\tline = %d,\n\tfunc = \"%s\"\n", __FILE__, __LINE__, __FUNCTION__); } \
    } \
}

#define LOG_FD_INFO(format, ...) if (DEBUG) { \
    if (FD_CONSOLE == 1) { fprintf(stdout, "INFO[%s]: "format, __FUNCTION__, ##__VA_ARGS__); fflush(stdout); } \
    else { if (log_fd == -1) log_fd = open("/tmp/dclprocessor.log", O_CREAT | O_TRUNC | O_RDWR, 0664); \
    if (log_fd != -1) { dprintf(log_fd, "INFO[%s]: "format, __FUNCTION__, ##__VA_ARGS__); } \
    } \
}

#define LOG_FD_ERROR(format, ...) { \
    if (FD_CONSOLE == 1) {fprintf(stderr, "ERROR[%s]: "format, __FUNCTION__,  ##__VA_ARGS__); fflush(stderr); } \
    else { if (log_fd == -1)  log_fd = open("/tmp/dclprocessor.log", O_CREAT | O_TRUNC | O_RDWR, 0664); \
    if (log_fd != -1) dprintf(log_fd, "ERROR[%s]: "format, __FUNCTION__,  ##__VA_ARGS__); \
    } \
}

#define TRACE_N_CONSOLE(sev, ...) { \
    if (sev >= ERROR) { LOG_FD_ERROR(__VA_ARGS__); } \
    else { LOG_FD_INFO(__VA_ARGS__); } \
}

//Usage: TRACE_N_CONSOLE(INFO, "print message\n", args);

#ifdef __cplusplus
}
#endif

#endif //_TRACE_H_


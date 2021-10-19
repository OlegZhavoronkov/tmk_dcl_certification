//------------------------------------------------------------------------------
//  Created     : 29.04.2021
//  Author      : Victor Kats
//  Description : Images capture thread function build around aravis library
//------------------------------------------------------------------------------

#ifndef ARV_CAPTURE_ARV_CAPTURE_THREAD_H
#define ARV_CAPTURE_ARV_CAPTURE_THREAD_H

#ifdef __cplusplus
extern "C" {
#endif

#include <arv.h>

#include <semaphore.h>
#include <pthread.h>

typedef struct {
    int buffer_count;
    long long int camulative_buffer_count;
    guint payload;
} CaptureThreadStatistics;

typedef void (*ArvBufferProcessCallback)	(ArvBuffer *buffer, CaptureThreadStatistics data);
typedef void (*ArvHeartbeatCallback)	(CaptureThreadStatistics data);
typedef void (*ArvFailureCallback)	(const char *message, ArvGvDevice *gv_device, CaptureThreadStatistics data);

struct CaptureThreadData {
    GMainLoop *main_loop;
    ArvCamera *camera;
    ArvStream *stream;
    CaptureThreadStatistics statistics;
    gboolean cancel;
    ArvBufferProcessCallback buffer_process_cb;
    ArvHeartbeatCallback heartbeat_cb;
    ArvFailureCallback failure_cb;
    sem_t data_ready;
    pthread_t thread_id;
};


struct CaptureThreadData *arv_start_capture_thread(ArvCamera *camera
                                , ArvBufferProcessCallback process_frame_cb
                                , ArvHeartbeatCallback heartbeat_cb
                                , ArvFailureCallback failure_cb);

void arv_stop_capture_thread(struct CaptureThreadData *data);

// all of your legacy C code here

#ifdef __cplusplus
}
#endif

#endif //ARV_CAPTURE_ARV_CAPTURE_THREAD_H

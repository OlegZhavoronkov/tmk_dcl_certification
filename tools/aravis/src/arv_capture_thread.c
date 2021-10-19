//------------------------------------------------------------------------------
//  Created     : 29.04.2021
//  Author      : Victor Kats
//  Description : Images capture thread function build around aravis library
//------------------------------------------------------------------------------

#include "arv_capture_thread.h"

#include <stdlib.h>
#include <stdio.h>

static void new_buffer_cb(ArvStream *stream, struct CaptureThreadData *data) {
    ArvBuffer *buffer;

    buffer = arv_stream_try_pop_buffer(stream);
    if (buffer != NULL) {
        if (arv_buffer_get_status(buffer) == ARV_BUFFER_STATUS_SUCCESS) {
            data->statistics.buffer_count++;
            /* Image processing here */
            data->buffer_process_cb(buffer, data->statistics);
        }
        arv_stream_push_buffer(stream, buffer);
    }
}

static gboolean periodic_task_cb(void *abstract_data) {
    struct CaptureThreadData *data = abstract_data;

    data->statistics.camulative_buffer_count += data->statistics.buffer_count;

    if (data->heartbeat_cb) {
        data->heartbeat_cb(data->statistics);
    }

    data->statistics.buffer_count = 0;

    if (data->cancel) {
        g_main_loop_quit(data->main_loop);
        return FALSE;
    }

    return TRUE;
}

static void control_lost_cb(ArvGvDevice *gv_device, void *abstract_data) {
    /* Control of the device is lost. Display a message and force application exit */
    fprintf(stderr, "[arvcapture]: control lost\n");

    struct CaptureThreadData *data = abstract_data;
    if (data->failure_cb) {
        data->failure_cb("Control lost", gv_device, data->statistics);
    }

    data->cancel = TRUE;
}

void *arv_process_capture_thread_inner(void *abstract_data) {
    struct CaptureThreadData *data = abstract_data;

    /* Start the video stream */
    arv_camera_start_acquisition(data->camera, NULL);

    /* Connect the new-buffer signal */
    g_signal_connect (data->stream, "new-buffer", G_CALLBACK(new_buffer_cb), data);
    /* And enable emission of this signal (it's disabled by default for performance reason) */
    arv_stream_set_emit_signals(data->stream, TRUE);

    /* Connect the control-lost signal */
    g_signal_connect (arv_camera_get_device(data->camera), "control-lost",
                      G_CALLBACK(control_lost_cb), data);

    /* Install the callback for frame rate display */
    g_timeout_add_seconds(1, periodic_task_cb, data);

    /* Create a new glib main loop */
    data->main_loop = g_main_loop_new(NULL, FALSE);

    /* Run the main loop */
    g_main_loop_run(data->main_loop);

    g_main_loop_unref(data->main_loop);

    /* Stop the video stream */
    arv_camera_stop_acquisition(data->camera, NULL);

    /* Signal must be inhibited to avoid stream thread running after the last unref */
    arv_stream_set_emit_signals(data->stream, FALSE);

    return NULL;
}

struct CaptureThreadData *arv_start_capture_thread(ArvCamera *camera, ArvBufferProcessCallback process_frame_cb,
                                                   ArvHeartbeatCallback heartbeat_cb, ArvFailureCallback failure_cb) {
    struct CaptureThreadData *data = (struct CaptureThreadData *) malloc(sizeof(struct CaptureThreadData));
    data->statistics.buffer_count = 0;
    data->statistics.camulative_buffer_count = 0;

    if (!ARV_IS_CAMERA(camera)) {
        fprintf(stderr, "[arvcapture]: invalid camera instance\n");
        if (data->failure_cb) {
            data->failure_cb("[arvcapture]: invalid camera instance", NULL, data->statistics);
        }
        free(data);
        return NULL;
    }

    if (sem_init(&(data->data_ready), 1, 0) == -1) {
        fprintf(stderr, "[arvcapture]: can't init object collector semaphore\n");
        free(data);
        return NULL;
    }

    GError *error = NULL;
    int i;

    data->statistics.payload = arv_camera_get_payload(camera, NULL);

    /* Create a new stream object */
    data->stream = arv_camera_create_stream(camera, NULL, NULL, &error);

    if (!ARV_IS_STREAM(data->stream)) {
        fprintf(stderr, "[arvcapture]: can't create stream thread%s%s\n",
                error != NULL ? ": " : "",
                error != NULL ? error->message : "");
        if (failure_cb) {
            failure_cb("[arvcapture]: can't create stream thread", NULL, data->statistics);
        }
        g_clear_error(&error);
        free(data);
        return NULL;
    }

    data->camera = camera;
    data->buffer_process_cb = process_frame_cb;
    data->heartbeat_cb = heartbeat_cb;
    data->failure_cb = failure_cb;
    data->cancel = FALSE;

    /* Push 50 buffer in the stream input buffer queue */
    for (i = 0; i < 50; i++)
        arv_stream_push_buffer(data->stream, arv_buffer_new(data->statistics.payload, NULL));

    pthread_create(&data->thread_id, NULL, arv_process_capture_thread_inner, data);

    return data;
}

void arv_stop_capture_thread(struct CaptureThreadData *data) {
    data->cancel = TRUE;
    pthread_join(data->thread_id, NULL);
    g_object_unref (data->stream);
}


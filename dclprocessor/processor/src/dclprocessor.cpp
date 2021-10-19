#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include <opencv2/core.hpp>
#include <opencv2/highgui.hpp>

#include <storage/storage_interface.h>
#include <misc/name_generator.h>

#include <misc/collector.h>
#include <misc/dclprocessor_old.h>

#include <pipeline/pipeline.h>
#include <dclfilters/cvfilter.h>

#define MINIMAL_BOOST_SIZE 2000000000 //58048974720

struct dclprocessor_old {
    char boost_path[256];
    void (*exec_object) (ObjectDescriptor &obj, char *, char *);
    struct data_collector *collector;
};

cv::Mat restore_image(struct source_description *sdesc, void *frame_data){
    size_t rows = sdesc->height;
    size_t cols = sdesc->width;
    int channels = sdesc->channels;
    int cv_const = CV_8UC4;
    switch (channels) {
        case 1: 
            cv_const = CV_8UC1;
            break;
        case 3:
            cv_const = CV_8UC3;
            break;
        default:
            cv_const = CV_8UC4;
        }
        cv::Mat img(rows, cols, cv_const, frame_data);
    return img;
}

//This callback could be used only when 
int object_detection_callback(void *internal_data, int source_id, int frame_id, void *frame_data) {
    struct dclprocessor *dclproc = (struct dclprocessor*)internal_data;
    struct data_collector *collector = dclproc->collector;
    if (collector) {
        struct source_description *sdesc = (collector->sources[source_id])->sdesc;
        cv::Mat img = restore_image(sdesc, frame_data);
        float radius = findRadius(img);
        fprintf(stdout, "[object_detection]: Frame with frame_id = %d has radius = %f\r", frame_id, radius);
        fflush(stdout);
//-----------------------------------------------------------------------
// This part is actual only for pencil_frames_5 in one source (try this idea)
//-----------------------------------------------------------------------
        static int frames_obj_cnt = 0;
        static int frames_empty_cnt = 0;
        static int frames_non_cnt = 0;
        static int new_object = 0;

        static int framesets_cnt = 0;
        static int frames_cnt = 0;

        static int id = 0;
        //not smart algotithm of object detection
        //fprintf(stdout, "[tcp_capture_data_process]: frames_obj_cnt = %d frames_empty_cnt %d frames_non_cnt %d\n", frames_obj_cnt, frames_empty_cnt, frames_non_cnt);
        if (radius < 135 && new_object == 0) {
            frames_empty_cnt++;
            if (frames_obj_cnt >= 16 && frames_empty_cnt >= 8) {
                new_object = 1;
                fprintf(stdout, "[source_data_thread]: object break is detected after %d frames\n", frames_empty_cnt + frames_obj_cnt + frames_non_cnt);
                fflush(stdout);
    //------------------------------------------------------------------------------
                //TODO: print RESUME about collected object
                //TODO: for debug check .valid() of first and random frame, may be save it 
                //TODO: check time for object collect
    //------------------------------------------------------------------------------
                //finalize current full object and request new if result is 1
                new_object = 0;
                frames_obj_cnt = 0;
                frames_non_cnt = 0;
                frames_empty_cnt = 0;
                return 1;
            }
        }
        else if (radius >= 150){
            frames_obj_cnt++;
            frames_obj_cnt += frames_empty_cnt;
            frames_empty_cnt = 0;
            frames_obj_cnt += frames_non_cnt;
            frames_non_cnt = 0;
        }
        else {
            frames_non_cnt++;
        }
    }
    return 0;
}

void add_frame_callback(void *internal_data, int source_id, int frame_id, void *frame_data) {
    struct dclprocessor *dclproc = (struct dclprocessor*)internal_data;
    struct data_collector *collector = dclproc->collector;
    if (collector) {
        struct source_description *sdesc = (collector->sources[source_id])->sdesc;
        cv::Mat img = restore_image(sdesc, frame_data);
        size_t available = storage_interface_add_frame_to_object_with_framesets(collector->boost_path, (collector->sources[source_id])->data, source_id, frame_id, (void*)&img);
        //fprintf(stdout, "[add_frame_callback]: %zd of shared segment available\n", available);
        //fflush(stdout);
    }
}

void save_frame_callback(void *internal_data, int source_id, int frame_id, void *frame_data) {
    struct dclprocessor *dclproc = (struct dclprocessor*)internal_data;
    struct data_collector *collector = dclproc->collector;
    //fprintf(stdout, "[save_frame_callback]: collector=%llx\n", collector);
    //fflush(stdout);
    if (collector) {
        struct source_description *sdesc = (collector->sources[source_id])->sdesc;
        char *output_path = sdesc->output_path;
        cv::Mat img = restore_image(sdesc, frame_data);
        char postfix[20];
        char name[256];
        generate_dt_postfix(postfix);
        sprintf(name, "%s/frame_%d_%s.bmp", output_path, source_id, postfix);
        cv::imwrite(name, img);
    }
}

void first_action(void *internal_data, void *data, int *status) {
    struct dclprocessor *dclproc = (struct dclprocessor*)internal_data;
    void *obj_ptr;
    char obj_name[256];
    //fprintf(stdout, "[first_action]: previous object %s\n", obj_name);
    //fflush(stdout);
    struct data_collector *collector = dclproc->collector;
    if (collector) {
        if (collector->exit_flag) {
            sem_post(&(collector->empty_ready));
            *status = 0;
            return;
        }
        //fprintf(stdout, "[first_action]: I'm  in first action before wait(collector:data)\n");
        //fflush(stdout);
        sem_wait(&(collector->data_ready));
        static int data_id = 0;
        //fprintf(stdout, "[first_action]: Object %d with name %s ready for sent to pipeline\n", data_id++, collector->data);
        //fflush(stdout);
        strcpy((char*)data, collector->data);
        generate_new_object_name(obj_name);
        if (collector->use_boost_shmem) {
            size_t available = storage_interface_new_object_with_framesets(collector->boost_path, obj_name, collector->sources_cnt);
        }
    }
    else {
        //fprintf(stdout, "[first_action]: I'm  in first action without collector\n");
        //fflush(stdout);
        generate_new_object_name(obj_name);
        size_t available = storage_interface_new_object(dclproc->boost_path, obj_name);
        sleep(2);
    }
    fprintf(stdout, "[first_action]: %s created\n", obj_name);
    fflush(stdout);
    //Try new object
    storage_interface_get_object(&obj_ptr, dclproc->boost_path, obj_name);
    ObjectDescriptor &obj = *(ObjectDescriptor*)obj_ptr;
    //Apply callback to new object -- normally to do nothing
    dclproc->exec_object(obj,dclproc->boost_path, obj_name);
    if (collector) {
        //fprintf(stdout, "[first_action]: Set new generated name %s to collector->data\n", obj_name);
        //fflush(stdout);
        strcpy(collector->data, obj_name);
        //fprintf(stdout, "[first_action]: I'm  in first action before post(collector:empty)\n");
        //fflush(stdout);
        sem_post(&(collector->empty_ready));
        //fprintf(stdout, "[first_action]: I'm  in first action after post(collector:empty)\n");
        //fflush(stdout);
    }
    *status = 0;
}

void middle_action(void *internal_data, void *data, int *status) {
    struct dclprocessor *dclproc = (struct dclprocessor*)internal_data;
    void *obj_ptr;
    char *obj_name = (char *)data;
    fprintf(stdout, "[middle_action]: %s\n", obj_name);
    fflush(stdout);
    // It should be very strange if the middle module do nothing
    storage_interface_get_object(&obj_ptr, dclproc->boost_path, obj_name);
    ObjectDescriptor &obj = *(ObjectDescriptor*)obj_ptr;
    dclproc->exec_object(obj,dclproc->boost_path, obj_name);
    storage_interface_analyze_object_with_framesets(dclproc->boost_path, obj_name);
    *status = 0;
}

void last_action(void *internal_data, void *data, int *status) {
    struct dclprocessor *dclproc = (struct dclprocessor*)internal_data;
    void *obj_ptr;
    char *obj_name = (char *)data;
    static int obj_id = 0;
    fprintf(stdout, "[last_action, #%d]: %s\n", obj_id++, obj_name);
    fflush(stdout);
    // It should be very strange if the middle module do nothing
    storage_interface_get_object(&obj_ptr, dclproc->boost_path, obj_name);
    ObjectDescriptor &obj = *(ObjectDescriptor*)obj_ptr;
    dclproc->exec_object(obj,dclproc->boost_path, obj_name);
    storage_interface_free_object(dclproc->boost_path, obj_name);
    *status = 0;
}

void connect_to_pipeline(enum PC_TYPE module_type, void *collector_ptr, void (*exec_object) (ObjectDescriptor &obj, char *, char *)) {
    struct dclprocessor dclproc;
    struct data_collector *collector = (struct data_collector *)collector_ptr;
    if (collector && collector->use_boost_shmem) {
        strcpy(dclproc.boost_path, collector->boost_path);
    }
    else {
        //TODO: this is result of migration, test way
        strcpy(dclproc.boost_path, "boostsegment");
    }
    dclproc.exec_object = exec_object;
    dclproc.collector = collector;
    switch (module_type) {
        case PC_FIRST:
            if (collector) {
                for (int i = 0; i < collector->sources_cnt; i++) {
                    (collector->sources[i])->internal_data = &dclproc;
                    (collector->sources[i])->object_detection_callback = NULL;
                    //TODO: should work only for one source now
                    //(collector->sources[i])->object_detection_callback = object_detection_callback;
                    (collector->sources[i])->add_frame_callback = add_frame_callback;
                    //(collector->sources[i])->save_frame_callback = NULL;
                    (collector->sources[i])->save_frame_callback = save_frame_callback;
                }
                generate_new_object_name(collector->data);
                if (collector->use_boost_shmem) {
                    //if we want make new version of boost shmem for new start of first module
                    storage_interface_destroy(collector->boost_path);
                    size_t realsize = storage_interface_init(collector->boost_path, collector->boost_size);
                    if (realsize != collector->boost_size) {
                        //TODO: this difference always -- what the reason? (may be allocators or some else ...)
                        fprintf(stderr, "[pipeline_init]: asked about %zd, but real allocated %zd bytes\n", collector->boost_size, realsize);
                        fflush(stderr);
                    }
                    else {
                        fprintf(stdout, "[pipeline_init]: real allocated %zd bytes\n", realsize);
                        fflush(stdout);
                    }
                    size_t available = storage_interface_new_object_with_framesets(collector->boost_path, collector->data, collector->sources_cnt);
                }
            }
            else {
                storage_interface_destroy(dclproc.boost_path);
                //TODO MINIMAL_BOOST_SIZE config
                storage_interface_init(dclproc.boost_path, MINIMAL_BOOST_SIZE);
            }
            pipeline_process((void *)(&dclproc), first_action);
            break;
        case PC_MIDDLE:
            pipeline_process((void *)(&dclproc), middle_action);
            break;
        case PC_LAST:
            pipeline_process((void *)(&dclproc), last_action);
            if (collector) {
                fprintf(stdout, "[connect_to_pipeline, PC_LAST]: collector->exit_flag = %d\n", collector->exit_flag);
                fflush(stdout);
            }
            fprintf(stderr, "[connect_to_pipeline, PC_LAST]: please, wait 5s before boost memory will destroyed...\n");
            fflush(stderr);
            sleep(5);
            if (collector && collector->use_boost_shmem) {
                storage_interface_free_object(collector->boost_path, collector->data);
                storage_interface_destroy(collector->boost_path);
            }
            else {
                storage_interface_destroy(dclproc.boost_path);
            }
            break;
        default:
            fprintf(stderr, "[connect_to_pipeline]: couldn't connect to pipeline with unknown type\n");
            fflush(stderr);
    }
    if (collector) collector->exit_flag = 1;
    fprintf(stderr, "[connect_to_pipeline]: please, wait 5s while pipeline will finished...\n");
    fflush(stderr);
    sleep(5);
}


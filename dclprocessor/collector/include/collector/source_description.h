#ifndef _SOURCE_DESCRIPTION_H_
#define _SOURCE_DESCRIPTION_H_

#ifdef __cplusplus
extern "C" {
#endif

enum source_type {
    STYPE_UNKNOWN = -1,
    STYPE_FILE = 0,     //file
    STYPE_UDP = 1,      //udp
    STYPE_TCP = 2       //tcp
};

struct source_description {
    enum source_type type;
    //identification string
    char sid_string[256];
    //connection information 
    char addr[16];
    char port[8];
    //packet information
    size_t header_len;
    size_t data_len; //optional information, it can be reading from the control stream or from the image header
    //image information
    //all optional, it can be readed from the header or frame header
    size_t width;
    size_t height;
    size_t channels; // 1 for CV_8UC1 and 3 for CV_8UC3 (cv::Mat constructor parameter)
    //debug_data_save
    char output_path[1024];
};

void create_gige_source_description(char *interface, struct source_description **p_sdesc);
void create_tcp_source_description(char *host, char *port, struct source_description **p_sdesc);
void source_description_destroy(struct source_description **p_sdesc);
void out_source_description(struct source_description *sdesc);

#ifdef __cplusplus
}
#endif

#endif //_SOURCE_DESCRIPTION_H_

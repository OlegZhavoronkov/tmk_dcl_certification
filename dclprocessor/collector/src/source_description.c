#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <collector/source_description.h>

void create_tcp_source_description(char *host, char *port, struct source_description **p_sdesc) {
    struct source_description *sdesc = *p_sdesc;
    if (!sdesc) sdesc = (struct source_description *)malloc(sizeof(struct source_description));
    sdesc->type = STYPE_TCP;
    sdesc->sid_string[0] = '\0';
    strcpy(sdesc->addr, host);
    strcpy(sdesc->port, port);
    sdesc->header_len = sizeof(size_t) + sizeof(int);
    sdesc->width = 960;
    sdesc->height = 1920;
    sdesc->channels = 3; //CV_8UC3
    sdesc->data_len = sdesc->width * sdesc->height * sdesc->channels;
    strcpy(sdesc->output_path, "/tmp/tmk/data");
    //strcpy(sdesc->output_path, ""); //for disable debug save
    *p_sdesc = sdesc;
}

void create_gige_source_description(char *interface, struct source_description **p_sdesc) {
    struct source_description *sdesc = *p_sdesc;
    if (!sdesc) sdesc = (struct source_description *)malloc(sizeof(struct source_description));
    sdesc->type = STYPE_UDP;
    strcpy(sdesc->sid_string, interface);
    sdesc->width = 2448;
    sdesc->height = 2048;
    //sdesc->width = 1920;
    //sdesc->height = 1200;
    sdesc->channels = 1; //CV_8UC1
    sdesc->data_len = sdesc->width * sdesc->height * sdesc->channels;
    strcpy(sdesc->output_path, "/tmp/tmk/data");
    //strcpy(sdesc->output_path, ""); //for disable debug save
    *p_sdesc = sdesc;
}


void out_source_description(struct source_description *sdesc) {
    if (sdesc->sid_string[0] == '\0') {
        fprintf(stdout, "\tSource description for ip=%s port=%s [type=%d]:\n", sdesc->addr, sdesc->port, sdesc->type);
        fprintf(stdout, "\t\theader_len = %zd\n", sdesc->header_len);
    }
    else {
        fprintf(stdout, "\tSource description for %s [type=%d]:\n", sdesc->sid_string, sdesc->type);
    }
    fprintf(stdout, "\t\twidth = %zd height = %zd channels = %ld\n", sdesc->width, sdesc->height, sdesc->channels);
    fprintf(stdout, "\t\toutput_path = %s\n", sdesc->output_path);
    fflush(stdout);
}

void source_description_destroy(struct source_description **p_sdesc){
    if (!p_sdesc || *p_sdesc == NULL) return;
    free(*p_sdesc); 
    *p_sdesc = NULL;
}



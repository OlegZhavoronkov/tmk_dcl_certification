//------------------------------------------------------------------------------
//  Created     : 12.05.2021
//  Author      : Victor Kats
//  Description : Demo of camera
//------------------------------------------------------------------------------

#include "CameraMetaData.h"

#include "stdio.h"
#include "stdlib.h"

int main(int argc, char *argv[]) {
    if(argc != 2) {
        fprintf(stderr,"Usage: %s <metadata_source>,\n"
               "\there <metadata_source> is path to metadata JSON file or this JSON itself\n", argv[0]);
        return EXIT_FAILURE;
    }
    char *words = getCameraCommandString(argv[1]);
    if(words) {
        printf("%s\n", words);
        free(words);
    }
    return EXIT_SUCCESS;
}


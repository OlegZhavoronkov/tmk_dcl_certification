#include <stdio.h>
#include <stdlib.h>
#include <dclfilters/cvtools.h>

int main(int argc, char *argv[]) {
    if (argc < 6) {
        printf("Usage:\n");
        printf("\t%s in.raw [width] [height] [channels] out.bmp\n", argv[0]);
        printf("Example:\n");
        printf("\t%s 1.raw 2048 2448 1 1.bmp", argv[0]);
        return 0;
    }
    convert_from_rawfile(argv[1], atoi(argv[2]), atoi(argv[3]), atoi(argv[4]), argv[5]);
    return 0;
}


#include <stdio.h>
#include <stdlib.h>

#include <sys/stat.h>
#include <sys/types.h>
#include <fcntl.h>
#include <unistd.h>

#include <dclfilters/cvfilter.h>

int main(int argc, char *argv[]) {
    if (argc < 6) {
        printf("Usage:\n");
        printf("\t%s in.raw [width] [height] [channels]\n", argv[0]);
        printf("Example:\n");
        printf("\t%s 1.raw 2048 2448 1\n", argv[0]);
        return 0;
    }

    struct stat st;
    stat(argv[0], &st);
    char *frame_data = (char*)malloc(st.st_size);
    int in_fd = open(argv[0], O_RDONLY, 0600);
    if (in_fd > 0) {
        read(in_fd, frame_data, st.st_size);
        close(in_fd);
    }

    float r = find_radius(atoi(argv[1]), atoi(argv[2]), atoi(argv[3]), (void *)frame_data);
    printf("%lf\n", r);
    return 0;
}


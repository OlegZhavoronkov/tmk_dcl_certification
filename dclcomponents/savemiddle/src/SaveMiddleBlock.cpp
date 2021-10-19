#include "savemiddle.h"

#include "dclprocessor/dclprocessor.h"

// Won't work without server!

int main(int argc, char *argv[]) {
    if (argc >= 2) {
        saveMiddleConfig(argv[1]);
        connect_to_pipeline(PC_MIDDLE, NULL, SaveBlock);
    }
    return 0;
}

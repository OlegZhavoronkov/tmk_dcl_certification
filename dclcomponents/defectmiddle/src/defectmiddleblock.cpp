#include "DefectMiddle.h"

#include "dclprocessor/dclprocessor.h"

// Won't work without pipeline!

int main() { //int argc, char *argv[]){
    connect_to_pipeline(PC_MIDDLE, NULL, DefectModel);
    return 0;
}

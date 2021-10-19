#ifndef _DCLPROCESSOR_OLD_H_
#define _DCLPROCESSOR_OLD_H_

#include <storage/ObjectDescriptor.h>

using namespace tmk;
using namespace tmk::storage;

enum PC_TYPE {
    PC_UNDEFINED = -1,
    PC_FIRST = 1,
    PC_MIDDLE = 2,
    PC_LAST = 0
};

typedef void (*exec_object_t) (ObjectDescriptor &obj, char *, char *);
void connect_to_pipeline(enum PC_TYPE module_type, void *collector, void (*exec_object) (ObjectDescriptor &obj, char *, char *));

#endif // _DCLPROCESSOR_OLD_H_


#include <storage/IpcTypes.h>

// converting ipc_string in std::string
std::string tmk::ipc::convertString_IPC_toSTD(const tmk::ipc::ipc_string& ipcString) {
    return std::string(ipcString.begin(), ipcString.end());
};

// converting std::string in ipc_string
tmk::ipc::ipc_string tmk::ipc::convertString_STD_toIPC(
                                                        const std::string& stdString
                                                      , const tmk::ipc::void_allocator& alloc
                                                      ) {
    return tmk::ipc::ipc_string(  std::begin(stdString), std::end(stdString), alloc);
};



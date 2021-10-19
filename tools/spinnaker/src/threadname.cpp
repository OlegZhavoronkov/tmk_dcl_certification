//------------------------------------------------------------------------------
//  Created     : 06.08.21
//  Author      : Victor Kats
//  Description : Import Spinnnaker-based capturer into project (see TMKDCL-380)
//------------------------------------------------------------------------------

#include "threadname.h"
namespace spintools {
void setThreadName(std::thread *thread, std::string threadName) {
#ifdef WIN32
#else
  auto handle = thread->native_handle();
  pthread_setname_np(handle, threadName.c_str());
#endif
}

std::string getThreadName() {
#ifdef WIN32
  return "";
#else
  auto handle = pthread_self();
  char buf[64];
  pthread_getname_np(handle, buf, sizeof(buf));
  return buf;
#endif
}
}

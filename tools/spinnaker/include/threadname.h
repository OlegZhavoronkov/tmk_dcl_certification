//------------------------------------------------------------------------------
//  Created     : 06.08.21
//  Author      : Victor Kats
//  Description : Import Spinnnaker-based capturer into project (see TMKDCL-380)
//------------------------------------------------------------------------------

#ifndef THREADNAME_H
#define THREADNAME_H

#include <thread>
#include <string>
namespace spintools {
void setThreadName(std::thread *thread, std::string threadName);
std::string getThreadName();
}
#endif // THREADNAME_H

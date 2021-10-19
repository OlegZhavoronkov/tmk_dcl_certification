#pragma once

#ifndef STORAGE_IPC_TYPES_H
#define STORAGE_IPC_TYPES_H

#include <boost/interprocess/allocators/allocator.hpp>
#include <boost/interprocess/containers/map.hpp>
#include <boost/interprocess/containers/string.hpp>
#include <boost/interprocess/containers/vector.hpp>
#include <boost/interprocess/managed_shared_memory.hpp>

namespace tmk::ipc {

//======= base defenition ======================================================

using segment_manager = boost::interprocess::managed_shared_memory::segment_manager;
using void_allocator  = boost::interprocess::allocator<void, segment_manager>;
using char_allocator  = boost::interprocess::allocator<char, segment_manager>;
using uchar_allocator = boost::interprocess::allocator<uint8_t, segment_manager>;
using buffer          = boost::interprocess::vector<uint8_t, uchar_allocator>;

// === for definition ipc_string ===
using ipc_string = boost::interprocess::basic_string<char, std::char_traits<char>, char_allocator>;

// converting ipc_string in std::string
std::string convertString_IPC_toSTD(const ipc_string& ipcString);

// converting std::string in ipc_string
ipc_string convertString_STD_toIPC( 
                                      const std::string& stdString
                                    , const void_allocator& alloc
                                  );
}

#endif // STORAGE_IPC_TYPES_H
//------------------------------------------------------------------------------
//  Created     : 26.04.2021
//  Author      : Vadim Yarmushov ...
//  Description : Defect.h -the structure describes the defects on the object:
//                pipe, shell, mandrel
//
//------------------------------------------------------------------------------

#pragma once

#ifndef STORAGE_DEFECT_H
#define STORAGE_DEFECT_H

#include <storage/Ipc.h>
#include <storage/IpcTypes.h>  // types for allocators for shared memory
#include <storage/Time.h>      // for TimeT

#include <string>

namespace tmk::storage {

  // base class for Defects
  class Defect {

    public:

    Defect(const ipc::void_allocator &shmemAlloc);

    Defect(
             const ipc::void_allocator &shmemAlloc
           , const int    t_defect_id
           , const double t_position
           , const double t_size
           , const double t_depth
           , const double t_precision
           , const TimeT  t_last_detection
           , const tmk::ipc::ipc_string t_image_url
          );

    // debug show info about Defect
    void show() const;

    // ===  setters ===
    void set_defect_id(const int& t_defect_id)      {m_defect_id = t_defect_id;}
    void set_position(const double& position)       {m_position = position;    }
    void set_size(const double& t_size)             {m_size = t_size;          }
    void set_depth(const double& depth)             {m_depth = depth;          }
    void set_precision (const double t_precision)   {m_precision = t_precision;}
    void set_last_detection(const TimeT& t_last_detection ) {m_last_detection = t_last_detection;}
    void set_image_url(std::string& image_url, ipc::void_allocator &shmemAlloc ) 
         {m_image_url =  tmk::ipc::convertString_STD_toIPC(image_url, shmemAlloc);}
    // ==================  getters ===

    int    get_defect_id() const {return m_defect_id;}
    double get_position()  const {return m_position; }
    double get_size()      const {return m_size;     }
    double get_depth()     const {return m_depth;    }
    double get_precision() const {return m_precision;}
    TimeT  get_last_detection() const {return m_last_detection;}
    const tmk::ipc::ipc_string& get_image_url() const {return m_image_url;}

    //===================

    protected:

      int    m_defect_id;                // number id  of defect which have description in pipes_defects_dictionary and in mandrels_defects_dictionary
      double m_position;                 // position of defect (distance from the beginning of the pipe, mandrel)
      double m_size;                     // size of defect
      double m_depth;                    // depth_of defect
      double m_precision;
      TimeT  m_last_detection;           // timestamp  2021-08-31 08:59:54.148 (not null) time when detected 
      tmk::ipc::ipc_string m_image_url;  // full path to the defect image

  };  // end class Defect

}  // end namespace tmk::storage

// === for definition vector < class Defect> ===
using defect_allocator  = boost::interprocess::allocator<tmk::storage::Defect, tmk::ipc::segment_manager>;
using ipc_vector_Defect = boost::interprocess::vector<tmk::storage::Defect, defect_allocator>;

#endif // STORAGE_DEFECT_H
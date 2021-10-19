#pragma once

//------------------------------------------------------------------------------
//  Created     : 10.08.2021
//  Author      : Vadim Yarmushov ...
//  ver         : 0.0.1
//  Description : ObjectData.h - define struct for result of processing ObjectData
//                this data will save in DB
//
//------------------------------------------------------------------------------

#ifndef STORAGE_OBJECTDATA_H
#define STORAGE_OBJECTDATA_H

#include <storage/Defect.h>

#include <storage/IObject.h>
#include <storage/Time.h>

#include <opencv2/core/types.hpp>
#include <vector>

namespace tmk::storage {

  struct ObjectDataID : public ObjectID {
    using ObjectID::ObjectID;
  };

  // ObjectData
  class ObjectData : public IObject<ObjectDataID> {

    public:

    ~ObjectData(){std::cout << "[ObjectData]: work destructor  ObjectData" << std::endl;}
    // Do NOT use this constructor standalone!
    // use ipc::createShared() or use ObjectDataSet::addObjectData() instead

    // shmemAlloc - memory allocator <void, segment_manager>;
    // name - name of ObjectData (example "name_ObjectData01")
    // storageType_ - time of existing NO_SAVE, SHORT_TERM, LONG_TERM 

 // constructor for mandrel, pipe, billet (args:12)
    ObjectData (
                 const tmk::ipc::void_allocator& shmemAlloc
               , const std::string& name_
               , const StorageType  storageType_
               , const bool         t_condition
               , const tmk::TimeT   t_last_detection
               , const double t_diameter
               , const double t_length
               , const double t_diameter1
               , const double t_diameter2
               , const double t_diameter3
               , const tmk::ipc::ipc_string& t_number
               , const ipc_vector_Defect& temp_vec_Defects
               );

  // empty constructor for mandrel, pipe, billet (args:3) have only memory allocator
    ObjectData(
                 const tmk::ipc::void_allocator& shmemAlloc
               , const std::string& name_
               , StorageType storageType_
             );

// constructor for billet
    ObjectData(
                 const ipc::void_allocator& shmemAlloc
               , const std::string& name_
               , const StorageType storageType_
               , const bool t_condition
               , const TimeT  t_last_detection
               , const double t_length
               , const double t_diameter1
               , const double t_diameter2
               , const double t_diameter3
              );

// constructor for mandrel
    ObjectData(
                 const ipc::void_allocator& shmemAlloc
               , const std::string& name_
               , const StorageType storageType_
               , const bool t_condition
               , const TimeT  t_last_detection
               , const double t_diameter
               , const ipc::ipc_string& t_number
               , const ipc_vector_Defect& temp_vec_Defects
              );

// constructor for pipe
    ObjectData(
                 const ipc::void_allocator& shmemAlloc
               , const std::string& name_
               , const StorageType storageType_
               , const bool t_condition
               , const TimeT  t_last_detection
               , const double t_diameter
               , const ipc_vector_Defect& temp_vec_Defects
                    );

    // show info in FrameData
    void show();

  // =============== getter =================
    bool   get_condition() const  {return m_condition;}
    tmk::TimeT get_last_detection() const {return m_last_detection;}
    double get_diameter () const  {return m_diameter; }

    double get_length   () const  {return m_length;   }
    double get_diameter1() const  {return m_diameter1;}
    double get_diameter2() const  {return m_diameter2;}
    double get_diameter3() const  {return m_diameter3;}
    const tmk::ipc::ipc_string&  get_number() const {return m_number;}
    const ipc_vector_Defect&  get_vec_Defect() const {return m_vec_Defects; }

    //================ setter =================
    void set_condition(const  bool   t_condition)   {m_condition = t_condition;}
    void set_last_detection( const tmk::TimeT t_last_detection) {m_last_detection = t_last_detection;}
    void set_diameter( const  double t_diameter )   {m_diameter  = t_diameter; }

    void set_length(   const  double t_length   )   {m_length    = t_length;   }
    void set_diameter1(const  double t_diameter1)   {m_diameter1 = t_diameter1;}
    void set_diameter2(const  double t_diameter2)   {m_diameter2 = t_diameter2;}
    void set_diameter3(const  double t_diameter3)   {m_diameter3 = t_diameter3;}

    void set_number(const std::string& t_number, const tmk::ipc::void_allocator& alloc )  {m_number = tmk::ipc::convertString_STD_toIPC(t_number, alloc);}
    void set_number(const tmk::ipc::ipc_string t_number) {m_number = t_number;}

    void set_vec_Defect( const ipc_vector_Defect& t_vec_Defects) {m_vec_Defects = t_vec_Defects;}
    void WhereIam() const {std::cout << "[ObjectData]: i'm in object: FrameData\n";}

  //==================== properties ==================
  private:
  bool   m_condition;                        // property: billet, pipe, mandrel
  tmk::TimeT  m_last_detection;              // property: billet, pipe, mandrel
  double m_diameter;                         // property: pipe, mandrels
  double m_length;                           // property: billet 
  double m_diameter1;                        // property: billet
  double m_diameter2;                        // property: billet
  double m_diameter3;                        // property: billet

  tmk::ipc::ipc_string m_number;             // property: mandrel - (std::strig) -  recognized number of mandrel, номер оправки, отсканированный и распознанный в middle block  
  ipc_vector_Defect m_vec_Defects; // property: mandrel, pipe             list of found defects

  };  // end class ObjectData


} // end namespace tmk::storage

#endif // STORAGE_OBJECTDATA_H

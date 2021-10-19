#include <storage/ObjectData.h>
//#include <glog/logging.h>

//#include <chrono>
#include <iomanip>
#include <iostream>

namespace tmk::storage {

ObjectData::ObjectData(
                       const tmk::ipc::void_allocator& shmemAlloc
                     , const std::string& name_
                     , const StorageType storageType_
                     , const bool   t_condition
                     , const TimeT  t_last_detection
                     , const double t_diameter
                     , const double t_length
                     , const double t_diameter1
                     , const double t_diameter2
                     , const double t_diameter3
                     , const tmk::ipc::ipc_string& t_number
                     , const ipc_vector_Defect& temp_vec_Defects
                    ) :
                                            IObject(name_, storageType_)
                                          , m_condition(t_condition)
                                          , m_last_detection(t_last_detection)
                                          , m_diameter(t_diameter)
                                          , m_length(t_length)
                                          , m_diameter1(t_diameter1)
                                          , m_diameter2(t_diameter2)
                                          , m_diameter3(t_diameter3)
                                          , m_number(t_number, shmemAlloc)
                                          , m_vec_Defects(temp_vec_Defects, shmemAlloc)
                                          {
                                            LOG(INFO) <<  "\nconstructor ObjectData(N... params)";
                                          }

ObjectData::ObjectData(
                       const ipc::void_allocator& shmemAlloc
                     , const std::string& name_
                     , StorageType storageType_
                    )
                    :  IObject(name_, storageType_)
                     , m_number(shmemAlloc)
                     , m_vec_Defects(shmemAlloc)
                    {LOG(INFO) <<  "\nconstructor ObjectData_base(3 params)";}

// constructor for billet
ObjectData::ObjectData(
                       const ipc::void_allocator& shmemAlloc
                     , const std::string& name_
                     , const StorageType storageType_
                     , const bool t_condition
                     , const TimeT  t_last_detection
                     , const double t_length
                     , const double t_diameter1
                     , const double t_diameter2
                     , const double t_diameter3
                    ) :
                                            IObject(name_, storageType_)
                                          , m_condition(t_condition)
                                          , m_last_detection(t_last_detection)
                                          , m_length(t_length)
                                          , m_diameter1(t_diameter1)
                                          , m_diameter2(t_diameter2)
                                          , m_diameter3(t_diameter3)
                                          , m_number(shmemAlloc)
                                          , m_vec_Defects(shmemAlloc)
                                          {
                                            LOG(INFO) <<  "\nconstructor ObjectData for BILLET(N... params)";
                                          }
// constructor for mandrel
ObjectData::ObjectData(
                       const ipc::void_allocator& shmemAlloc
                     , const std::string& name_
                     , const StorageType storageType_
                     , const bool t_condition
                     , const TimeT  t_last_detection
                     , const double t_diameter
                     , const ipc::ipc_string& t_number
                     , const ipc_vector_Defect& temp_vec_Defects
                    ) :
                                            IObject(name_, storageType_)
                                          , m_condition(t_condition)
                                          , m_last_detection(t_last_detection)
                                          , m_diameter(t_diameter)
                                          , m_number(t_number, shmemAlloc)
                                          , m_vec_Defects(temp_vec_Defects, shmemAlloc)
                                          {
                                            LOG(INFO) <<  "\nconstructor ObjectData for MANDREL(N... params)";
                                          }

// constructor for pipe
ObjectData::ObjectData(
                       const ipc::void_allocator& shmemAlloc
                     , const std::string& name_
                     , const StorageType storageType_
                     , const bool t_condition
                     , const TimeT  t_last_detection
                     , const double t_diameter
                     , const ipc_vector_Defect& temp_vec_Defects
                    ) :
                                            IObject(name_, storageType_)
                                          , m_condition(t_condition)
                                          , m_last_detection(t_last_detection)
                                          , m_diameter(t_diameter)
                                          , m_number(shmemAlloc)
                                          , m_vec_Defects(temp_vec_Defects, shmemAlloc)
                                          {
                                            LOG(INFO) <<  "\nconstructor ObjectData for PIPE (N... params)";
                                          }

// show info in ObjectData
void ObjectData::show() {
    LOG(INFO) << "\nm_condition:          " << m_condition;
    //LOG(INFO) << "\nm_last_detection:     " << tmk::service_func::getFullTimeString(m_last_detection);
    LOG(INFO) << "\nm_diameter:           " << m_diameter;
    LOG(INFO) << "\nm_length:             " << m_length;
    LOG(INFO) << "\nm_diameter1:          " << m_diameter1;
    LOG(INFO) << "\nm_diameter2:          " << m_diameter2;
    LOG(INFO) << "\nm_diameter3:          " << m_diameter3;
   // std::cout << "\nm_number:             " << tmk::ipc::convertString_IPC_toSTD(m_number);

    /*
    int size_vec = m_vec_Defects.size();
    size_t count = 0;
    if (size_vec > 0)
    {
        std::cout << "\nDefects::             ";
        for (const auto& item: m_vec_Defects)
        {
           std::cout << "\nitem [ " << count << " ] \n";
           item.show();
           std::cout << "\n";
           ++count;
        }

    }
    */
    std::cout << std::endl;

}

} // namespace tmk::storage

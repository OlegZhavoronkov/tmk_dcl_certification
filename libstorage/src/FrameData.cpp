#include <storage/FrameData.h>
#include <storage/FrameDataSet.h>

#include <glog/logging.h>

#include <chrono>
#include <iomanip>
#include <iostream>

namespace tmk::storage {

// general constructor (12 args)
FrameData::FrameData(
                       const ipc::void_allocator &shmemAlloc
                     , const std::string &name_
                     , const StorageType storageType_
                     , const bool t_condition
                     , const TimeT  t_last_detection
                     , const double t_diameter
                     , const double t_length
                     , const double t_diameter1
                     , const double t_diameter2
                     , const double t_diameter3
                     , const ipc::ipc_string& t_number
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
                                          , m_shmemAlloc(shmemAlloc) 
                                          { LOG(INFO) << "\nconstructor FrameData(N... params)";}



FrameData::FrameData(
                       const ipc::void_allocator& shmemAlloc
                     , const std::string& name_
                     , StorageType storageType_
                    )
                    : IObject(name_, storageType_)
                      , m_number(shmemAlloc)
                      , m_vec_Defects(shmemAlloc)
                      , m_shmemAlloc(shmemAlloc)
                      {LOG(INFO) << "\nconstructor FrameData_base(3 params)";}

// constructor for billet
FrameData::FrameData(
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
                                          , m_shmemAlloc(shmemAlloc)
                                           { LOG(INFO) << "\nconstructor FrameData for BILLET(N... params)";}


// constructor for mandrel
FrameData::FrameData(
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
                                          , m_shmemAlloc(shmemAlloc)
                                           { LOG(INFO) <<  "\nconstructor FrameData for MANDREL(N... params)";}


// constructor for pipe
FrameData::FrameData(
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
                                          , m_shmemAlloc(shmemAlloc)
                                          { LOG(INFO) << "\nconstructor FrameData for PIPE (N... params)";}


json FrameData::toJsonDB(const FrameDataSetID &parentId) const {
  json ret = IObject::toJson();

  auto time = std::chrono::system_clock::to_time_t(m_last_detection);
  std::stringstream str;
  str << std::put_time(std::localtime(&time), "%F_%H:%M:%S");

  ret["condition"] = m_condition;
  ret["last_detection"]  = str.str();
  ret["diameter"]  = m_diameter;
  ret["length"]    = m_length;
  ret["diameter1"] = m_diameter1;
  ret["diameter2"] = m_diameter2;
  ret["diameter3"] = m_diameter3;


  std::string str_number = tmk::ipc::convertString_IPC_toSTD(m_number);
  ret["number"]    = str_number;

  if (parentId.id != IMPOSSIBLE_ID) {
      ret["frameset_id"] = parentId.id;
  }
  return ret;
}

void FrameData::fromJsonDB(const json &framedata) {
    IObject::fromJson(framedata);
    if (!framedata.contains("condition") || !framedata.contains("last_detection")  ||
        !framedata.contains("diameter")  || !framedata.contains("length")          || 
        !framedata.contains("diameter1") || !framedata.contains("diameter2")       || 
        !framedata.contains("diameter3") ) 
    {
        LOG(ERROR) << "JSON object does not contain FrameData structure";
        return;
    }
  auto datetime = framedata["datetime"].get<std::string>();
  std::tm time;
  std::get_time(&time, "%F_%H:%M:%S");
  m_last_detection = system_clock::from_time_t(mktime(&time));
  m_condition      = framedata["condition"].get<bool>();
  m_diameter       = framedata["diameter" ].get<double>();
  m_length         = framedata["length"   ].get<double>();
  m_diameter1      = framedata["diameter1"].get<double>();
  m_diameter2      = framedata["diameter2"].get<double>();
  m_diameter3      = framedata["diameter3"].get<double>();
  std::string  str_number = framedata["diameter3"].get<std::string>();
  m_number = tmk::ipc::convertString_STD_toIPC(str_number,m_shmemAlloc);

}

json FrameData::toJson() const { return toJsonDB(FrameDataSetID{""}); }
void FrameData::fromJson(const json &j) { fromJsonDB(j); }

// show info in FrameData
void FrameData::show() {
    LOG(INFO) << "\nm_condition:          " << m_condition;
    //LOG(INFO) << "\nm_last_detection:     " << tmk::service_func::getFullTimeString(m_last_detection);
    LOG(INFO) << "\nm_diameter:           " << m_diameter;
    LOG(INFO) << "\nm_length:             " << m_length;
    LOG(INFO) << "\nm_diameter1:          " << m_diameter1;
    LOG(INFO) << "\nm_diameter2:          " << m_diameter2;
    LOG(INFO) << "\nm_diameter3:          " << m_diameter3;
    LOG(INFO) << "\nm_number:             " << tmk::ipc::convertString_IPC_toSTD(m_number);

    int size_vec = m_vec_Defects.size();
    size_t count = 0;
    if (size_vec >0) {

        LOG(INFO) << "\nDefects::             ";

        for (const auto& item: m_vec_Defects)
        {
           LOG(INFO) << "\nitem [ " << count << " ] \n";
           item.show();
           LOG(INFO) << "\n";
           ++count;
        }

    }

    std::cout << std::endl;

} // end func FrameData::show()

} // namespace tmk::storage
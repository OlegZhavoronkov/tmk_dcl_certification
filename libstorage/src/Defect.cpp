#include "storage/Defect.h"

#include <glog/logging.h>

tmk::storage::Defect::Defect(const tmk::ipc::void_allocator& shmemAlloc)
                             : m_image_url(shmemAlloc)
                             {};

tmk::storage::Defect::Defect(
                               const tmk::ipc::void_allocator& shmemAlloc
                             , const int    t_defect_id
                             , const double t_position
                             , const double t_size
                             , const double t_depth
                             , const double t_precision
                             , const TimeT  t_last_detection
                             , const tmk::ipc::ipc_string t_image_url
                            ) : 
                                         m_defect_id(t_defect_id)
                                       , m_position(t_position)
                                       , m_size(t_size)
                                       , m_depth(t_depth)
                                       , m_precision( t_precision)
                                       , m_last_detection(t_last_detection)
                                       , m_image_url(t_image_url, shmemAlloc)
                                      {}

void tmk::storage::Defect::show() const {
  LOG(INFO) << "\nm_defect_id:      "  << m_defect_id;
  LOG(INFO) << "\nm_position:       "  << m_position;
  LOG(INFO) << "\nm_size:           "  << m_size;
  LOG(INFO) << "\nm_depth:          "  << m_depth;
  LOG(INFO) << "\nm_precision:      "  << m_precision;
  //LOG(INFO) << "\nm_last_detection: "  << tmk::service_func::getFullTimeString(m_last_detection);
  LOG(INFO) << "\nm_image_url:      "  << m_image_url;
}

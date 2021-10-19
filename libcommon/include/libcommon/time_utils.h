#pragma once
#include <chrono>
#include <string>
#include <sstream>
#include <iomanip>
#define BOOST_CHRONO_HEADER_ONLY
#define BOOST_CHRONO_VERSION 2
#define BOOST_ALLOW_DEPRECATED_HEADERS
#include <boost/chrono.hpp>
#include <boost/chrono/io/ios_base_state.hpp>
namespace tmk
{

namespace common
{

namespace configuration
{

namespace details
{

template<typename clock,typename duration> std::timespec 
    TimePointToTimeSpec(const std::chrono::time_point<clock,duration>& tp)
{
    using namespace std::chrono;
    auto secs = time_point_cast<seconds>( tp );
    auto nsecs = time_point_cast<nanoseconds>( tp ) - time_point_cast<nanoseconds>
                    ( secs );    
    std::timespec ret={secs.time_since_epoch().count() ,nsecs.count()};
    return ret;
}

template<   typename clock ,
            typename duration,
            long nsecPeriod=1000*1000 > std::string TimePointToString(
                    const std::chrono::time_point<clock,duration>& tp)
{
    std::timespec ts = TimePointToTimeSpec(tp);
    auto boost_tp = boost::chrono::system_clock::from_time_t( ts.tv_sec );
    std::stringstream str;
    boost::chrono::set_time_fmt(str,std::string("%x %T"));
    str << boost_tp << "." << (ts.tv_nsec / nsecPeriod);
    return str.str();
}

} //namespace details
} //namespace configuration
} //namespace common
} // namespace tmk
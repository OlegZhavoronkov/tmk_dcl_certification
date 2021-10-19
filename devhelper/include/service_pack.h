//------------------------------------------------------------------------------
//  Created     : 21.04.2021
//  Author      : Vadim Yarmushov
//  Description : service_pack consist from  pack of the general 
//                service functions (like show_mem() - show 
//                available shared memory )
// 
//------------------------------------------------------------------------------

#pragma once

#ifndef SERVICE_PACK_H
#define SERVICE_PACK_H


#include "myTypes.h"

#include <string>
#include <vector>
#include <utility> // for using std::pair
#include <boost/lexical_cast.hpp>   // for boost conversion boost::lexical_cast
#include <opencv2/core.hpp>
#include "myTypes.h"
#include "libstorage_types.h"

namespace tmk::service_func {


//using namespace tmk;
//using namespace tmk::storage;

// create new type byte
using byte =  unsigned char;


template <typename TYPE>
void swap(TYPE& val_a, TYPE& val_b)
{
  TYPE temp = std::move(val_a);
  val_a     = std::move(val_b);
  val_b     = std::move(temp );
}

// Get size of free space in shared memory segment
// void show_mem(const std::string& info);

// get RND string from vec
std::string get_RND_string(std::vector<std::string>& vec);

// get int RND value in term (start, stop);
int get_simpleRND(const int start,const int stop);

// get TYPE RND value in term (start, stop);
template<typename TYPE>
TYPE get_RND(const int start,const int stop);

// some pause for some tics time
int my_pause (int num_tic);

// convert int in string
std::string convertToString(const int number);

// convert TYPE in string, TYPE int, float, double
template<typename TYPE>
std::string convertTYPE_toString(const TYPE number);

// convert data and time in string
std::string convertTimeToString(const TimeT& t_time);
 
// get full data: date + time + ms  from TimeT t_time
std::string getFullTimeString(const TimeT& t_time);

// get full data: date + time + ms  from TimeT t_time
std::string getFullTimeString2(const TimeT& t_time);

// get date from TimeT t_time
std::string getDateString(const TimeT& t_time);

// get time from TimeT t_time
std::string getTimeString(const TimeT& t_time);

// get part with ms  of time  from TimeT t_time
std::string getTimeMSString(const TimeT& t_time);

// get part with ms  of time  from TimeT t_time
std::string getTimeWithMSString(const TimeT& t_time);

// make a affixes " ' ", add "'"" to the start and end of string
std::string make_affixes (const std::string& str_obj, std::string affix = "\'");

// make string with commas between args
std::string add_commas(std::vector<std::string>& vec_words);

// make string with commas between args of std::vector<pair std::string>
std::pair<std::string, std::string> add_commas( std::vector<std::pair<std::string, std::string>>& vec_words);

// get int value of storageType, return string value of storageType
// int = 0, return string "NO_SAVE"
std::string getStringOfStorageType(const int storageType);

// to convert string str_Data in lower_case, return this result in str_Data 
void convertString_tolowerCase(std::string& strData );

// to convert string str_Data in upper_case, return this result in str_Data 
void convertString_toupperCase(std::string& strData );

// to get string like  str_Data in lower_case, return this result in new string
std::string getString_lowerCase (const std::string& strData);

// to get string like  str_Data in lower_case, return this result in new string
std::string getString_upperCase (const std::string& strData);

// Debug info
void test_existObj(bool exist);

// convert order of byte in number from Host to Network  (little endian(Intel  CPU) to big endian)
uint32_t convert_HostToNetwork (const  uint32_t  val);

// convert order of byte in number from Network to Host  ( big endian to little endian (Intel CPU))
uint32_t convert_NetworkToHost ( const uint32_t  val);


// convert PixelFormat in cvMat_type
int convert_PixelFormat_cvMat_type(const tmk::types::PixelFormat pixelFormat);

// convert cvMat_type in PixelFormat
tmk::types::PixelFormat convert_cvMat_type_toPixelFormat(const int cv_mat_type);

// count pixel size in bytes in the dependence of cvMat_type 
int pixel_size_inBytes(const int cv_mat_type);

// count pixel size in bytes in the dependence of PixelFormat
int pixel_size_inBytes(const tmk::types::PixelFormat pixelFormat);


// get info have many channels in picture
int get_channelsBy_cvMatType(const int cv_mat_type);

// get image info  from cv::Mat mat  and save it in imageinfo 
void GetImageInfo(const cv::Mat& image, tmk::types::ImageInfo& imageinfo);

// make byteswap 12 -> 21 like  htonl<-> nlohl(big endian <->  little endian) 
uint16_t make_2byteswap (uint16_t Data);

// make byteswap 1234 -> 4321 like  htonl<-> nlohl(big endian <->  little endian) 
uint32_t make_4byteswap (uint32_t Data);

// make byteswap 12345678 -> 87654321 like  htonl<-> nlohl(big endian <->  little endian) 
uint64_t make_8byteswap (uint64_t Data);


// class helper to interpritate one type as other type 
template <typename TYPE_in, typename TYPE_out>
class CombaineType
{
   public:
   CombaineType(TYPE_in Value_in): Data_in(Value_in){}
   TYPE_out& get() {return Data_out;}

   union 
   {
    TYPE_in   Data_in;
    TYPE_out  Data_out;

   };
};


template <typename TYPE, size_t sizeTYPE_inBytes>
class ByteSwapper;

// make spacialization for 2 bytes
template <typename TYPE>
class ByteSwapper<TYPE, 2>
{
  public:
  TYPE swap(TYPE Data_in) const 
  {
    uint16_t result =  make_2byteswap(CombaineType <TYPE, uint16_t> (Data_in).get());  // we put swap value (float) for 16 bit ;)  in union, and get back type uint16_t
    return CombaineType<uint16_t, TYPE>(result).get();                           // return back float in reverse order
  }

};

// make spacialization for 4 bytes
template <typename TYPE>
class ByteSwapper<TYPE, 4>
{
  public:
  TYPE swap(TYPE Data_in) const 
  {
    uint32_t result = make_4byteswap(CombaineType <TYPE, uint32_t> (Data_in).get());  // we put swap value (float)  in union, and get back type uint16_t
    return CombaineType<uint32_t, TYPE>(result).get();                           // return back float in reverse order
  }

};

// make spacialization for 8 bytes
template <typename TYPE>
class ByteSwapper<TYPE, 8>
{
  public:
  TYPE swap(TYPE Data_in) const 
  {
    uint64_t result =  make_8byteswap(CombaineType <TYPE, uint64_t> (Data_in).get());  // we put swap value (double)  in union, and get back type uint16_t
    return CombaineType<uint64_t, TYPE>(result).get();                           // return back (double) in reverse order
  }

};

// main function which auto change byte order from (Big Endian on Little Endian and back  LE -> BE)
// function change byte order in value, use classes ByteSwapper<TYPE, 8>, ByteSwapper<TYPE, 4>,
// ByteSwapper<TYPE, 2>, class CombaineType, and func make_8byteswap, make_byteswap, make_2byteswap
// we will call ByteSwapper by sizeof(TYPE), sizeof(int) = 4,then ByteSwapper<int, 4> 
template <typename TYPE>
TYPE ByteSwap(TYPE Data_in)
{
  return ByteSwapper<TYPE, sizeof(TYPE)>().swap(Data_in);
}




} // end namespace tmk::service_func



template<typename TYPE>
TYPE tmk::service_func::get_RND(const int start, const int stop)
{
  TYPE result = 0;

  int end = stop - 1;

  int whole_part       = tmk::service_func::get_simpleRND(start, end);
  int fractional_part  = tmk::service_func::get_simpleRND(0, 999);
  result =  static_cast<TYPE>(whole_part) + static_cast<TYPE>((fractional_part) / 1000);

  return result;
}




template<typename TYPE>
std::string tmk::service_func::convertTYPE_toString(const TYPE number)
{
    // ====  work  edition  1 
   /* 
   std::ostringstream ss;
   ss << number;
   std::string s(ss.str());
   return s;
   */

   // ====  work  edition 2
   // from boost
    std::string s = boost::lexical_cast<std::string>(number);
    return s;

    /*
    // ==== work  edition 3 
    std::stringstream ss;
    std::string res;

    // get ref on framedataset
    ss << number; 
    ss >> res;
    ss.clear();

    return res;
    */
}

#endif // end ifndef SERVICE_PACK_H

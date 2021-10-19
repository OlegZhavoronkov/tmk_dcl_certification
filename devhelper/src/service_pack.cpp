#include <storage/service_pack.h>

// #include <storage/Ipc.h>

//#include <storage/ObjectDescriptor.h>


#include <iostream>
#include <iomanip>       // for put_time()
#include <sstream>
#include <math.h>        // round, floor, ceil, trunc 
#include <netinet/in.h>  // for ntohl


#include <glog/logging.h>
/*
void tmk::service_func::show_mem(const std::string& info)
{
    size_t size_ShareMemory = tmk::ipc::availableSharedMemory("SharedObjects");
    std::cout << "\nYour shared memory: " 
              << size_ShareMemory << " " << info << "\n";
}
*/

// get RND string from vec
std::string tmk::service_func::get_RND_string(std::vector<std::string>& vec)
{
     int stop_lim = vec.size()-1;
    return vec[ (tmk::service_func::get_simpleRND(1, stop_lim))]; 
}

int tmk::service_func::get_simpleRND(const int start,const  int stop) {
      
       return  (rand() % (stop - start + 1) + start);
  //   return  (start + rand() % stop); // wrong formula
}


int tmk::service_func::my_pause (int num_tic)
{
    int count = 0;
    for (auto i = 0; i < num_tic; ++i)
    {
      ++count;
    }

    return count;
}



std::string tmk::service_func::convertToString(const int number)
{
    std::stringstream ss;
    std::string res;

    // get ref on framedataset
    ss << number; 
    ss >> res;
    ss.clear();
         
    return res;               
}



/*
// enum class StorageType { NO_SAVE, SHORT_TERM, LONG_TERM };
// get int value of storageType, return string value of storageType
// int = 0, return string "NO_SAVE"
std::string tmk::service_func::getStringOfStorageType(const tmk::storage::StorageType storageType)
{
    std::string result;

    switch (storageType)
    {
      case tmk::storage::StorageType::NO_SAVE:     {result = "NO_SAVE";    break;  }
      case tmk::storage::StorageType::SHORT_TERM:  {result = "SHORT_TERM"; break;  }
      case tmk::storage::StorageType::LONG_TERM:   {result = "LONG_TERM";  break;  }
      default:                       {result = "UNKNOWN_STORAGE_TYPE"; break; }
    }

    return result;


}
*/

// get int value of storageType, return string value of storageType
// int = 0, return string "NO_SAVE"
std::string getStringOfStorageType(const int storageType)
{
     std::string result;

    switch (storageType)
    {
      case 0: {result = "NO_SAVE";    break;  } // tmk::storage::StorageType::NO_SAVE:
      case 1: {result = "SHORT_TERM"; break;  } // tmk::storage::StorageType::SHORT_TERM:
      case 2: {result = "LONG_TERM";  break;  } // tmk::storage::StorageType::LONG_TERM:
      default:                       {result = "UNKNOWN_STORAGE_TYPE"; break; }
    }

    return result;
}


// get part with ms  of time  from TimeT t_time
std::string tmk::service_func::getTimeMSString(const TimeT& t_time)
{
    std::stringstream ss;
    std::string part_ms;

    const auto now_ms = std::chrono::time_point_cast<std::chrono::milliseconds>(t_time);
    const auto now_s  = std::chrono::time_point_cast<std::chrono::seconds>(now_ms);
    const auto addPart_millis = now_ms - now_s;
   

    // get ref on framedataset
    // ss << number; 
    ss << addPart_millis.count();
     
    ss >> part_ms;
    ss.clear();


    return part_ms;          
}



// get time from TimeT t_time
std::string tmk::service_func::getTimeString(const TimeT& t_time)
{

    std::stringstream ss;
    std::string myTime;

    const auto all_time_t = std::chrono::system_clock::to_time_t(t_time);
 
/* 
 // manual get date
   // convert time_t to tm
   tm* date_tm = localtime(&all_time_t);

   ss << 1900 + date_tm->tm_year;    // set year
   ss << "-";
   ss << 1 + date_tm->tm_mon;        // set month 
   ss << "-";
   ss << date_tm->tm_mday;           // set day
*/
   // it's work select date in all data tm 
    ss << std::put_time(gmtime(&all_time_t), "%T");
    ss >> myTime;
    ss.clear();


    return myTime;      
}


// get part with ms  of time  from TimeT t_time
std::string tmk::service_func::getTimeWithMSString(const TimeT& t_time)
{
    const auto now_ms = std::chrono::time_point_cast<std::chrono::milliseconds>(t_time);
    const auto now_s  = std::chrono::time_point_cast<std::chrono::seconds>(now_ms);
    const auto addPart_millis = now_ms - now_s;


    std::stringstream ss;
    std::string myTime;

    const auto all_time_t = std::chrono::system_clock::to_time_t(t_time);


   // it's work select date in all data tm 
    ss << std::put_time(gmtime(&all_time_t), "%T");
    ss << ".";
    ss << addPart_millis.count();
    ss >> myTime;
    ss.clear();


    return myTime; 
}

// get full data: date + time + ms  from TimeT t_time
std::string tmk::service_func::getFullTimeString2(const TimeT& t_time)
{

std::string myDate              =  tmk::service_func::getDateString(t_time);
std::string myTimeMS            =  tmk::service_func::getTimeWithMSString(t_time);
std::string resTtime = myDate + " " + myTimeMS; 

return  resTtime;

}


// get full data: date + time + ms  from TimeT t_time
std::string tmk::service_func::getFullTimeString(const TimeT& t_time)
{
    std::stringstream ss;
    std::string fulldate;

    const auto now_ms = std::chrono::time_point_cast<std::chrono::milliseconds>(t_time);
    const auto now_s  = std::chrono::time_point_cast<std::chrono::seconds>(t_time);
    const auto addPart_millis = now_ms - now_s;


    const auto all_time_t = std::chrono::system_clock::to_time_t(t_time);

    ss << std::put_time(gmtime(&all_time_t), "%F");
    ss << " ";
    ss << std::put_time(gmtime(&all_time_t), "%T");
    ss << ".";
    ss << addPart_millis.count();
    ss >> fulldate;
   
    ss.clear();

    //LOG(INFO) << "\nCONTROL TTime : " << fulldate;
    return fulldate;  
}


// get data from TimeT t_time
std::string tmk::service_func::getDateString(const TimeT& t_time)
{
    std::stringstream ss;
    std::string date;

    const auto all_time_t = std::chrono::system_clock::to_time_t(t_time);
 
/* // mabual case to get date
   // manual get date
   // convert time_t to tm
   tm* date_tm = localtime(&all_time_t);

   ss << 1900 + date_tm->tm_year;    // set year
   ss << "-";
   ss << 1 + date_tm->tm_mon;        // set month 
   ss << "-";
   ss << date_tm->tm_mday;           // set day
*/
   // it's work select date in all data tm 
    ss << std::put_time(gmtime(&all_time_t), "%F");
    ss >> date;
    ss.clear();


    return date;          

}


std::string tmk::service_func::convertTimeToString(const TimeT& t_time)
{
    std::stringstream ss;
    std::string data;

    const auto now_ms = std::chrono::time_point_cast<std::chrono::milliseconds>(t_time);
    const auto now_s  = std::chrono::time_point_cast<std::chrono::seconds>(now_ms);
    const auto addPart_millis = now_ms - now_s;
    const auto c_now  = std::chrono::system_clock::to_time_t(now_s);

    // get ref on framedataset
    //     ss << number; 
    ss << std::put_time(gmtime(&c_now), "%FT%T")
       << '.' << std::setfill('0') << std::setw(3) << addPart_millis.count() << 'Z';
    ss >> data;
    ss.clear();


    return data;          
}



// make a affixes like " ' ", add "'" to the start and end of string
// str_obj = "Hello", affix = "/" 
// res = after make_affixes (str_obj, affix ); res = /Hello/
std::string tmk::service_func::make_affixes (const std::string& str_obj, std::string affix ) 
{
    return (affix + str_obj + affix); 
}




// make string with commas between args of std::vector<std::string>
std::string tmk::service_func::add_commas(std::vector<std::string> &vec_words)
{
    std::string result = "";
    int count          = 0;

    for (auto& word: vec_words) {
    // first value  [count = 0]  result =  result + value
    // second value [count = 1]  result =  result + ", " + value ...
        if (count) {
        result = result + ", " +  std::move(word);
        }
        else {
        result = result + std::move(word);
        }

        ++count;
    }

    return result;
}


// make string with commas between args of std::vector<pair std::string>
std::pair<std::string, std::string> tmk::service_func::add_commas(std::vector<std::pair<std::string, std::string> >& vec_words)
{
    std::string result_nameColumn = "";
    std::string result_values     = "";
    int count = 0;

    for (auto& [nameColumn, value]: vec_words) {
    // first value  [count = 0]  result =  result + value
    // second value [count = 1]  result =  result + ", " + value ...
        if (count) {
        result_nameColumn = result_nameColumn + ", " +  std::move(nameColumn);
        result_values     = result_values     + ", " +  std::move(value);
        }
        else {
        result_nameColumn = result_nameColumn + std::move(nameColumn);
        result_values     = result_values     + std::move(value);
        }

        ++count;
    }

    return std::pair<std::string, std::string> (result_nameColumn, result_values);
}



// to convert string str_Data in lower_case
void tmk::service_func::convertString_tolowerCase(std::string& strData)
{
    std::for_each(strData.begin(), strData.end(), [](char & c) {c = ::tolower(c);});
}



// to convert string str_Data in upper_case, return this result in str_Data 
void tmk::service_func::convertString_toupperCase(std::string& strData )
{
    std::for_each(strData.begin(), strData.end(), [](char & c) {c = ::toupper(c);});
}


// to get string like  str_Data in lower_case, return this result in new string
std::string tmk::service_func::getString_lowerCase (const std::string& strData)
{

   std::string res = strData;                           // get copy
   tmk::service_func::convertString_tolowerCase(res);   // convert in lover case
   return res;
}

// to get string like  str_Data in lower_case, return this result in new string
std::string tmk::service_func::getString_upperCase (const std::string& strData)
{
   std::string res = strData;
   tmk::service_func::convertString_toupperCase(res);
   return res;
}




void tmk::service_func::test_existObj(bool exist)
{
   if (exist) {
       LOG(INFO) << "\n#### obj exist ! ####";
   }
   else {
       LOG(INFO) << "\n#### obj not exist ! ####";
   }
}



// конверт PixelFormat in cvMat_type
int tmk::service_func::convert_PixelFormat_cvMat_type(const tmk::types::PixelFormat pixelFormat)
{
   int result_cvMat_type;

  switch(pixelFormat)
  {
       case tmk::types::PixelFormat::BW:             { result_cvMat_type = CV_8UC1;   break;}
       case tmk::types::PixelFormat::GRAY8:          { result_cvMat_type = CV_8UC1;   break;}
       case tmk::types::PixelFormat::GRAYALPHA16:    { result_cvMat_type = CV_8UC2;   break;}
       case tmk::types::PixelFormat::RGB16_555:      { result_cvMat_type = CV_8UC2;   break;}
       case tmk::types::PixelFormat::RGB16_565:      { result_cvMat_type = CV_8UC2;   break;}
       case tmk::types::PixelFormat::RGB24:          { result_cvMat_type = CV_8UC3;   break;}
       case tmk::types::PixelFormat::BGR24:          { result_cvMat_type = CV_8UC3;   break;}
       case tmk::types::PixelFormat::RGBA32:         { result_cvMat_type = CV_8UC4;   break;}
       case tmk::types::PixelFormat::BGRA32:         { result_cvMat_type = CV_8UC4;   break;}
       default :                                     { result_cvMat_type = -1;   break;}
  }

  return result_cvMat_type;
}


// convert cvMat_type in PixelFormat
tmk::types::PixelFormat tmk::service_func::convert_cvMat_type_toPixelFormat(const int cv_mat_type)
{
   tmk::types::PixelFormat pixelformat;

  switch(cv_mat_type)
  {
       case CV_8UC1:    { pixelformat = tmk::types::PixelFormat::GRAY8;       break;}
       case CV_8UC2:    { pixelformat = tmk::types::PixelFormat::GRAYALPHA16; break;}
       case CV_8UC3:    { pixelformat = tmk::types::PixelFormat::RGB24;       break;}
       case CV_8UC4:    { pixelformat = tmk::types::PixelFormat::RGBA32;      break;}
       default :        { pixelformat = tmk::types::PixelFormat::NoFormat;    break;}
  }
  return pixelformat;
}

// get info have many channels in picture
int tmk::service_func::get_channelsBy_cvMatType(const int cv_mat_type) {

    int channels = -1;

  switch(cv_mat_type)
  {
       case CV_8UC1:    { channels = 1; break;}
       case CV_8UC2:    { channels = 2; break;}
       case CV_8UC3:    { channels = 3; break;}
       case CV_8UC4:    { channels = 4; break;}
       default :        { channels = -1; break;}
  }

    return channels;
}




void  tmk::service_func::GetImageInfo(const cv::Mat& image, tmk::types::ImageInfo& imageinfo)
{
   imageinfo.height = image.rows;
   imageinfo.width  = image.cols;
   imageinfo.color_type = tmk::service_func::convert_cvMat_type_toPixelFormat(image.type());
   imageinfo.channels   = get_channelsBy_cvMatType(image.type());
   imageinfo.bit_depth  = 8;  // now its a const
   imageinfo.size_pixel = int (ceil(( (static_cast<double>(imageinfo.channels)) * imageinfo.bit_depth )/ 8)) ;
}



// count pixel size in bytes in the dependence of cvMat_type 
int tmk::service_func::pixel_size_inBytes(const int cv_mat_type)
{
  int pixel_size = -1;  // pixel_size in bytes

  switch(cv_mat_type)
  {
       case CV_8UC1:    { pixel_size =  1; break;}
       case CV_8UC2:    { pixel_size =  2; break;}
       case CV_8UC3:    { pixel_size =  3; break;}
       case CV_8UC4:    { pixel_size =  4; break;}
       default :        { pixel_size = -1; break;}
  }

    return pixel_size;
}




// count pixel size in bytes in the dependence of PixelFormat
int tmk::service_func::pixel_size_inBytes(const tmk::types::PixelFormat pixelFormat)
{
  int pixel_size = -1;  // pixel_size in bytes

  switch(pixelFormat)
  {
       case tmk::types::PixelFormat::BW:             { pixel_size = 1;   break;}
       case tmk::types::PixelFormat::GRAY8:          { pixel_size = 1;   break;}
       case tmk::types::PixelFormat::GRAYALPHA16:    { pixel_size = 2;   break;}
       case tmk::types::PixelFormat::RGB16_555:      { pixel_size = 2;   break;}
       case tmk::types::PixelFormat::RGB16_565:      { pixel_size = 2;   break;}
       case tmk::types::PixelFormat::RGB24:          { pixel_size = 3;   break;}
       case tmk::types::PixelFormat::BGR24:          { pixel_size = 3;   break;}
       case tmk::types::PixelFormat::RGBA32:         { pixel_size = 4;   break;}
       case tmk::types::PixelFormat::BGRA32:         { pixel_size = 4;   break;}
       default :                                     { pixel_size = -1;  break;}
  }

  return pixel_size;
}


// convert int value from Intel to htons order of bytes  (HostToNetwork) (intel use little endian it mean what  value  0100(hex) will store in memory: 00 01)
 uint32_t tmk::service_func::convert_HostToNetwork (const  uint32_t  val)
{
  return htonl(val);   //host to network short
}

 uint32_t tmk::service_func::convert_NetworkToHost ( const uint32_t  val)
{
  return ntohl(val);   //network to host short
}



// make byteswap 12 -> 21 like  htonl<-> nlohl(big endian <->  little endian) 
uint16_t tmk::service_func::make_2byteswap (uint16_t Data)
{
  return ( (Data >> 8) | 
           (Data << 8)  ); 
    
}


// make byteswap 1234 -> 4321 like  htonl<-> nlohl(big endian <->  little endian) 
uint32_t tmk::service_func::make_4byteswap (uint32_t Data)
{
  return ( (Data >> 24) & 0x000000ff | 
           (Data >>  8) & 0x0000ff00 | 
           (Data <<  8) & 0x00ff0000 | 
           (Data << 24) & 0xff000000   );
 
}


uint64_t tmk::service_func::make_8byteswap (uint64_t Data)
{
  return ( (Data >> 56) & 0x00000000000000ff | 
           (Data >> 40) & 0x000000000000ff00 | 
           (Data >> 24) & 0x0000000000ff0000 | 
           (Data >>  8) & 0x00000000ff000000 | 
           (Data <<  8) & 0x000000ff00000000 | 
           (Data << 24) & 0x0000ff0000000000 | 
           (Data << 40) & 0x00ff000000000000 | 
           (Data << 56) & 0xff00000000000000  );
}



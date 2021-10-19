#pragma once
#ifndef __CONFIG_EXCEPTIONS_H__
#define __CONFIG_EXCEPTIONS_H__


#include <stdexcept>

namespace tmk
{
namespace common
{
namespace configuration
{

class configuration_exception:public std::runtime_error
{

public:
    configuration_exception(const std::string& filename);
    const char* what() const noexcept override;
protected:
    std::string _config_path;
private:
    std::string _msg;    
};

class config_not_found_exception: public configuration_exception
{
public:
    config_not_found_exception(const std::string& filename);
    const char* what() const noexcept override;
private:
    std::string _msg;
};

class bad_config_exception: public configuration_exception
{
public:
    bad_config_exception(const std::string& filename,const std::string& reason);
    bad_config_exception(const std::string& reason);
    const char* what() const noexcept override;
protected:
    std::string _reason;
private:
    std::string _msg;
};


} //namespace configuration
} //namespace common
} // namespace tmk

#endif // __CONFIG_EXCEPTIONS_H__
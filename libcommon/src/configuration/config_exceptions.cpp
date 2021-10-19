#include <boost/format.hpp>
#include <libcommon/config_exceptions.h>

namespace tmk
{
namespace common
{

namespace configuration
{
    

configuration_exception::configuration_exception(const std::string& filename) :     
                std::runtime_error("configuration exception"),
                _config_path(filename),
                _msg(  (boost::format("configuration file \"%s\" parsing exception") 
                            % filename
                        ).str() )
{
}


config_not_found_exception::config_not_found_exception(const std::string& filename) 
                : configuration_exception(filename)
{
    _msg=   (boost::format("%s : file not found") 
                % configuration_exception::what())
            .str();
}


bad_config_exception::bad_config_exception( const std::string& filename,
                                            const std::string& reason) 
                                    :   configuration_exception(filename),
                                        _reason(reason)
{
    _msg =  (boost::format("%s : %s") %configuration_exception::what() % _reason)
            .str();
}


bad_config_exception::bad_config_exception(const std::string& reason) 
        : bad_config_exception::bad_config_exception(   std::string("not known"),
                                                        reason)
{

}

const char* configuration_exception::what() const noexcept 
{   
    return _msg.c_str();
}


const char* config_not_found_exception::what() const noexcept 
{
    return _msg.c_str();
}


const char* bad_config_exception::what() const noexcept 
{   
    return _msg.c_str();
}



} //configuration
} //namespace common
} // namespace tmk
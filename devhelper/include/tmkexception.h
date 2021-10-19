#pragma once

#ifndef TMKEXCEPTION_H
#define TMKEXCEPTION_H

#include <exception>
#include <string>
	


class TMKEXCEPTION: public std::exception
{

    public:
    // constructor
    TMKEXCEPTION(const std::string& ERR_message) : m_ERR_message(ERR_message) {}

    TMKEXCEPTION(const std::string& ERR_whatfunc, const std::string& ERR_type_of_err) : m_ERR_message(ERR_whatfunc  + " " + ERR_type_of_err ) {}

    // get info from  m_ERR_message
    const char* what()  const noexcept {return m_ERR_message.c_str();}


    private: 

    std::string m_ERR_message;


};




#endif  // TMKEXCEPTION_H
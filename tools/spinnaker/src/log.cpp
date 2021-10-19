//------------------------------------------------------------------------------
//  Created     : 06.08.21
//  Author      : Victor Kats
//  Description : Import Spinnnaker-based capturer into project (see TMKDCL-380)
//------------------------------------------------------------------------------

#include <map>
#include <iostream>

#include "log.h"
#include "threadname.h"

const std::map<const Log::LogType, const std::string> logName =
{
    {Log::Debug,    " Debug: "},
    {Log::Time,     " Time: "},
    {Log::Trace,    " Trace: "},
    {Log::Notice,   " Notice: "},
    {Log::STATUS,   " Status: "},
    {Log::Warning,  " Warning: "},
    {Log::Error,    " Error: "},
};

const std::map<const Log::LogType, const std::string> logColor =
{
    {Log::Debug,    ""},
    {Log::Time,     ""},
    {Log::Trace,    ""},
    {Log::Notice,   ""},
    {Log::STATUS,   "\033[1;32m"},
    {Log::Warning,  "\033[1;33m"},
    {Log::Error,    "\033[1;91m"},
};

Log::Log(Log::LogType _t)
    : t(_t)
{
    *this << spintools::getThreadName() << logName.at(t);
    if (autoColorized()) *this << logColor.at(t);
}

Log::~Log()
{
#ifdef RELEASE
    if (t == Debug || t == Time || t == Trace) return;
#endif
    if (autoColorized()) *this << "\033[0m";
    std::lock_guard<std::mutex> guard(_mutexLog);
    std::cout << this->str() << '\n';
}

bool Log::autoColorized() const
{
    return !logColor.at(t).empty();
}

std::mutex Log::_mutexLog{};


LogTrace::LogTrace(const std::string &file, const std::string &line, const std::string &func)
    : function(func)
    , file(file)
{
    Log(Log::Trace) << "File: " << file << " Line: " << line << " Method enter: " << func;
}

LogTrace::~LogTrace()
{
    Log(Log::Trace) << "File: " << file << " Method exit: " << function;
}

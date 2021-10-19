//------------------------------------------------------------------------------
//  Created     : 06.08.21
//  Author      : Victor Kats
//  Description : Import Spinnnaker-based capturer into project (see TMKDCL-380)
//------------------------------------------------------------------------------

#ifndef LOG_H
#define LOG_H

#include <iostream>
#include <sstream>
#include <mutex>
#include <map>
#include <list>

#include <time.h>

// inspired by https://stackoverflow.com/a/41602932/4108202
class Log : public std::ostringstream
{
public:
    enum LogType
    {
        Debug = 0,
        Time,
        Trace,
        Notice,
        STATUS,
        Warning,
        Error
    };

    Log(LogType _t);

    virtual ~Log();

    bool autoColorized() const;

protected:
    static std::mutex _mutexLog;
    LogType t;
};

class LogTrace
{
public:
    LogTrace(const std::string &file, const std::string &line, const std::string &func);
    ~LogTrace();
private:
    std::string function;
    std::string file;
};

#ifdef WIN32
#define LOGTRACE LogTrace lt(__FILE__,std::to_string(__LINE__),__FUNCTION__);
#else
#define LOGTRACE LogTrace lt(__FILE__,std::to_string(__LINE__),__func__);
#define LOGVISITOR Log(Log::Notice) << __FILE__ << ":" << std::to_string(__LINE__) << ":" << __func__;
#define LOG_VISITOR Log(Log::Notice) << __FILE__ << ":" << std::to_string(__LINE__) << ":" << __func__;
#define LOG_NOTICE(x) Log(Log::Notice) << __FILE__ << ":" << std::to_string(__LINE__) << " " << x;
#define LOG_STATUS(x) Log(Log::STATUS) << __FILE__ << ":" << std::to_string(__LINE__) << " " << x;
#define LOG_WARNING(x) Log(Log::Warning) << __FILE__ << ":" << std::to_string(__LINE__) << " " << x;
#define LOG_ERROR(x) Log(Log::Error) << __FILE__ << ":" << std::to_string(__LINE__) << " " << x;
#endif
#define LOGTRACEFN(func) LogTrace lt(__FILE__,std::to_string(__LINE__),func);



class AlgorithmAverageSpeedTester : public std::map<std::string, std::list<int>>
{
public:
    void add(const char *name, int value)
    {
        ((*this)[name]).push_back(value);
    }
    static AlgorithmAverageSpeedTester *instance()
    {
        static AlgorithmAverageSpeedTester  instance;
        return &instance;
    }
    ~AlgorithmAverageSpeedTester()
    {
        for(auto it : *this)
        {
            if(it.second.size() > 0)
            {
                long long sum = 0;
                for( auto it2 : it.second) sum += it2;
                std::cerr << "Average time for " << it.first << " is: "
                          << (float)sum*1000 / it.second.size() / CLOCKS_PER_SEC
                          << std::endl;
            }
        }
    }

private:
    AlgorithmAverageSpeedTester() {}
};


class AlgorithmSpeedTester
{
public:
    AlgorithmSpeedTester(const char *name) : m_start(clock()), m_name(name) { }
    ~AlgorithmSpeedTester()
    {
        int res = clock() - m_start;
        AlgorithmAverageSpeedTester::instance()->add(m_name, res);
        std::cerr << "Time meter for " << m_name << ": " << ((float)res*1000 / CLOCKS_PER_SEC) << std::endl;
    }

private:
    int         m_start;
    const char *m_name;
};

#define TIME_METER(a) AlgorithmSpeedTester t((a));
#endif // LOG_H

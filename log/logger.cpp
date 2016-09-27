
#include <stdarg.h>
#include <stdio.h>
#include <fstream>

#include "logger.h"

long long utc_timestamp()
{
    struct timeval  tv;

    gettimeofday(&tv, NULL);

    return (long long)tv.tv_sec * 1000 + tv.tv_usec / 1000 ;
}

namespace swss {


Logger::Priority Logger::m_minPrio = Logger::SWSS_NOTICE;

Logger::Logger()
{
    std::ifstream self("/proc/self/cmdline", std::ios::binary);

    m_self = "unknown";

    if (self.is_open())
    {
        self >> m_self;

        size_t found = m_self.find_last_of("/");

        if (found != std::string::npos)
        {
            m_self = m_self.substr(found + 1);
        }

        self.close();
    }
}

Logger &Logger::getInstance()
{
    static Logger m_logger;
    return m_logger;
}

void Logger::setMinPrio(Priority prio)
{
    m_minPrio = prio;
}

Logger::Priority Logger::getMinPrio()
{
    return m_minPrio;
}

void Logger::write(Priority prio, const char *format, ...)
{
    if (prio > m_minPrio)
        return;

    // TODO
    // + add thread id using std::thread::id this_id = std::this_thread::get_id();
    // + add timestmap with millisecond resolution

    va_list argptr;
    va_start(argptr, format);
    vfprintf(stderr, format, argptr);
    va_end(argptr);
    fprintf(stderr,"\n");
}

Logger::ScopeLogger::ScopeLogger(int line, const char *fun) : m_line(line), m_fun(fun)
{
    swss::Logger::getInstance().write(swss::Logger::SWSS_DEBUG, ":> %s: enter", m_fun);
}

Logger::ScopeLogger::~ScopeLogger()
{
    swss::Logger::getInstance().write(swss::Logger::SWSS_DEBUG, ":< %s: exit", m_fun);
}

Logger::ScopeTimer::ScopeTimer(int line, const char *fun, std::string msg) :
    m_line(line),
    m_fun(fun),
    m_msg(msg)
{
    m_start = std::chrono::high_resolution_clock::now();
}

Logger::ScopeTimer::~ScopeTimer()
{
    auto end = std::chrono::high_resolution_clock::now();

    double duration = std::chrono::duration_cast<second_t>(end - m_start).count();

    Logger::getInstance().write(swss::Logger::SWSS_INFO, ":- %s: %s took %lf", m_fun, m_msg.c_str(), duration);
}

};

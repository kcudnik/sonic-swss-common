#ifndef SWSS_COMMON_LOGGER_H
#define SWSS_COMMON_LOGGER_H

#include <string>
#include <chrono>

#include <sys/time.h>
#include <unistd.h>
#include <sys/syscall.h>
#define gettid() syscall(SYS_gettid)

extern long long utc_timestamp();

namespace swss {

#define CONCAT_IMPL(x,y) x##y
#define MACRO_CONCAT(x,y) CONCAT_IMPL(x,y)

#define SWSS_LOG_DEBUG(MSG, ...) swss::Logger::getInstance().write(swss::Logger::SWSS_DEBUG,\
        "d,%s:%d,%d,%d,%lld @ %s @ :- %s: " MSG, __FILE__, __LINE__, getpid(), gettid(), utc_timestamp(), __PRETTY_FUNCTION__, __FUNCTION__, ##__VA_ARGS__)

#define SWSS_LOG_ENTER() swss::Logger::ScopeLogger  MACRO_CONCAT(logger,__COUNTER__) (__LINE__, __PRETTY_FUNCTION__)

class Logger
{
public:
    enum Priority
    {
        SWSS_EMERG,
        SWSS_ALERT,
        SWSS_CRIT,
        SWSS_ERROR,
        SWSS_WARN,
        SWSS_NOTICE,
        SWSS_INFO,
        SWSS_DEBUG
    };

    static Logger &getInstance();
    static void setMinPrio(Priority prio);
    static Priority getMinPrio();
    void write(Priority prio, const char *fmt, ...);

    static std::string priorityToString(Priority prio);
    static Priority stringToPriority(const std::string);

    class ScopeLogger
    {
        public:

        ScopeLogger(int line, const char *fun);
        ~ScopeLogger();

        private:
            const int m_line;
            const char *m_fun;
    };

    class ScopeTimer
    {
        typedef std::chrono::duration<double, std::ratio<1>> second_t;

        public:

            ScopeTimer(int line, const char* fun, std::string msg);
            ~ScopeTimer();

        private:

            const int m_line;
            const char *m_fun;

            std::string m_msg;

            std::chrono::time_point<std::chrono::high_resolution_clock> m_start;
    };

private:
    Logger();
    Logger(const Logger&);
    Logger &operator=(const Logger&);

    std::string m_self;

    static Priority m_minPrio;
};

}

#endif /* SWSS_COMMON_LOGGER_H */

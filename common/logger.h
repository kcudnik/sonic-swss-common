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

#define SWSS_JOIN(x,y) x##y
#define MACRO_JOIN(x,y) SWSS_JOIN(x,y)

#define SWSS_LOGGER(LEVEL,L,MSG,...) \
    swss::Logger::getInstance().write(LEVEL, L ",%s:%d,%d,%d,%lld @ %s @ :- %s: " MSG, \
            __FILE__, __LINE__, getpid(), gettid(), utc_timestamp(), __PRETTY_FUNCTION__, __FUNCTION__, ##__VA_ARGS__)

#define SWSS_LOG_ERROR(MSG, ...)       SWSS_LOGGER(swss::Logger::SWSS_ERROR,  "e", MSG, ##__VA_ARGS__)
#define SWSS_LOG_WARN(MSG, ...)        SWSS_LOGGER(swss::Logger::SWSS_WARN,   "w", MSG, ##__VA_ARGS__)
#define SWSS_LOG_NOTICE(MSG, ...)      SWSS_LOGGER(swss::Logger::SWSS_NOTICE, "n", MSG, ##__VA_ARGS__)
#define SWSS_LOG_INFO(MSG, ...)        SWSS_LOGGER(swss::Logger::SWSS_INFO,   "i", MSG, ##__VA_ARGS__)
#define SWSS_LOG_DEBUG(MSG, ...)       SWSS_LOGGER(swss::Logger::SWSS_DEBUG,  "d", MSG, ##__VA_ARGS__)

#define SWSS_LOG_ENTER()               swss::Logger::ScopeLogger MACRO_JOIN(logger, __COUNTER__) (__FILE__, __LINE__, __PRETTY_FUNCTION__, __FUNCTION__)
#define SWSS_LOG_TIMER(MSG)            swss::Logger::ScopeTimer scopetimer ## __LINE__ (__LINE__, __FUNCTION__, MSG)

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

        ScopeLogger(const char* file, int line, const char* pretty_function, const char* function);

        ~ScopeLogger();

        private:

            const char* m_file;
            int m_line;
            int m_pid;
            int m_tid;
            long long m_timestamp;
            const char* m_pretty_function;
            const char* m_function;
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

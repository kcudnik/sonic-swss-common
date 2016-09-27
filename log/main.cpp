#include "logger.h"

#include <thread>


template <class T, class U>
class  A
{
    public:
        A(T i): m_i(i)
        {
            SWSS_LOG_ENTER();
        }

        void test()
        {
            SWSS_LOG_ENTER();

            SWSS_LOG_DEBUG("intest %d", m_i);

        }

        ~A()
        {
            SWSS_LOG_ENTER();
        }
    private:

        T m_i;
};

class B : public A<int, long>
{
    public:
        B():A<int, long>(7)
        {
            SWSS_LOG_ENTER();
        }
};

typedef int (*fun)(int*,long*);

fun call_from_thread() {
    SWSS_LOG_ENTER();

    SWSS_LOG_DEBUG("foo");

    return NULL;
}
int main()
{
    swss::Logger::setMinPrio(swss::Logger::Priority::SWSS_DEBUG);

    SWSS_LOG_ENTER();
    SWSS_LOG_ENTER();

     std::thread t1(call_from_thread);
    {
        A<int, long> a(1);

        a.test();

        SWSS_LOG_DEBUG("test %d", 1);

        B b;
    }
                  t1.join();

    return 0;
}


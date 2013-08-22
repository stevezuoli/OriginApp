#ifndef __COMMON_LOCKED_STACK_H__
#define __COMMON_LOCKED_STACK_H__

#include <stack>
#include <pthread.h>

namespace dk
{
namespace common
{
template<typename T>
class LockedStack
{

public:
    LockedStack()
    {
        pthread_mutex_init(&m_mutex, NULL);
        pthread_cond_init (&m_cond, NULL);
    }

    ~LockedStack()
    {
        pthread_mutex_destroy(&m_mutex);
        pthread_cond_destroy(&m_cond);
    }

    bool Receive(T* t)
    {
        pthread_mutex_lock(&m_mutex);
        if (m_stack.empty())
        {
            pthread_cond_wait(&m_cond, &m_mutex);
        }
        *t = m_stack.top();
        m_stack.pop();
        pthread_mutex_unlock(&m_mutex);
        return true;
    }

    bool Send(const T& t)
    {
        pthread_mutex_lock(&m_mutex);
        m_stack.push(t);
        pthread_cond_signal(&m_cond);
        pthread_mutex_unlock(&m_mutex);
        return true;
    }
private:
    LockedStack<T>& operator=(const LockedStack&);
    LockedStack(const LockedStack<T>&);

    pthread_mutex_t m_mutex;
    pthread_cond_t m_cond;
    std::stack<T> m_stack;
};
} // namespace common
} // namespace dk
#endif


#ifndef __COMMON_LOCKED_QUEUE_H__
#define __COMMON_LOCKED_QUEUE_H__

#include <deque>

namespace dk
{
namespace common
{
template<typename T>
class LockedQueue
{

public:
    LockedQueue()
    {
        pthread_mutex_init(&m_mutex, NULL);
        pthread_cond_init (&m_cond, NULL);
    }

    ~LockedQueue()
    {
        pthread_mutex_destroy(&m_mutex);
        pthread_cond_destroy(&m_cond);
    }

    bool Receive(T* t)
    {
        pthread_mutex_lock(&m_mutex);
        if (m_queue.empty())
        {
            pthread_cond_wait(&m_cond, &m_mutex);
        }
        *t = m_queue.front();
        m_queue.pop_front();
        pthread_mutex_unlock(&m_mutex);
        return true;
    }

    bool Send(const T& t)
    {
        pthread_mutex_lock(&m_mutex);
        m_queue.push_back(t);
        pthread_cond_signal(&m_cond);
        pthread_mutex_unlock(&m_mutex);
        return true;
    }
private:
    LockedQueue<T>& operator=(const LockedQueue&);
    LockedQueue(const LockedQueue<T>&);

    pthread_mutex_t m_mutex;
    pthread_cond_t m_cond;
    std::deque<T> m_queue;
};
} // namespace common
} // namespace dk
#endif


#ifndef __COMMON_LOCKOBJECT_H__
#define __COMMON_LOCKOBJECT_H__

#include <pthread.h>

namespace dk
{
namespace common
{

class LockObject
{
public:
    LockObject(){
        (void)pthread_mutex_init(&m_lock, 0);
    } 
    ~LockObject(){
        pthread_mutex_destroy(&m_lock);
    } 
    void Lock(){
        pthread_mutex_lock(&m_lock);
    }

    void Unlock(){
        pthread_mutex_unlock(&m_lock);
    }
private :
    LockObject(const LockObject&);
    LockObject& operator=(const LockObject&);

    pthread_mutex_t m_lock;
};

class LockScope
{
public:
    LockScope(LockObject& lock)
        : m_lock(lock)
    {
        m_lock.Lock();
    };
    ~LockScope()
    {
        m_lock.Unlock();
    }
private:
    LockScope(const LockScope&);
    LockScope& operator=(const LockScope&);
    
    LockObject& m_lock;
};
} // nameapace common
} // namespace dk
#endif

#ifndef __EVENTARGS_H__
#define __EVENTARGS_H__

#include <stdint.h>
#include <string>

class EventArgs
{
public:
    EventArgs()
        :handled(0)
    {
    }
    virtual ~EventArgs()
    {
    }

    const char* GetName() const
    {
        return m_name.c_str();
    }

    void SetName(const char* name) const
    {
        m_name = name;
    }

    virtual EventArgs* Clone() const = 0;

    mutable uint32_t handled;
private:
    mutable std::string m_name;
};
#endif


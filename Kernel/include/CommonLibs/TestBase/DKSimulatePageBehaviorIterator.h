#ifndef __KERNEL_TESTS_TESTBASE_DKSIMULATEPAGEBEHAVIORITERATOR_H__
#define __KERNEL_TESTS_TESTBASE_DKSIMULATEPAGEBEHAVIORITERATOR_H__

#include "KernelType.h"

struct DKSimulatePageBehavior
{
    DK_CHAR action;
    DK_UINT pagePos;
};

class DKSimulatePageBehaviorIterator
{
public:
    typedef DKSimulatePageBehavior value_type;
    typedef const DKSimulatePageBehavior* pointer;
    typedef const DKSimulatePageBehavior& reference;
    typedef ptrdiff_t difference_type;
    typedef std::input_iterator_tag iterator_category;

    typedef DKSimulatePageBehaviorIterator self;


    DKSimulatePageBehaviorIterator(const DK_CHAR* p)
        : m_data(p)
        , m_current(p)
    {
    }
    DKSimulatePageBehavior operator*()
    {
        DKSimulatePageBehavior result;
        switch(*m_current)
        {
            case 'l':
            case 'L':
                result.action='l';
                result.pagePos = atoi(m_current + 1);
                break;
            case 'p':
            case 'P':
                result.action = 'p';
                break;
            default:
                result.action = 'n';
        }
        return result;
    }
    self& operator++();
    self operator++(int);
    DK_BOOL operator==(const self& rhs) const
    {
        if (this == &rhs)
        {
            return DK_TRUE;
        }
        if (IsEnd() && rhs.IsEnd())
        {
            return DK_TRUE;
        }
        return m_current == rhs.m_current;
    }
    DK_BOOL operator!=(const self& rhs) const
    {
        return !(*this == rhs);
    }

private:
    DK_BOOL IsEnd() const
    {
        return DK_NULL == m_data || 0 == *m_current;
    }
    const DK_CHAR* m_data;
    const DK_CHAR* m_current;
};
#endif

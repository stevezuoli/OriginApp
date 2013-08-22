#ifndef __QIUSHIBAIKEIMPL_H_
#define __QIUSHIBAIKEIMPL_H_

#include "GUI/UIMessageBox.h"
#include "GUI/UITextSingleLine.h"
#include <semaphore.h>
#include "singleton.h"
#include "dkObjBase.h"
#include "Wifi/WifiManager.h"
#include <time.h>
#include <string>
#include <vector>
#include "GUI/EventSet.h"
#include "GUI/EventArgs.h"

class ThreadParameter
{
public:
    ThreadParameter(UINT32 _uSeqNo, void * _owner)
    {
        m_uSeqNo = _uSeqNo;
        m_pOwner = _owner;
    }

    void* GetOwner()
    {
        return m_pOwner;
    }

    UINT32 GetSeqNo()
    {
        return m_uSeqNo;
    }

private:
    UINT32 m_uSeqNo;
    void* m_pOwner;

};

enum QSBKMessage
{
    GettingContent,
    ContentReady
};

class QiushibaikeContentReadyEventArgs: public EventArgs
{
public:
    virtual EventArgs* Clone() const
    {
        return new QiushibaikeContentReadyEventArgs(*this);
    }
    UINT32 uSeqNo;
};

class QiushibaikeImpl: public EventSet
{
    SINGLETON_H(QiushibaikeImpl);
public:
    static const char* ContentReadyEvent;
    QiushibaikeImpl();
    virtual ~QiushibaikeImpl();
    void SendContent(UINT32 uSeqNo);
    static void* RetriveContent(void *);
    STDMETHOD(StartRetriveContent)(UINT32 uSeqNo);
    std::vector<std::string> GetContent()    {    return m_vContentList;    }
private:

    std::string GetContent(int * piErrCode, UINT32 iPageNum);
    void FilterContent(std::string szContent);
    std::string removeBR(std::string szContent);    
private:
    std::vector<std::string>     m_vContentList;
    pthread_t         m_pThread;
    BOOL             m_bIsWorking;
    sem_t             m_sem;
    
};

#endif



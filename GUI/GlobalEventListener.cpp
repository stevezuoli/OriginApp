#include "GUI/GlobalEventListener.h"
#include <tr1/functional>
#include "BookStore/BookStoreInfoManager.h"
#include "BookStore/OfflineBookStoreManager.h"
#include "DKXManager/DKX/DKXManager.h"
#include "Common/FileManager_DK.h"
#include "TouchAppUI/UIBookReaderContainer.h"

using namespace dk::bookstore;

GlobalEventListener::GlobalEventListener()
{
    SubscribeMessageEvent(
            BookStoreInfoManager::EventPutReadingDataUpdate,
            *(BookStoreInfoManager::GetInstance()),
            std::tr1::bind(
                std::tr1::mem_fn(&GlobalEventListener::OnPutReadingDataUpdate),
                this,
                std::tr1::placeholders::_1));
}

GlobalEventListener* GlobalEventListener::GetInstance()
{
    static GlobalEventListener s_eventListener;
    return &s_eventListener;
}

bool GlobalEventListener::OnPutReadingDataUpdate(const EventArgs& args)
{
    OfflineBookStoreManager::GetInstance()->OnPutReadingDataUpdate(args);

    const ReadingDataSyncUpdateArgs readingDataSyncUpdateArgs =
        (const ReadingDataSyncUpdateArgs&)args;
    if (readingDataSyncUpdateArgs.succeeded 
            && readingDataSyncUpdateArgs.result.HasSyncResult())
    {
        const SyncResult& syncResult = 
            readingDataSyncUpdateArgs.result.GetSyncResult();
        if (syncResult.GetResultCode() != SRC_SUCCEEDED)
        {
            return false;
        }
        
        string bookId = syncResult.GetBookId();
        PCDKFile fileInfo = CDKFileManager::GetFileManager()->GetFileByBookId(bookId.c_str());
        if(!fileInfo)
        {
            return false;
        }
        string bookPath = fileInfo->GetFilePath();
        DKXManager* dkxManager = DKXManager::GetInstance();
        dkxManager->SetReadingDataModified(bookPath, false);
        dkxManager->MarkReadingDataAsUploaded(bookPath);
        dkxManager->SetDataVersion(bookPath, syncResult.GetLatestVersion());
        dkxManager->SaveCurDkx();
        if(m_bookreaderContainer)
        {
            m_bookreaderContainer->SyncDKXBlock();
        }
        return true;
    }

    return false;
}

void GlobalEventListener::RegisterTouchHook(TouchHookFunc func, void* data)
{
    
    for (DK_AUTO(cur, m_touchHooks.begin()); cur != m_touchHooks.end(); ++cur)
    {
        if (cur->func == func && cur->data == data)
        {
            return;
        }
    }
    TouchHookNode node;
    node.func = func;
    node.data = data;
    m_touchHooks.push_back(node);

}

void GlobalEventListener::UnRegisterTouchHook(TouchHookFunc func, void* data)
{
    
    for (DK_AUTO(cur, m_touchHooks.begin()); cur != m_touchHooks.end(); ++cur)
    {
        if (cur->func == func && cur->data == data)
        {
            m_touchHooks.erase(cur);
            return;
        }
    }
}

bool GlobalEventListener::HookTouch(MoveEvent* moveEvent)
{
    bool result = false;
    for (DK_AUTO(cur, m_touchHooks.begin()); cur != m_touchHooks.end(); ++cur)
    {
        if (cur->func(moveEvent, cur->data))
        {
            result = true; 
        }
    }
    return result;
}

void GlobalEventListener::SetBookReaderContainer(UIBookReaderContainer* bookreader)
{
    m_bookreaderContainer = bookreader;
}


///////////////////////////////////////////////////////////////////////
// DkReaderPage.cpp
// Contact: liuhj
// Copyright (c) Duokan Corporation. All rights reserved.
//
////////////////////////////////////////////////////////////////////////

#include <stdlib.h>
#include <time.h>
#include "GUI/UIMessageBox.h"

#include "TouchAppUI/DkReaderPage.h"
#include "TouchAppUI/UISingleTextInputDialog.h"

#include "Utility.h"
#include "ArchiveParser/ArchiveFactory.h"
#include "DKXManager/DKX/DKXManager.h"

#include "I18n/StringManager.h"
#include "../Common/FileManager_DK.h"
#include "MemoryStream.h"
#include "TextLocatorImpl.h"
#include "Utility/FileSystem.h"
#include "Utility/Misc.h"
#include "Framework/CDisplay.h"
#include "SQM.h"

using namespace DkFormat;

DkReaderPage::DkReaderPage ()
	: UIPage() 
	, m_bIsDisposed(FALSE)
	, m_eInitType(READER_PAGE_INIT_NONE)
	, m_uCurIndex(0)
	, m_fIsAllImage(FALSE)
	, m_pPassword(NULL)
	, m_pBookReaderContainer(NULL)
	, m_pImageReaderContainer(NULL)
	, m_eFocusedContainer(NONE_FOCUSED)
{
	SQM::GetInstance()->IncCounter(SQM_ACTION_DKREADERPAGE_OPEN);
}

DkReaderPage::~DkReaderPage ()
{
	Dispose ();
	SQM::GetInstance()->IncCounter(SQM_ACTION_DKREADERPAGE_CLOSE);
    CDisplay::GetDisplay()->SetFullRepaint(true);
}

BOOL DkReaderPage::Initialize (LPCSTR strFilePath, LPCSTR strBookName, int iBookId)
{
    DebugPrintf(DLC_DKREADERPAGE, "%s, %d, %s, %s 1 START", __FILE__,  __LINE__, "DkReaderPage", __FUNCTION__);
	if (NULL == strFilePath || 0 == strlen(strFilePath))
    {
        DebugPrintf(DLC_DKREADERPAGE, "%s, %d, %s, %s end", __FILE__,  __LINE__, "DkReaderPage", __FUNCTION__);
		return FALSE;
	}

	ResetContainers();

	if(CUtility::StringEndWith(strFilePath, ".txt", TRUE) || 
		CUtility::StringEndWith(strFilePath, ".pdf", TRUE) ||
		CUtility::StringEndWith(strFilePath, ".epub", TRUE) ||
		CUtility::StringEndWith(strFilePath, ".mobi", TRUE) ||
		CUtility::StringEndWith(strFilePath, ".prc", TRUE))
	{
		if(NULL == m_pBookReaderContainer)
		{
			m_pBookReaderContainer = new UIBookReaderContainer(this);
			if(NULL == m_pBookReaderContainer)
            {
                DebugPrintf(DLC_DKREADERPAGE, "%s, %d, %s, %s end", __FILE__,  __LINE__, "DkReaderPage", __FUNCTION__);
				return FALSE;
			}

			if(!m_pBookReaderContainer->Initialize(strFilePath, strBookName, iBookId))
            {
                DebugPrintf(DLC_DKREADERPAGE, "%s, %d, %s, %s end", __FILE__,  __LINE__, "DkReaderPage", __FUNCTION__);
				SafeDeletePointer(m_pBookReaderContainer);
				return FALSE;
			}
			m_pBookReaderContainer->MoveWindow(0, 0, m_iWidth, m_iHeight);
		}
		else
		{
			AppendChild(m_pBookReaderContainer);
			if(!m_pBookReaderContainer->Initialize(strFilePath, strBookName, iBookId))
            {
                DebugPrintf(DLC_DKREADERPAGE, "%s, %d, %s, %s end", __FILE__,  __LINE__, "DkReaderPage", __FUNCTION__);
				SafeDeletePointer(m_pBookReaderContainer);
				return FALSE;
			}
		}

		m_pBookReaderContainer->SetFocus(TRUE);
		m_eFocusedContainer = BOOK_READER_CONTAINER;

		if(READER_PAGE_INIT_NONE == m_eInitType)
		{
			m_eInitType = READER_PAGE_INIT_SINGLEFILE;
		}
	}
	else
    {
        DebugPrintf(DLC_DKREADERPAGE, "%s, %d, %s, %s end", __FILE__,  __LINE__, "DkReaderPage", __FUNCTION__);
		return FALSE;
	}

    DebugPrintf(DLC_DKREADERPAGE, "%s, %d, %s, %s end", __FILE__,  __LINE__, "DkReaderPage", __FUNCTION__);
	return TRUE;
}

BOOL DkReaderPage::Initialize (UINT nFileId, INT iBookId, IDkStream *pStream, PCCH pstrExtension, PCCH pPassword)
{
    DebugPrintf(DLC_DKREADERPAGE, "%s, %d, %s, %s 2 START", __FILE__,  __LINE__, "DkReaderPage", __FUNCTION__);
    if(!pStream || NULL == pstrExtension)
    {
        DebugPrintf(DLC_DKREADERPAGE, "%s, %d, %s, %s end", __FILE__,  __LINE__, "DkReaderPage", __FUNCTION__);
		return FALSE;
	}

	ResetContainers();

	if(CUtility::StringEndWith(pstrExtension, ".txt", TRUE) ||
		CUtility::StringEndWith(pstrExtension, ".pdf", TRUE) ||
		CUtility::StringEndWith(pstrExtension, ".epub", TRUE) ||
		CUtility::StringEndWith(pstrExtension, ".mobi", TRUE) ||
		CUtility::StringEndWith(pstrExtension, ".prc", TRUE))
    {
        // TODO
	}
    else if(CUtility::StringEndWith(pstrExtension, ".jpeg", TRUE) ||
		CUtility::StringEndWith(pstrExtension, ".jpg", TRUE) ||
		CUtility::StringEndWith(pstrExtension, ".jpe", TRUE) ||
		CUtility::StringEndWith(pstrExtension, ".png", TRUE) ||
		CUtility::StringEndWith(pstrExtension, ".bmp", TRUE) ||
		CUtility::StringEndWith(pstrExtension, ".gif", TRUE) ||
		CUtility::StringEndWith(pstrExtension, ".tif", TRUE) ||
		CUtility::StringEndWith(pstrExtension, ".tiff", TRUE))
	{
		int totalImage = m_spFileList ? m_spFileList->uLength : 1;
		if(NULL == m_pImageReaderContainer)
		{
			m_pImageReaderContainer = new UIImageReaderContainer(this);
			if(NULL == m_pImageReaderContainer)
            {
                DebugPrintf(DLC_DKREADERPAGE, "%s, %d, %s, %s end", __FILE__,  __LINE__, "DkReaderPage", __FUNCTION__);
				return FALSE;
			}

			m_pImageReaderContainer->MoveWindow(0, 0, m_iWidth, m_iHeight);
			if(!m_pImageReaderContainer->Initialize(nFileId, iBookId, pStream, pPassword, totalImage))
            {
                DebugPrintf(DLC_DKREADERPAGE, "%s, %d, %s, %s end", __FILE__,  __LINE__, "DkReaderPage", __FUNCTION__);
				delete m_pImageReaderContainer;
				m_pImageReaderContainer = NULL;
				return FALSE;
			}
		}
		else
		{
			if(!m_pImageReaderContainer->SetImage(nFileId, iBookId, pStream, pPassword))
            {
                DebugPrintf(DLC_DKREADERPAGE, "%s, %d, %s, %s end", __FILE__,  __LINE__, "DkReaderPage", __FUNCTION__);
				return FALSE;
			}
			AppendChild(m_pImageReaderContainer);
			DebugPrintf(DLC_DKREADERPAGE, "DkReaderPage::Initialize (UINT nFileId, int iBookId, AppendChild(m_pImageReaderContainer); pass");
		}
        m_pImageReaderContainer->SetTotalImage(totalImage);
		DebugPrintf(DLC_DKREADERPAGE, "DkReaderPage::Initialize (UINT nFileId, int iBookId, m_pImageReaderContainer->SetTotalImage pass");
		m_pImageReaderContainer->SetCurImage(m_uCurIndex + 1);
		DebugPrintf(DLC_DKREADERPAGE, "DkReaderPage::Initialize (UINT nFileId, int iBookId, m_pImageReaderContainer->SetCurImage pass");

		m_pImageReaderContainer->SetFocus(TRUE);
		m_eFocusedContainer = IMAGE_READER_CONTAINER;

		if(READER_PAGE_INIT_NONE == m_eInitType)
		{
			m_eInitType = READER_PAGE_INIT_DKBUFFER;
		}

		DebugPrintf(DLC_DKREADERPAGE, "DkReaderPage::Initialize m_pImageReaderContainer pass");
	}
	else
    {
        DebugPrintf(DLC_DKREADERPAGE, "%s, %d, %s, %s end", __FILE__,  __LINE__, "DkReaderPage", __FUNCTION__);
		return FALSE;
    }
    DebugPrintf(DLC_DKREADERPAGE, "%s, %d, %s, %s end", __FILE__,  __LINE__, "DkReaderPage", __FUNCTION__);
	return TRUE;
}

BOOL DkReaderPage::Initialize(SPtr<BookFileList> spFileList, UINT uCurFileIndex, BOOL fIsAllImage, const char * pPassword)
{
    DebugPrintf(DLC_DKREADERPAGE, "%s, %d, %s, %s 3 START", __FILE__,  __LINE__, "DkReaderPage", __FUNCTION__);
	if(!spFileList || NULL == spFileList->pFileList || 0 == spFileList->uLength)
    {
        DebugPrintf(DLC_DKREADERPAGE, "%s, %d, %s, %s end", __FILE__,  __LINE__, "DkReaderPage", __FUNCTION__);
		return FALSE;
	}
	DebugPrintf(DLC_DKREADERPAGE, "DkReaderPage::Initialize entering with bookid=%d, password : %s", spFileList->iBookId, pPassword);
	m_spFileList = spFileList;
	m_fIsAllImage = fIsAllImage;

	INT iIndexTemp = -1; 
	if(m_fIsAllImage)
	{
		iIndexTemp = LoadImgReadingInfo();
	}

	m_uCurIndex = iIndexTemp < 0 ?  uCurFileIndex : (UINT)iIndexTemp;

	if(pPassword)
	{
		if(m_pPassword)
		{
			free(m_pPassword);
			m_pPassword = NULL;
		}

		m_pPassword = (PCHAR)strdup(pPassword);
		if(NULL == m_pPassword)
		{
			return FALSE;
		}
	}

	UINT uTotal = spFileList->uLength;

	UINT uMaxIndex = 0;
	if(uTotal > 0)
	{
		uMaxIndex = uTotal - 1;
	}

	UINT uLimitedIndex = m_uCurIndex;

	while(!InitCurFile())
	{
		SafeFreePointerEx(m_pPassword);
		if(m_uCurIndex < uMaxIndex)
		{
			m_uCurIndex++;
		}
		else
		{
			m_uCurIndex = 0;
		}

		if(m_uCurIndex == uLimitedIndex)
		{
			return FALSE;
		}
	}
	m_eInitType = READER_PAGE_INIT_FILELIST;      

    DebugPrintf(DLC_DKREADERPAGE, "%s, %d, %s, %s end", __FILE__,  __LINE__, "DkReaderPage", __FUNCTION__);
	return TRUE;
}

void DkReaderPage::OnLoad()
{
    DebugPrintf(DLC_DKREADERPAGE, "%s, %d, %s, %s START", __FILE__,  __LINE__, "DkReaderPage", __FUNCTION__);
	UIPage::OnLoad();
	int iBookID = -1;
	if(m_pBookReaderContainer && 
		BOOK_READER_CONTAINER == m_eFocusedContainer)
	{
		m_pBookReaderContainer->OnLoad();
		if (NULL != m_pBookReaderContainer)
			iBookID = m_pBookReaderContainer->GetBookID();
	}
	if(m_pImageReaderContainer &&
		IMAGE_READER_CONTAINER == m_eFocusedContainer)
	{
		m_pImageReaderContainer->OnLoad();
		if (NULL != m_pImageReaderContainer)
			iBookID = m_pImageReaderContainer->GetBookID();        
	}

	if(-1 != iBookID)
	{
		CDKFileManager *pFileManager = CDKFileManager::GetFileManager();
		if (!pFileManager)
        {
            DebugPrintf(DLC_DKREADERPAGE, "%s, %d, %s, %s end", __FILE__,  __LINE__, "DkReaderPage", __FUNCTION__);
			return;
		}
        
        pFileManager->ReadingOrderPlus();
        SystemSettingInfo::GetInstance()->SetInitReadingOrder(pFileManager->GetReadingOrder());
    }    
    DebugPrintf(DLC_DKREADERPAGE, "%s, %d, %s, %s end", __FILE__,  __LINE__, "DkReaderPage", __FUNCTION__);
}

void DkReaderPage::OnUnLoad()
{
    DebugPrintf(DLC_DKREADERPAGE, "%s, %d, %s, %s START", __FILE__,  __LINE__, "DkReaderPage", __FUNCTION__);
	UIPage::OnUnLoad();
	if(m_pBookReaderContainer &&
		BOOK_READER_CONTAINER == m_eFocusedContainer)
	{
		m_pBookReaderContainer->OnUnLoad();
	}

	if(m_pImageReaderContainer &&
		IMAGE_READER_CONTAINER == m_eFocusedContainer)
	{
		m_pImageReaderContainer->OnUnLoad();
	}
    DebugPrintf(DLC_DKREADERPAGE, "%s, %d, %s, %s end", __FILE__,  __LINE__, "DkReaderPage", __FUNCTION__);
}

void DkReaderPage::OnEnter()
{
    DebugPrintf(DLC_DKREADERPAGE, "%s, %d, %s, %s START", __FILE__,  __LINE__, "DkReaderPage", __FUNCTION__);
	int iBookID = -1;
	if(m_pBookReaderContainer &&
		BOOK_READER_CONTAINER == m_eFocusedContainer)
	{
		m_pBookReaderContainer->OnEnter();
		iBookID = m_pBookReaderContainer->GetBookID();
	}

	if(m_pImageReaderContainer &&
		IMAGE_READER_CONTAINER == m_eFocusedContainer)
	{
		m_pImageReaderContainer->OnEnter();
		iBookID = m_pImageReaderContainer->GetBookID();
	}

	if(-1 != iBookID)
	{
		CDKFileManager *pFileManager = CDKFileManager::GetFileManager();
		if (!pFileManager)
        {
            DebugPrintf(DLC_DKREADERPAGE, "%s, %d, %s, %s end", __FILE__,  __LINE__, "DkReaderPage", __FUNCTION__);
			return;
		}
        pFileManager->ReadingOrderPlus();
        SystemSettingInfo::GetInstance()->SetInitReadingOrder(pFileManager->GetReadingOrder());
    }
    UIPage::OnEnter();
    DebugPrintf(DLC_DKREADERPAGE, "%s, %d, %s, %s end", __FILE__,  __LINE__, "DkReaderPage", __FUNCTION__);
}

void DkReaderPage::OnLeave()
{
    DebugPrintf(DLC_DKREADERPAGE, "%s, %d, %s, %s START", __FILE__,  __LINE__, "DkReaderPage", __FUNCTION__);
    if (m_pDialog)
    {
        m_pDialog->EndDialog(IDCANCEL);
    }
	if(m_pBookReaderContainer &&
		BOOK_READER_CONTAINER == m_eFocusedContainer)
	{
		m_pBookReaderContainer->OnLeave();
	}

	if(m_pImageReaderContainer &&
		IMAGE_READER_CONTAINER == m_eFocusedContainer)
	{
		m_pImageReaderContainer->OnLeave();
		if(m_fIsAllImage)
		{
			SaveImgReadingInfo();
		}
	}
	UIPage::OnLeave();
    DebugPrintf(DLC_DKREADERPAGE, "%s, %d, %s, %s end", __FILE__,  __LINE__, "DkReaderPage", __FUNCTION__);
}


HRESULT DkReaderPage::Draw(DK_IMAGE drawingImg)
{
    if(m_pBookReaderContainer &&
        BOOK_READER_CONTAINER == m_eFocusedContainer)
    {
        return m_pBookReaderContainer->Draw(drawingImg);
    }

    if(m_pImageReaderContainer &&
        IMAGE_READER_CONTAINER == m_eFocusedContainer)
    {
        return m_pImageReaderContainer->Draw(drawingImg);
    }
	return UIPage::Draw(drawingImg);
}

BOOL DkReaderPage::OnKeyPress (INT32 iKeyCode)
{
    DebugPrintf(DLC_DIAGNOSTIC, "%s, %d, %s, %s START", __FILE__,  __LINE__, "DkReaderPage", __FUNCTION__);
	BOOL result = UIPage::OnKeyPress(iKeyCode);
	switch(iKeyCode)
	{
	case KEY_LPAGEDOWN:
	case KEY_RPAGEDOWN:
	case KEY_SPACE:
		{
			if(IsCurDownFinished() && READER_PAGE_INIT_FILELIST == m_eInitType)
			{
				UINT uTotalFiles = m_spFileList->uLength;
				if(m_uCurIndex < uTotalFiles - 1)
				{
					m_uCurIndex++;
					if(!InitCurFile())
					{
						SetCurDownFinished();
						SetCurUpFinished();
						UIMessageBox messagebox(this, StringManager::GetPCSTRById(BOOK_OPEN_FAILED), MB_OK);
						messagebox.DoModal(); 
					}
					Repaint();
				}
				return FALSE;
			}
		}
		break;
	case KEY_LPAGEUP:
	case KEY_RPAGEUP:
		{
			if(IsCurUpFinished() && READER_PAGE_INIT_FILELIST == m_eInitType)
			{
				if(m_uCurIndex > 0)
				{
					m_uCurIndex--;
					if(!InitCurFile())
					{
						SetCurDownFinished();
						SetCurUpFinished();
						UIMessageBox messagebox(this, StringManager::GetPCSTRById(BOOK_OPEN_FAILED), MB_OK);
						messagebox.DoModal(); 
					}

					GotoCurLastPage();
					Repaint();
                }
                DebugPrintf(DLC_DIAGNOSTIC, "%s, %d, %s, %s end", __FILE__,  __LINE__, "DkReaderPage", __FUNCTION__);
				return FALSE;
			}
		}
		break;
	default:
		break;
	}

	if (result || KEY_BACK == iKeyCode || KEY_PTRSC == iKeyCode || KEY_PTRSC_K4NT == iKeyCode)
	{
		if(NULL != m_pBookReaderContainer &&
			BOOK_READER_CONTAINER == m_eFocusedContainer)
		{
			m_pBookReaderContainer->UpdateAutoPageDownStatus(TRUE);
		}
	}

    DebugPrintf(DLC_DIAGNOSTIC, "%s, %d, %s, %s end", __FILE__,  __LINE__, "DkReaderPage", __FUNCTION__);
	return result;
}

void DkReaderPage::GotoBookmark(ICT_ReadingDataItem *pBookmark)
{
    DebugPrintf(DLC_DKREADERPAGE, "%s, %d, %s, %s START", __FILE__,  __LINE__, "DkReaderPage", __FUNCTION__);
	if(m_pBookReaderContainer)
	{
		m_pBookReaderContainer->GotoBookmark(pBookmark);
    }
    DebugPrintf(DLC_DKREADERPAGE, "%s, %d, %s, %s end", __FILE__,  __LINE__, "DkReaderPage", __FUNCTION__);
}

void DkReaderPage::OnTTSMessage(SNativeMessage* msg)
{
    DebugPrintf(DLC_DKREADERPAGE, "%s, %d, %s, %s START", __FILE__,  __LINE__, "DkReaderPage", __FUNCTION__);
	if(m_pBookReaderContainer)
	{
		m_pBookReaderContainer->OnTTSMessage(msg);
    }
    DebugPrintf(DLC_DKREADERPAGE, "%s, %d, %s, %s end", __FILE__,  __LINE__, "DkReaderPage", __FUNCTION__);
}

BOOL DkReaderPage::GotoImgPageNo(INT iPageNo)
{
    DebugPrintf(DLC_DKREADERPAGE, "%s, %d, %s, %s START", __FILE__,  __LINE__, "DkReaderPage", __FUNCTION__);
	if(READER_PAGE_INIT_FILELIST != m_eInitType && m_fIsAllImage)
    {
        DebugPrintf(DLC_DKREADERPAGE, "%s, %d, %s, %s READER_PAGE_INIT_FILELIST != m_eInitType && m_fIsAllImage", __FILE__,  __LINE__, "DkReaderPage", __FUNCTION__);
		return FALSE;
	}

	UINT uTotalPage = m_spFileList->uLength;
	UINT uOldCurIndex = m_uCurIndex;

	if(iPageNo <= 0)
	{
		iPageNo = 1;
	}
	else if(iPageNo > (INT)uTotalPage)
	{
		iPageNo = uTotalPage; 
	}

	if((UINT)(iPageNo - 1) == m_uCurIndex)
    {
        DebugPrintf(DLC_DKREADERPAGE, "%s, %d, %s, %s (UINT)(iPageNo - 1) == m_uCurIndex", __FILE__,  __LINE__, "DkReaderPage", __FUNCTION__);
		return TRUE;
	}
	else
	{
		m_uCurIndex = iPageNo - 1;
	}

	DebugPrintf(DLC_DKREADERPAGE, "DkReaderPage::GotoImgPageNo m_uCurIndex %d", m_uCurIndex);
	if(!InitCurFile())
    {
        DebugPrintf(DLC_DKREADERPAGE, "%s, %d, %s, %s !InitCurFile()", __FILE__,  __LINE__, "DkReaderPage", __FUNCTION__);
		m_uCurIndex = uOldCurIndex;
		if(!InitCurFile())
		{
			UIMessageBox messagebox(this, StringManager::GetPCSTRById(BOOK_OPEN_FAILED), MB_OK);
			messagebox.DoModal(); 
		}
		Repaint();
		return FALSE;
	}
    Repaint();
    DebugPrintf(DLC_DKREADERPAGE, "%s, %d, %s, %s end", __FILE__,  __LINE__, "DkReaderPage", __FUNCTION__);
	return TRUE;
}

BOOL DkReaderPage::IsCurDownFinished()
{
    DebugPrintf(DLC_DIAGNOSTIC, "%s, %d, %s, %s START", __FILE__,  __LINE__, "DkReaderPage", __FUNCTION__);
	if(NULL != m_pBookReaderContainer &&
		BOOK_READER_CONTAINER == m_eFocusedContainer)
    {
        DebugPrintf(DLC_DIAGNOSTIC, "%s, %d, %s, %s end m_pBookReaderContainer", __FILE__,  __LINE__, "DkReaderPage", __FUNCTION__);
		return m_pBookReaderContainer->IsPageDownFinished();
	}

	if(NULL != m_pImageReaderContainer &&
		IMAGE_READER_CONTAINER == m_eFocusedContainer)
    {
        DebugPrintf(DLC_DIAGNOSTIC, "%s, %d, %s, %s end m_pImageReaderContainer", __FILE__,  __LINE__, "DkReaderPage", __FUNCTION__);
		return m_pImageReaderContainer->IsPageDownFinished();
	}

    DebugPrintf(DLC_DIAGNOSTIC, "%s, %d, %s, %s end return FALSE", __FILE__,  __LINE__, "DkReaderPage", __FUNCTION__);
	return FALSE;
}

BOOL DkReaderPage::IsCurUpFinished()
{
    DebugPrintf(DLC_DKREADERPAGE, "%s, %d, %s, %s START", __FILE__,  __LINE__, "DkReaderPage", __FUNCTION__);
	if(NULL != m_pBookReaderContainer &&  
		BOOK_READER_CONTAINER == m_eFocusedContainer)
    {
        DebugPrintf(DLC_DKREADERPAGE, "%s, %d, %s, %s end m_pBookReaderContainer", __FILE__,  __LINE__, "DkReaderPage", __FUNCTION__);
		return m_pBookReaderContainer->IsPageUpFinished();
	}

	if(NULL != m_pImageReaderContainer &&
		IMAGE_READER_CONTAINER == m_eFocusedContainer)
    {
        DebugPrintf(DLC_DKREADERPAGE, "%s, %d, %s, %s end m_pImageReaderContainer", __FILE__,  __LINE__, "DkReaderPage", __FUNCTION__);
		return m_pImageReaderContainer->IsPageUpFinished();
    }

    DebugPrintf(DLC_DKREADERPAGE, "%s, %d, %s, %s end return FALSE", __FILE__,  __LINE__, "DkReaderPage", __FUNCTION__);
	return FALSE;
}

void DkReaderPage::SetCurDownFinished()
{
    DebugPrintf(DLC_DIAGNOSTIC, "%s, %d, %s, %s START", __FILE__,  __LINE__, "DkReaderPage", __FUNCTION__);
	if(NULL != m_pBookReaderContainer &&
		BOOK_READER_CONTAINER == m_eFocusedContainer)
	{
		m_pBookReaderContainer->SetPageDownFinished();
	}

	if(NULL != m_pImageReaderContainer &&
		IMAGE_READER_CONTAINER == m_eFocusedContainer)
	{
		m_pImageReaderContainer->SetPageDownFinished();
    }
    DebugPrintf(DLC_DIAGNOSTIC, "%s, %d, %s, %s end", __FILE__,  __LINE__, "DkReaderPage", __FUNCTION__);
}

void DkReaderPage::SetCurUpFinished()
{
    DebugPrintf(DLC_DIAGNOSTIC, "%s, %d, %s, %s START", __FILE__,  __LINE__, "DkReaderPage", __FUNCTION__);
	if(NULL != m_pBookReaderContainer &&  
		BOOK_READER_CONTAINER == m_eFocusedContainer)
	{
		m_pBookReaderContainer->SetPageUpFinished();
	}

	if(NULL != m_pImageReaderContainer &&
		IMAGE_READER_CONTAINER == m_eFocusedContainer)
	{
		m_pImageReaderContainer->SetPageUpFinished();
    }
    DebugPrintf(DLC_DIAGNOSTIC, "%s, %d, %s, %s end", __FILE__,  __LINE__, "DkReaderPage", __FUNCTION__);
}

BOOL DkReaderPage::GotoCurLastPage()
{
    DebugPrintf(DLC_DKREADERPAGE, "%s, %d, %s, %s START", __FILE__,  __LINE__, "DkReaderPage", __FUNCTION__);
	if(NULL != m_pBookReaderContainer &&
		BOOK_READER_CONTAINER == m_eFocusedContainer)
    {
        DebugPrintf(DLC_DKREADERPAGE, "%s, %d, %s, %s m_pBookReaderContainer", __FILE__,  __LINE__, "DkReaderPage", __FUNCTION__);
		return m_pBookReaderContainer->GotoLastPage();
	}

	if(NULL != m_pImageReaderContainer &&
		IMAGE_READER_CONTAINER == m_eFocusedContainer)
    {
        DebugPrintf(DLC_DKREADERPAGE, "%s, %d, %s, %s m_pImageReaderContainer", __FILE__,  __LINE__, "DkReaderPage", __FUNCTION__);
		return m_pImageReaderContainer->GotoLastPage();
	}

    DebugPrintf(DLC_DKREADERPAGE, "%s, %d, %s, %s END return false", __FILE__,  __LINE__, "DkReaderPage", __FUNCTION__);
	return FALSE;
}

void DkReaderPage::Dispose ()
{
    DebugPrintf(DLC_DKREADERPAGE, "%s, %d, %s, %s START", __FILE__,  __LINE__, "DkReaderPage", __FUNCTION__);
	UIPage::Dispose ();
	if (m_bIsDisposed)
	{
		return;
	}

	m_bIsDisposed = TRUE;

    Finalize ();
    DebugPrintf(DLC_DKREADERPAGE, "%s, %d, %s, %s END", __FILE__,  __LINE__, "DkReaderPage", __FUNCTION__);
}

void DkReaderPage::Finalize ()
{
    DebugPrintf(DLC_DKREADERPAGE, "%s, %d, %s, %s START", __FILE__,  __LINE__, "DkReaderPage", __FUNCTION__);
	ReleaseContainers();

	m_eInitType = READER_PAGE_INIT_NONE;
	m_uCurIndex = 0;

	if(m_pPassword)
	{
		free(m_pPassword);
		m_pPassword = NULL;
    }
    DebugPrintf(DLC_DKREADERPAGE, "%s, %d, %s, %s END", __FILE__,  __LINE__, "DkReaderPage", __FUNCTION__);
}

void DkReaderPage::ResetContainers()
{
    DebugPrintf(DLC_DKREADERPAGE, "%s, %d, %s, %s START", __FILE__,  __LINE__, "DkReaderPage", __FUNCTION__);
	ClearFocus();
	if(m_pBookReaderContainer &&
		BOOK_READER_CONTAINER == m_eFocusedContainer)
	{
		m_pBookReaderContainer->ResetContainer();
		RemoveChild(m_pBookReaderContainer, false);
	}

	if(m_pImageReaderContainer &&
		IMAGE_READER_CONTAINER == m_eFocusedContainer)
	{
		m_pImageReaderContainer->ResetContainer();
		RemoveChild(m_pImageReaderContainer, false);
    }
    DebugPrintf(DLC_DKREADERPAGE, "%s, %d, %s, %s END", __FILE__,  __LINE__, "DkReaderPage", __FUNCTION__);
}

void DkReaderPage::ReleaseContainers()
{
    DebugPrintf(DLC_DKREADERPAGE, "%s, %d, %s, %s START", __FILE__,  __LINE__, "DkReaderPage", __FUNCTION__);
	if(m_pBookReaderContainer)
	{
		delete m_pBookReaderContainer;
		m_pBookReaderContainer = NULL;
	}
	if(m_pImageReaderContainer)
	{
		delete m_pImageReaderContainer;
		m_pImageReaderContainer = NULL;
    }
    DebugPrintf(DLC_DKREADERPAGE, "%s, %d, %s, %s END", __FILE__,  __LINE__, "DkReaderPage", __FUNCTION__);
}

BOOL DkReaderPage::InitCurFile()
{
    DebugPrintf(DLC_DIAGNOSTIC, "%s, %d, %s, %s START", __FILE__,  __LINE__, "DkReaderPage", __FUNCTION__);
	if(!m_spFileList)
    {
        DebugPrintf(DLC_DIAGNOSTIC, "%s, %d, %s, %s !m_spFileList", __FILE__,  __LINE__, "DkReaderPage", __FUNCTION__);
		return FALSE;
	}

	CDKFileManager *pFileManager = CDKFileManager::GetFileManager();
	if(NULL == pFileManager)
    {
        DebugPrintf(DLC_DIAGNOSTIC, "%s, %d, %s, %s NULL == pFileManager", __FILE__,  __LINE__, "DkReaderPage", __FUNCTION__);
		return FALSE;
	}

	INT iBookId = m_spFileList->iBookId;
	CDKFile *pDkFile = pFileManager->GetFileById(iBookId);
	if(NULL == pDkFile)
    {
        DebugPrintf(DLC_DIAGNOSTIC, "%s, %d, %s, %s NULL == pDkFile", __FILE__,  __LINE__, "DkReaderPage", __FUNCTION__);
		return FALSE;
	}

	DkFileFormat format = pDkFile->GetFileFormat();
	if(format != DFF_ZipFile && format != DFF_RoshalArchive )
	{
		DebugPrintf(DLC_DIAGNOSTIC, "DkReaderPage::InitCurFile end3 with bookid=%d!",iBookId ); 
		return FALSE;
	}

	PCHAR pExtension = NULL;
    IDkStream *pStream = NULL;
	UINT uFileId = 0;
    if (GetAchiveFileContent(pDkFile, pStream, &pExtension, &uFileId) &&
        Initialize(uFileId, iBookId, pStream, pExtension, m_pPassword))
    {
        DebugPrintf(DLC_DIAGNOSTIC, "%s, %d, %s, %s end return TRUE", __FILE__,  __LINE__, "DkReaderPage", __FUNCTION__);
        return TRUE;
    }

    DebugPrintf(DLC_DIAGNOSTIC, "%s, %d, %s, %s end return FALSE", __FILE__,  __LINE__, "DkReaderPage", __FUNCTION__);
	return FALSE;
}

BOOL DkReaderPage::GetAchiveFileContent(CDKFile *pDkFile, IDkStream *&pStream, CHAR **ppExtension,  PUINT puFileId)
{
    DebugPrintf(DLC_DKREADERPAGE, "%s, %d, %s, %s START", __FILE__,  __LINE__, "DkReaderPage", __FUNCTION__);
	if(NULL == pDkFile ||NULL == ppExtension ||  NULL == puFileId)
	{
        DebugPrintf(DLC_DKREADERPAGE, "%s, %d, %s, %s NULL == pDkFile ||NULL == ppExtension ||  NULL == puFileId", __FILE__,  __LINE__, "DkReaderPage", __FUNCTION__);
        return FALSE;
	}

	if(!m_spFileList || NULL == m_spFileList->pFileList || 0 == m_spFileList->uLength)
	{
        DebugPrintf(DLC_DKREADERPAGE, "%s, %d, %s, %s !m_spFileList || NULL == m_spFileList->pFileList || 0 == m_spFileList->uLength", __FILE__,  __LINE__, "DkReaderPage", __FUNCTION__);
		return FALSE;
	}

	DkFileFormat format = pDkFile->GetFileFormat();
	if(format != DFF_ZipFile && format != DFF_RoshalArchive)
    {
        DebugPrintf(DLC_DKREADERPAGE, "%s, %d, %s, %s format != DFF_ZipFile && format != DFF_RoshalArchive", __FILE__,  __LINE__, "DkReaderPage", __FUNCTION__);
		return FALSE;
	}

	IArchiverParser *pArchiver = NULL;
	HRESULT hr = CArchiveFactory::CreateArchiverInstance((const char*)(pDkFile->GetFilePath()), &pArchiver);
	if(FAILED(hr) || NULL == pArchiver)
    {
        DebugPrintf(DLC_DKREADERPAGE, "%s, %d, %s, %s FAILED(hr) || NULL == pArchiver", __FILE__,  __LINE__, "DkReaderPage", __FUNCTION__);
		return FALSE;
	}

	ArchiveEncryption isEncryption = ARCHIVE_NO_ENCRYPTION;
	hr = pArchiver->IsEncryption(&isEncryption);
	if(SUCCEEDED(hr) && ARCHIVE_NO_ENCRYPTION != isEncryption)
    {
        DebugPrintf(DLC_DKREADERPAGE, "%s, %d, %s, %s SUCCEEDED(hr) && ARCHIVE_NO_ENCRYPTION != isEncryption", __FILE__,  __LINE__, "DkReaderPage", __FUNCTION__);
		if(NULL == m_pPassword)
        {
            DebugPrintf(DLC_DKREADERPAGE, "%s, %d, %s, %s NULL == m_pPassword", __FILE__,  __LINE__, "DkReaderPage", __FUNCTION__);
			delete pArchiver;
			pArchiver = NULL;
			return FALSE;
		}

		pArchiver->SetPassword(m_pPassword);
	}

	UINT uFileId = m_spFileList->pFileList[m_uCurIndex].uFileId;
	DebugPrintf(DLC_DKREADERPAGE, "DkReaderPage::GetAchiveFileContent uFileId %d", uFileId);
	DebugPrintf(DLC_DKREADERPAGE, "DkReaderPage::GetArchiveFileContent password %s", m_pPassword);

	if(FAILED(pArchiver->GetFileStreamByIndex((INT)uFileId, &pStream)))
    {
        DebugPrintf(DLC_DKREADERPAGE, "%s, %d, %s, %s FAILED(pArchiver->GetFileStreamByIndex((INT)uFileId, &pStream))", __FILE__,  __LINE__, "DkReaderPage", __FUNCTION__);
		delete pArchiver;
		pArchiver = NULL;
		return FALSE;
	}

	if(pArchiver)
    {
		delete pArchiver;
		pArchiver = NULL;
	}


	*puFileId = uFileId;
	*ppExtension = m_spFileList->pFileList[m_uCurIndex].pExtension;
	DebugPrintf(DLC_DKREADERPAGE, " DkReaderPage::GetAchiveFileContent *ppExtension %s\n", *ppExtension);

	DebugPrintf(DLC_DKREADERPAGE, " DkReaderPage::GetAchiveFileContent pass\n");
	return TRUE;
}

LPCSTR DkReaderPage::GetCurBookPath()
{
    DebugPrintf(DLC_DKREADERPAGE, "%s, %d, %s, %s START", __FILE__,  __LINE__, "DkReaderPage", __FUNCTION__);
	if(!m_spFileList)
    {
        DebugPrintf(DLC_DKREADERPAGE, "%s, %d, %s, %s !m_spFileList", __FILE__,  __LINE__, "DkReaderPage", __FUNCTION__);
		return NULL;
	}

	INT iBookId = m_spFileList->iBookId;

	CDKFileManager *pFileManager = CDKFileManager::GetFileManager();
	if(NULL == pFileManager)
    {
        DebugPrintf(DLC_DKREADERPAGE, "%s, %d, %s, %s NULL == pFileManager", __FILE__,  __LINE__, "DkReaderPage", __FUNCTION__);
		return NULL;
	}

	CDKFile *pDkFile = pFileManager->GetFileById(iBookId);
	if(NULL == pDkFile)
    {
        DebugPrintf(DLC_DKREADERPAGE, "%s, %d, %s, %s NULL == pDkFile", __FILE__,  __LINE__, "DkReaderPage", __FUNCTION__);
		return NULL;
	}

    DebugPrintf(DLC_DKREADERPAGE, "%s, %d, %s, %s end", __FILE__,  __LINE__, "DkReaderPage", __FUNCTION__);
	return pDkFile->GetFilePath();
}

INT DkReaderPage::LoadImgReadingInfo()
{
    DebugPrintf(DLC_DKREADERPAGE, "%s, %d, %s, %s START", __FILE__,  __LINE__, "DkReaderPage", __FUNCTION__);
	if(!m_spFileList || NULL == m_spFileList->pFileList || 0 == m_spFileList->uLength)
    {
        DebugPrintf(DLC_DKREADERPAGE, "%s, %d, %s, %s !m_spFileList || NULL == m_spFileList->pFileList || 0 == m_spFileList->uLength", __FILE__,  __LINE__, "DkReaderPage", __FUNCTION__);
		return -1;
	}

	INT _iBookId = m_spFileList->iBookId;

	CDKFileManager* _pclsFileManager = CDKFileManager::GetFileManager();
	if(!_pclsFileManager)
    {
        DebugPrintf(DLC_DKREADERPAGE, "%s, %d, %s, %s !_pclsFileManager", __FILE__,  __LINE__, "DkReaderPage", __FUNCTION__);
		return -1;
	}
	PCDKFile _pFile = _pclsFileManager->GetFileById(_iBookId);
	if(!_pFile)
    {
        DebugPrintf(DLC_DKREADERPAGE, "%s, %d, %s, %s !_pFile", __FILE__,  __LINE__, "DkReaderPage", __FUNCTION__);
		return -1;
	}

	string _strFilePath(_pFile->GetFilePath());

	DKXManager *_pclsDKXManager = DKXManager::GetInstance();
    _pclsDKXManager->Reset();
	ICT_ReadingDataItem* _pclsReadingProgress = DKXReadingDataItemFactory::CreateReadingDataItem(ICT_ReadingDataItem::PROGRESS);

	if(!_pclsReadingProgress)
    {
        DebugPrintf(DLC_DKREADERPAGE, "%s, %d, %s, %s !_pclsReadingProgress", __FILE__,  __LINE__, "DkReaderPage", __FUNCTION__);
		return -1;
	}
	_pclsDKXManager->FetchProgress(_strFilePath, _pclsReadingProgress);
	int _iCurIndex = _pclsReadingProgress->GetBeginPos().GetChapterIndex();
	DebugPrintf(DLC_DKREADERPAGE, "DkReaderPage::LoadImgReadingInfo load progress current chapter=%d for file %s", _iCurIndex, _strFilePath.c_str());
    SafeDeletePointerEx(_pclsReadingProgress);
    DebugPrintf(DLC_DKREADERPAGE, "%s, %d, %s, %s end", __FILE__,  __LINE__, "DkReaderPage", __FUNCTION__);
	return _iCurIndex-1;
}

void DkReaderPage::SaveImgReadingInfo()
{
    if (NULL == m_pImageReaderContainer || NULL == m_pImageReaderContainer->GetDKXBlock())
    {
        return;
    }
    DKXBlock* dkxBlock = m_pImageReaderContainer->GetDKXBlock();
    DebugPrintf(DLC_DKREADERPAGE, "%s, %d, %s, %s START", __FILE__,  __LINE__, "DkReaderPage", __FUNCTION__);
	if(!m_spFileList || NULL == m_spFileList->pFileList || 0 == m_spFileList->uLength)
    {
        DebugPrintf(DLC_DKREADERPAGE, "%s, %d, %s, %s !m_spFileList || NULL == m_spFileList->pFileList || 0 == m_spFileList->uLength", __FILE__,  __LINE__, "DkReaderPage", __FUNCTION__);
		return;
	}

	INT _iBookId = m_spFileList->iBookId;

	CDKFileManager* _pclsFileManager = CDKFileManager::GetFileManager();
	if(!_pclsFileManager)
    {
        DebugPrintf(DLC_DKREADERPAGE, "%s, %d, %s, %s !_pclsFileManager", __FILE__,  __LINE__, "DkReaderPage", __FUNCTION__);
		return;
	}
	PCDKFile _pFile = _pclsFileManager->GetFileById(_iBookId);
	if(!_pFile)
    {
        DebugPrintf(DLC_DKREADERPAGE, "%s, %d, %s, %s !_pFile", __FILE__,  __LINE__, "DkReaderPage", __FUNCTION__);
		return;
	}

	string _strFilePath(_pFile->GetFilePath());


	ICT_ReadingDataItem* _pclsReadingProgress = DKXReadingDataItemFactory::CreateReadingDataItem(ICT_ReadingDataItem::PROGRESS);

	if(!_pclsReadingProgress)
    {
        DebugPrintf(DLC_DKREADERPAGE, "%s, %d, %s, %s !_pclsReadingProgress", __FILE__,  __LINE__, "DkReaderPage", __FUNCTION__);
		return;
	}


	UINT uImgCount = m_spFileList->uLength;

	CT_RefPos _clsStartPos(m_uCurIndex+1, 0,0, -1), _clsEndPos(uImgCount, 0, 0, -1);
	_pclsReadingProgress->SetBeginPos(_clsStartPos);
	_pclsReadingProgress->SetEndPos(_clsEndPos);

	time_t _tTime;
	time(&_tTime);
	string _strTime  = CUtility::TransferTimeToString(_tTime);
	_pclsReadingProgress->SetCreateTime(_strTime);

	dkxBlock->SetLastReadTime(_strTime);
	dkxBlock->SetReadingOrder(_pclsFileManager->GetReadingOrder());

	_pFile->SetFileLastReadTime(CUtility::TransferStringToTime(_strTime));
    _pFile->SetFileReadingOrder(_pclsFileManager->GetReadingOrder());
    if (uImgCount > 0)
    {
        int _iProgress = ((m_uCurIndex + 1) * 100) / uImgCount;
        if (_iProgress >= 0)
        {
            _pclsReadingProgress->SetPercent(_iProgress);
            _pFile->SetFilePlayProcess(_iProgress);
        }
    }

	DebugPrintf(DLC_DKREADERPAGE, "DkReaderPage::SaveImgReadingInfo save progress[current chapter=%d, total chapter=%d] for file %s", m_uCurIndex+1, uImgCount, _pFile->GetFilePath());

	dkxBlock->SetProgress(_pclsReadingProgress);
    dkxBlock->Serialize();

	SafeDeletePointerEx(_pclsReadingProgress);

    DebugPrintf(DLC_DKREADERPAGE, "%s, %d, %s, %s end", __FILE__,  __LINE__, "DkReaderPage", __FUNCTION__);
}

INT DkReaderPage::GetImgReadingIndex(IBookmark *pBookmark)
{
    DebugPrintf(DLC_DKREADERPAGE, "%s, %d, %s, %s START", __FILE__,  __LINE__, "DkReaderPage", __FUNCTION__);
	if(NULL == pBookmark)
    {
        DebugPrintf(DLC_DKREADERPAGE, "%s, %d, %s, %s NULL == pBookmark", __FILE__,  __LINE__, "DkReaderPage", __FUNCTION__);
		return -1;
	}

	IBookLocator *pLocator = NULL;

	if(FAILED(pBookmark->GetLocator(&pLocator, 0)) || NULL == pLocator)
    {
        DebugPrintf(DLC_DKREADERPAGE, "%s, %d, %s, %s FAILED(pBookmark->GetLocator(&pLocator, 0)) || NULL == pLocator", __FILE__,  __LINE__, "DkReaderPage", __FUNCTION__);
		if(pLocator)
		{
			delete pLocator;
			pLocator = NULL;
		}
		return -1;
	}

	int iLocatorCount = 0;
	pBookmark->GetLocatorCount(&iLocatorCount);
	if(iLocatorCount == 2)
    {
        DebugPrintf(DLC_DKREADERPAGE, "%s, %d, %s, %s iLocatorCount == 2", __FILE__,  __LINE__, "DkReaderPage", __FUNCTION__);
		IBookLocator *pTmpLocator = NULL;
		if(FAILED(pBookmark->GetLocator(&pTmpLocator, 1)) || NULL == pTmpLocator)
        {
            DebugPrintf(DLC_DKREADERPAGE, "%s, %d, %s, %s FAILED(pBookmark->GetLocator(&pTmpLocator, 1)) || NULL == pTmpLocator", __FILE__,  __LINE__, "DkReaderPage", __FUNCTION__);
			if(pTmpLocator)
			{
				delete pTmpLocator;
				pTmpLocator = NULL;
            }
			return -1;
		}

		int iRet = 0;
		if(SUCCEEDED(pLocator->LocatorCmp(pTmpLocator, &iRet)))
        {
            DebugPrintf(DLC_DKREADERPAGE, "%s, %d, %s, %s SUCCEEDED(pLocator->LocatorCmp(pTmpLocator, &iRet))", __FILE__,  __LINE__, "DkReaderPage", __FUNCTION__);
			if(iRet < 0)
			{
				delete pTmpLocator;
			}
			else
			{
				delete pLocator;
				pLocator = pTmpLocator;
			}
		}
		else
		{
			delete pTmpLocator;
		}
	}

	BookLocatorType eType = FILE_LOCATOR_BASE;
	if(FAILED(pLocator->GetType(eType)) || eType != FILE_LOCATOR_BASE)
    {
        DebugPrintf(DLC_DKREADERPAGE, "%s, %d, %s, %s FAILED(pLocator->GetType(eType)) || eType != FILE_LOCATOR_BASE", __FILE__,  __LINE__, "DkReaderPage", __FUNCTION__);
        delete pLocator;
		return -1;
	}

	CTextLocatorImpl *pTextLocator = (CTextLocatorImpl *)pLocator;
	INT iCurIndex = 0;
	HRESULT hr = pTextLocator->GetBlockId(iCurIndex);
	if (FAILED(hr))
	{
		iCurIndex = -1;
	}

    delete pLocator;
    DebugPrintf(DLC_DKREADERPAGE, "%s, %d, %s, %s end", __FILE__,  __LINE__, "DkReaderPage", __FUNCTION__);
	return iCurIndex;
}

UINT DkReaderPage::GetCurFileId()
{
    DebugPrintf(DLC_DKREADERPAGE, "%s, %d, %s, %s START", __FILE__,  __LINE__, "DkReaderPage", __FUNCTION__);
	if(m_pBookReaderContainer)
    {
        DebugPrintf(DLC_DKREADERPAGE, "%s, %d, %s, %s if(m_pBookReaderContainer)", __FILE__,  __LINE__, "DkReaderPage", __FUNCTION__);
		return m_pBookReaderContainer->GetCurFileId();
	}
	else if(m_pImageReaderContainer)
    {
        DebugPrintf(DLC_DKREADERPAGE, "%s, %d, %s, %s else if(m_pImageReaderContainer)", __FILE__,  __LINE__, "DkReaderPage", __FUNCTION__);
		return m_pImageReaderContainer->GetCurFileId();
	}
	else
    {
        DebugPrintf(DLC_DKREADERPAGE, "%s, %d, %s, %s else", __FILE__,  __LINE__, "DkReaderPage", __FUNCTION__);
		return knMainFileId;
	}
}

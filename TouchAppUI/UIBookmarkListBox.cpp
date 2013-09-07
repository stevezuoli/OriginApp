////////////////////////////////////////////////////////////////////////
// UIBookmarkListBox.cpp
// Contact: zhangjd
// Copyright (c) Duokan Corporation. All rights reserved.
////////////////////////////////////////////////////////////////////////

#include <vector>
#include "TouchAppUI/UIBookmarkListBox.h"
#include "TouchAppUI/UIBookmarkListItem.h"
#include "TouchAppUI/BookOpenManager.h"
#include "CommonUI/CPageNavigator.h"
#include "TouchAppUI/DkReaderPage.h"
#include "TouchAppUI/UISingleTextInputDialog.h"
#include "Common/FileManager_DK.h"
#include "Common/File_DK.h"
#include "Framework/CDisplay.h"
#include "Framework/CNativeThread.h"
#include "GUI/UIMessageBox.h"
#include "GUI/UISizer.h"
#include "ArchiveParser/ArchiveFactory.h"
#include "Model/BookmarkItemModelImpl.h"
#include "I18n/StringManager.h"
#include "MemoryStream.h"
#include "DKXManager/DKX/DKXManager.h"
#include "DKXManager/DKX/DKXReadingDataItemFactory.h"
#include "BookInfoEntity.h"
#include "BookInfoManager.h"
#include "dkWinError.h"
#include "Bookmark.h"
#include "Utility.h"
#include "drivers/DeviceInfo.h"
#include "BookReader/IBookReader.h"
#include "Common/WindowsMetrics.h"
using namespace WindowsMetrics;

using namespace DkFormat;
using namespace std;

UIBookmarkListBox::UIBookmarkListBox(
        int iBookId,
        const char *pPassword,
        DKXBlock* dkxBlock,
        IBookReader* pBookReader,
        int iType)
	: m_iCurModelIndex(0)
	, m_bNeedUpdate(true)
	, m_iBookId(iBookId)
	, m_pPassword(NULL)
    , m_listType(iType)
    , m_pBookReader(pBookReader)
    , m_dkxBlock(dkxBlock)
{ 
	if(pPassword)
	{
		m_pPassword = strdup(pPassword);
	}

    int itemPerPage = 4;
    int itemHeight  = GetWindowMetrics(UIBookMarkListItemHeightIndex);
    if (m_listType & UIBookmarkListItem::UIBOOKMARKLISTTYPE_COMMENT)
    {
        itemPerPage = 3;
        itemHeight  = GetWindowMetrics(UIBookCommentListItemHeightIndex);
    }

	UIBookmarkListItem *pItem = NULL;
	for(int i = 0; i < itemPerPage; i++)
	{
		pItem = new UIBookmarkListItem(this, m_pBookReader, m_listType);
		if(NULL == pItem)
		{
			return;
		}
        SetItemHeight(itemHeight);
        if (m_windowSizer)
        {
            pItem->SetMinSize(dkSize(CDisplay::GetDisplay()->GetScreenWidth() - (GetWindowMetrics(UITocDlgHorizonMarginIndex)<<1), itemHeight));
            m_windowSizer->Add(pItem, UISizerFlags().Border(dkRIGHT | dkLEFT, GetWindowMetrics(UITocDlgHorizonMarginIndex)).Expand());
        }
		//pItem->MoveWindow(40, i * m_iItemHeight, 520, itemHeight);
	}
    m_iVisibleItemNum = itemPerPage;
    UpdateList();
}

UIBookmarkListBox::~UIBookmarkListBox()
{
	if(m_pPassword)
	{
		free(m_pPassword);
		m_pPassword = NULL;
	}

    ClearAllChild(true);
	ClearModelList();
}

void UIBookmarkListBox::ClearModelList()
{
	for(size_t i = 0; i < m_allModeList.size(); i++)
	{
		delete m_allModeList[i];
		m_allModeList[i] = NULL;
	}

	m_allModeList.clear();
	m_curModeList.clear();
}

void UIBookmarkListBox::Init()
{
	LoadBookmark();
}

void UIBookmarkListBox::LoadBookmark()
{
	ClearModelList();
    if (NULL == m_dkxBlock)
    {
        return;
    }
    
	int _iBookMarkCount = m_dkxBlock->GetBookmarkCount();
	for(int i = 0; i < _iBookMarkCount; i++)
	{
		ICT_ReadingDataItem *_pclsMarkItem = DKXReadingDataItemFactory::CreateReadingDataItem();

		if(!_pclsMarkItem)
		{
			continue;
		}
		if(!m_dkxBlock->GetBookmarkByIndex(i, _pclsMarkItem))
		{
			DKXReadingDataItemFactory::DestoryReadingDataItem(_pclsMarkItem);
			continue;
		}

		if(_pclsMarkItem)
		{
			CBookmarkItemModelImpl *pTmpModel = new CBookmarkItemModelImpl(_pclsMarkItem, m_iBookId,i);
			if(NULL == pTmpModel)
			{
				DKXReadingDataItemFactory::DestoryReadingDataItem(_pclsMarkItem);
				continue;
			}

			m_allModeList.push_back(pTmpModel);
			m_curModeList.push_back(pTmpModel);
		}
	}
	if(m_allModeList.empty())
	{
		return;
    }
	m_bNeedUpdate = true;
}

void UIBookmarkListBox::InitListItem()
{
	int iModelSize = m_curModeList.size();
	int iItemSize = GetChildrenCount();
	UIBookmarkListItem *pItem = NULL;
	for (int i = 0; i < iItemSize; i++)
    {
		pItem = (UIBookmarkListItem *)GetChildByIndex(i);
		if(i + m_iCurModelIndex < iModelSize)
		{
			if (m_bNeedUpdate)
			{
				pItem->SetBookItemModel(m_curModeList[i + m_iCurModelIndex]);
			}

			pItem->SetVisible(true);
		}
		else
		{
			pItem->SetVisible(false);
		}
	}

    m_bNeedUpdate = false;
    Layout();
}

void UIBookmarkListBox::UpdateList()
{
    Init();
    SetBookmarkListType(m_listType);
    InitListItem();
}

void UIBookmarkListBox::HandlePageDown()
{
	if (m_allModeList.size() > 0)
	{
		m_iCurModelIndex += GetChildrenCount();
		if(m_iCurModelIndex + 1 > (int)m_curModeList.size())
		{
			m_iCurModelIndex = 0;

		}
        m_bNeedUpdate = true;
        InitListItem();
		Repaint();
    }
}

void UIBookmarkListBox::HandlePageUp()
{
	if (m_allModeList.size() > 0)
	{
		m_iCurModelIndex -= GetChildrenCount();
		if(m_iCurModelIndex < 0)
		{
			int iTotalPageNum = GetTotalPage();
			m_iCurModelIndex = (iTotalPageNum - 1) * GetChildrenCount();
        }
        m_bNeedUpdate = true;
        InitListItem();
		Repaint();
    }
}

void UIBookmarkListBox::HandleGoToBookMark(int selectedIndex)
{
    if (selectedIndex < 0 || selectedIndex >= (int)GetChildrenCount() || NULL == m_dkxBlock)
        return;
    UIWindow* selectedWnd = GetChildByIndex(selectedIndex);
	if(selectedWnd && selectedWnd->IsVisible())
    {
        ICT_ReadingDataItem * pReadingDataItem = m_curModeList[m_iCurModelIndex + selectedIndex]->GetBookmark();
        ICT_ReadingDataItem *pclsProgress = DKXReadingDataItemFactory::CreateReadingDataItem(ICT_ReadingDataItem::PROGRESS);
        if(pclsProgress && m_dkxBlock->GetProgress(pclsProgress))
        {
            if (m_pBookReader)
            {
                pclsProgress->SetComicsFrameMode(m_pBookReader->IsComicsFrameMode());
            }
            pclsProgress->SetBeginPos(pReadingDataItem->GetBeginPos());
            m_dkxBlock->SetProgress(pclsProgress);
        }
        SafeDeletePointerEx(pclsProgress);
    }
}

BOOL UIBookmarkListBox::OnKeyPress(INT32 iKeyCode)
{
    return UIWindow::OnKeyPress(iKeyCode);
}

int UIBookmarkListBox::GetCurPageNum()
{
	int iItemNumPerPage = GetChildrenCount();
	if(0 == iItemNumPerPage)
	{
		return 0;
	}

	return (m_iCurModelIndex + iItemNumPerPage) / iItemNumPerPage;
}

int UIBookmarkListBox::GetTotalPage()
{
	int iItemNumPerPage = GetChildrenCount();
	if(0 == iItemNumPerPage)
	{
		return 0;
	}

	return (m_curModeList.size() + iItemNumPerPage - 1) / iItemNumPerPage;
}

int UIBookmarkListBox::GetCurModeIndex()
{
	return m_iCurModelIndex;
}

int UIBookmarkListBox::GetCurBookmarkNum()
{
	return m_curModeList.size();
}

int UIBookmarkListBox::GetClassNum()
{
	int iBookmark = 0;
	int iComment = 0;
	int iSummary = 0;
	for(size_t i = 0; i < m_curModeList.size(); i++)
	{
		if(ICT_ReadingDataItem::COMMENT == m_curModeList[i]->GetBookmarkType())
		{
			iComment = 1;
		}

		if(ICT_ReadingDataItem::BOOKMARK== m_curModeList[i]->GetBookmarkType())
		{
			iBookmark = 1;
		}

		if(ICT_ReadingDataItem::DIGEST== m_curModeList[i]->GetBookmarkType())
		{
			iSummary = 1;
		}

		if(iSummary + iBookmark + iComment > 2)
		{
			break;
		}
	}

	return iSummary + iBookmark + iComment;
}

bool UIBookmarkListBox::DoHandleListTurnPage(bool fKeyUp)
{
    fKeyUp ? HandlePageUp() : HandlePageDown();
    return true;
}

void UIBookmarkListBox::ClearFocus()
{
	for(size_t i = 0; i < GetChildrenCount(); i++)
	{
		GetChildByIndex(i)->SetFocus(false);
	}
}

bool UIBookmarkListBox::RemoveBookmarkFromDKX(int _iBookmarkIndex)
{
    if (NULL == m_dkxBlock)
    {
        return false;
    }
	m_dkxBlock->RemoveBookmarkByIndex(_iBookmarkIndex);
    m_dkxBlock->SetReadingDataModified(true);
    return m_dkxBlock->Serialize();
}

void UIBookmarkListBox::DeleteKeyHander(INT32 deleteIndex)
{
    if(m_curModeList.size() < 1)
    {
        m_iCurModelIndex = 0;
        return;
    }

    if (deleteIndex < 0 || deleteIndex >= (int)GetChildrenCount())
    {
        return;
    }
    
    m_bNeedUpdate = true;
    IBookmarkItemModel *pModel = m_curModeList[m_iCurModelIndex + deleteIndex];
    for(size_t j = 0; j < m_allModeList.size(); j++)
    {
        if(pModel && pModel == m_allModeList[j])
        {
            m_curModeList.erase(m_curModeList.begin() + m_iCurModelIndex + deleteIndex);
            m_allModeList.erase(m_allModeList.begin() + j);
            // RemoveBookmarkFromDKX(pModel->GetBookmarkIndex());
            RemoveBookmarkFromDKX(j);
            break;
        }
    }
    SafeFreePointer(pModel);
    if(m_curModeList.size() < 1)
    {
        m_iCurModelIndex = 0;
    }
	else
	{
		 //当前页中已经删没了的时候
		if(m_iCurModelIndex >= (int)m_curModeList.size())
	    {
	        m_iCurModelIndex = (GetTotalPage() - 1) * GetChildrenCount();
	    }
	}
	ListItemDeleteEvent args(this);
    FireEvent(ListItemDelete, (EventArgs&)args);
}

void UIBookmarkListBox::SetBookmarkListType(int iType)
{
	m_curModeList.clear();
	m_bNeedUpdate = true;
    if (iType > 0)
    {
        for(size_t i = 0; i < m_allModeList.size(); i++)
        {
            ICT_ReadingDataItem::UserDataType bookmarkType = m_allModeList[i]->GetBookmarkType();
            if(((iType & UIBookmarkListItem::UIBOOKMARKLISTTYPE_BOOKMARK) && (bookmarkType == ICT_ReadingDataItem::BOOKMARK))
                || ((iType & UIBookmarkListItem::UIBOOKMARKLISTTYPE_DIGEST) && (bookmarkType == ICT_ReadingDataItem::DIGEST))
                || ((iType & UIBookmarkListItem::UIBOOKMARKLISTTYPE_COMMENT) && (bookmarkType == ICT_ReadingDataItem::COMMENT)))
            {
                m_curModeList.push_back(m_allModeList[i]);
            }
        }
    }
	else//选择所有
	{
		for(size_t i = 0; i < m_allModeList.size(); i++)
		{
			m_curModeList.push_back(m_allModeList[i]);
		}
	}

	//按时间排序  TODO:目前先用冒泡排序,待以后改进
	IBookmarkItemModel *pTmpModel = NULL;
	int iListSize = m_curModeList.size();
	string _strFirst = "";
	string _strSecond = "";
	for(int i = 0; i < iListSize; i++)
	{
		for(int j = 0; j < iListSize - 1 - i; j++)
		{
			_strFirst = m_curModeList[j]->GetCreateTime();
			_strSecond = m_curModeList[j + 1]->GetCreateTime();
			if(_strFirst < _strSecond)
			{
				pTmpModel = m_curModeList[j];
				m_curModeList[j] = m_curModeList[j + 1];
				m_curModeList[j + 1] = pTmpModel;
			}
		}
	}

	m_iCurModelIndex = 0;
}

// iPageNum based 0
bool UIBookmarkListBox::GotoPage(int iPageNum)
{
	if (iPageNum < 0 || iPageNum >= GetTotalPage())
	{
		return FALSE;
	}

	m_iCurModelIndex = iPageNum * GetChildrenCount();
	m_bNeedUpdate = true;
	return TRUE;
}

BOOL UIBookmarkListBox::OpenArchive(INT iBookmarkIndex, CDKFile *pDkFile)
{
	// zip rar 书签书摘的跳转
	IArchiverParser *pArchiver = NULL;
	HRESULT hr = CArchiveFactory::CreateArchiverInstance((const char*)(pDkFile->GetFilePath()), &pArchiver);
	if(FAILED(hr) || NULL == pArchiver)
	{
		return FALSE;
	}

	// 密码处理
	ArchiveEncryption isEncryption = ARCHIVE_NO_ENCRYPTION;
	if(SUCCEEDED(pArchiver->IsEncryption(&isEncryption)) && ARCHIVE_NO_ENCRYPTION != isEncryption)
	{
		// 需要密码
		if(NULL == m_pPassword || strlen(m_pPassword) < 1)
		{
			if(m_pPassword)
			{
				free(m_pPassword);
				m_pPassword = NULL;
			}

			// 1 先从书籍信息里读
			char *pTmpPassword = (char *)(pDkFile->GetFilePassword());
			if(pTmpPassword && strlen(pTmpPassword) > 0)
			{
				//这里pTmpPassword不需要释放
				m_pPassword = strdup(pTmpPassword);
			}
			else// 2 用户自己输入
			{
				// 这里要释放pTmpPassword
				pTmpPassword = NULL;
				UISingleTextInputDialog PwdDlg(GetParent());
				PwdDlg.SetDialogTitle(StringManager::GetStringById(INPUT_PASSWORD));
				PwdDlg.MoveWindow(100, 320, 380, 160);
				if(0 == PwdDlg.DoModal())
				{
					delete pArchiver;
					pArchiver = NULL;
					if(pTmpPassword)
					{
						delete[] pTmpPassword;
						pTmpPassword = NULL;
					}

					return FALSE;
				}

				if(pTmpPassword)
				{
					m_pPassword = strdup(pTmpPassword);
					delete[] pTmpPassword;
					pTmpPassword = NULL;
				}
			}
		}

		pArchiver->SetPassword(m_pPassword);
	}

	if(iBookmarkIndex >= (int)m_curModeList.size())
	{
		delete pArchiver;
		pArchiver = NULL;
		return FALSE;
	}

	UINT nFileId = m_curModeList[iBookmarkIndex]->GetDkpFileId();
	int iFileSize = 0;
	if(pArchiver->GetFileSizeByIndex(nFileId, &iFileSize) != S_OK || iFileSize < 1)
	{
		delete pArchiver;
		pArchiver = NULL;
		UIMessageBox messagebox(this, StringManager::GetStringById(CANNOT_OPEN_BOOK), MB_OK);
		messagebox.DoModal();
		if(m_pPassword)
		{
			m_pPassword[0] = 0;
		}

		return FALSE;
	}

	// 取得书签书摘的文件名.
	char rgFileName[512];
	rgFileName[0] = 0;
	char *pTmp = NULL;
	if(FAILED(pArchiver->GetFileNameByIndex((int)nFileId, &pTmp)) || NULL == pTmp)
	{
		delete pArchiver;
		pArchiver = NULL;
		return FALSE;
	}

	strncpy(rgFileName, pTmp, sizeof(rgFileName) - 1);
	rgFileName[sizeof(rgFileName) - 1] = 0;
	delete[] pTmp;
	pTmp = NULL;


	BOOL fNeedReload = FALSE;
	DkReaderPage *pReader = (DkReaderPage *)CPageNavigator::GetLastPage();
	if (pReader != NULL && pReader->GetCurFileId() != nFileId)
	{
		// 不是同一个文件
		pReader->OnUnLoad();
		fNeedReload = TRUE;
	}

	if(NULL == pReader)
	{
		return FALSE;
	}

	if(fNeedReload)
	{
		IDkStream *pStream = NULL;
		if(pArchiver->GetFileStreamByIndex((int)nFileId, &pStream) != S_OK)
		{
			delete pArchiver;
			pArchiver = NULL;
			UIMessageBox messagebox(this, StringManager::GetStringById(CANNOT_OPEN_BOOK), MB_OK);
			messagebox.DoModal();
			if(m_pPassword)
			{
				m_pPassword[0] = 0;
            }
            SafeDeletePointer(pReader);

			return FALSE;
		}

		delete pArchiver;
		pArchiver = NULL;

		if(FALSE == pReader->Initialize(nFileId, m_iBookId, pStream, (const char *)rgFileName, m_pPassword))
		{
			UIMessageBox messagebox(this, StringManager::GetStringById(CANNOT_OPEN_BOOK), MB_OK);
			messagebox.DoModal();
            SafeDeletePointer(pReader);
			return FALSE;
		}
	}

	CDisplay* display = CDisplay::GetDisplay();
	if(NULL == display)
	{
		return FALSE;
	}
	pReader->MoveWindow(0, 0, display->GetScreenWidth(), display->GetScreenHeight());
	pReader->GotoBookmark(m_curModeList[iBookmarkIndex]->GetBookmark());
	CPageNavigator::BackToPrePage();

	return TRUE;
}

bool UIBookmarkListBox::OnItemClick(INT32 iSelectedItem)
{
	DebugPrintf (DLC_UICOMPOUNDLISTBOX, "UIBookmarkListBox::OnItemClick begin");
    if (iSelectedItem < 0 || iSelectedItem >= (int)GetChildrenCount())
    {
        return true;
    }

    UICompoundListBox::OnItemClick(iSelectedItem);
    HandleGoToBookMark(iSelectedItem);
    if (m_pParent)
	    m_pParent->OnCommand(ID_BTN_TOUCH_TOC_GOTOBOOKMARK, this, 0);
	DebugPrintf (DLC_UICOMPOUNDLISTBOX, "UIBookmarkListBox::OnItemClick finish");
	return true;
}

BOOL UIBookmarkListBox::HandleLongTap(INT32 selectedItem)
{
    if (selectedItem < 0 || selectedItem >= (int)GetChildrenCount())
        return false;

    UIBookmarkListItem *pItem = (UIBookmarkListItem *)GetChildByIndex(selectedItem);
    bool bRet = false;
    if (pItem && pItem->IsVisible())
    {
        UIMessageBox messagebox(m_pParent, StringManager::GetStringById(SURE_DELETE), MB_OK | MB_CANCEL);
        if (MB_OK == messagebox.DoModal())
        {
            m_bNeedUpdate = true;
            DeleteKeyHander(selectedItem);
            InitListItem();
            Repaint();
        }
    }
    return bRet;
}


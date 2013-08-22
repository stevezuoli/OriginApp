///////////////////////////////////////////////////////////////////////
// BookOpenManager.cpp
// Contact:
// Copyright (c) Duokan Corporation. All rights reserved.
// create by jzn
////////////////////////////////////////////////////////////////////////
#include "../Common/FileManager_DK.h"
#include "GUI/UIMessageBox.h"
#include "Framework/CHourglass.h"
#include "Framework/CDisplay.h"
#include "TouchAppUI/BookOpenManager.h"
#include "TouchAppUI/UISingleTextInputDialog.h"
#include "TouchAppUI/DkReaderPage.h"
#include "TouchAppUI/BookInfoItem.h"
#include "TouchAppUI/UITablePage.h"
#include "TouchAppUI/BookMarkItem.h"
#include "CommonUI/CPageNavigator.h"
#include "Common/IFileChapterInfo.h"
#include "Common/FileContentsFactory.h"
#include "Model/IFileContentsModel.h"
#include "I18n/StringManager.h"
#include "BookInfoManager.h"
#include "TextLocatorImpl.h"
#include "SQM.h"

SINGLETON_CPP(BookOpenManager)

static INT AdjustFileList(FileListItem *pFileList, UINT uLength, LPCSTR *ppFileNameArray, LPCSTR pFileName)
{
	if(NULL == pFileList || NULL == ppFileNameArray || NULL == pFileName)
	{
		return -1;

	}

	INT iIndex = 0;
	while(iIndex < (INT)uLength)
	{
		if(strcmp(pFileName, ppFileNameArray[iIndex]) < 0)
		{
			break;
		}
		iIndex ++;

	}

	INT iIndexTemp = (INT)(uLength - 1);
	while(iIndexTemp >= iIndex)
	{
		ppFileNameArray[iIndexTemp + 1] = ppFileNameArray[iIndexTemp];
		memcpy(&(pFileList[iIndexTemp + 1]), &(pFileList[iIndexTemp]), sizeof(FileListItem));
		iIndexTemp --;
	}

	ppFileNameArray[iIndex] = pFileName;

	return iIndex;
}

void BookOpenManager::Initialize(UIContainer *pCurPage, UINT32 iHourglassLeft, UINT32  iHourglassTop)//:m_iBookID(-1), m_pCurPage(pCurPage)
{
    DebugPrintf(DLC_BOOKOPENMANAGER, "%s, %d, %s, %s 1 START", __FILE__,  __LINE__, "BookOpenManager", __FUNCTION__);
    m_pCurPage=pCurPage;
    m_iHourglassLeft = iHourglassLeft;
    m_iHourglassTop = iHourglassTop;
    m_iBookID = -1;
    DebugPrintf(DLC_BOOKOPENMANAGER, "%s, %d, %s, %s 1 END : m_iBookID = %d", __FILE__,  __LINE__, "BookOpenManager", __FUNCTION__, m_iBookID);
}

void BookOpenManager::Initialize(UIContainer *pCurPage, INT32  iBookID, UINT32  iHourglassLeft, UINT32  iHourglassTop)
{
    DebugPrintf(DLC_BOOKOPENMANAGER, "%s, %d, %s, %s 2 START", __FILE__,  __LINE__, "BookOpenManager", __FUNCTION__);
	m_pCurPage = pCurPage;
	m_iHourglassLeft = iHourglassLeft;
	m_iHourglassTop = iHourglassTop;
    m_iBookID = iBookID;
    DebugPrintf(DLC_BOOKOPENMANAGER, "%s, %d, %s, %s 2 END : m_iBookID = %d", __FILE__,  __LINE__, "BookOpenManager", __FUNCTION__, m_iBookID);
}

void BookOpenManager::Initialize(UIContainer *pCurPage, CString strBookName, UINT32  iHourglassLeft, UINT32  iHourglassTop)
{
    DebugPrintf(DLC_BOOKOPENMANAGER, "%s, %d, %s, %s 3 START", __FILE__,  __LINE__, "BookOpenManager", __FUNCTION__);
	m_pCurPage = pCurPage;
	m_iHourglassLeft = iHourglassLeft;
	m_iHourglassTop = iHourglassTop;


	m_iBookID = -1;
	CDKFileManager  *pFileManager = CDKFileManager::GetFileManager();
	if(pFileManager)
	{
		m_iBookID = pFileManager->GetFileIdByPath(strBookName.GetBuffer());
    }
    DebugPrintf(DLC_BOOKOPENMANAGER, "%s, %d, %s, %s 3 END : m_iBookID = %d", __FILE__,  __LINE__, "BookOpenManager", __FUNCTION__, m_iBookID);
}

BookOpenManager::~BookOpenManager()
{
}


LPCSTR BookOpenManager::GetBookName()
{
    DebugPrintf(DLC_BOOKOPENMANAGER, "%s, %d, %s, %s START", __FILE__,  __LINE__, "BookOpenManager", __FUNCTION__);
	CDKFileManager *pFileManager = CDKFileManager::GetFileManager();
	if(pFileManager)
	{
		PCDKFile pDkFile = pFileManager->GetFileById(m_iBookID);
        if(pDkFile)
        {
            DebugPrintf(DLC_BOOKOPENMANAGER, "%s, %d, %s, %s end", __FILE__,  __LINE__, "BookOpenManager", __FUNCTION__);
			return (CHAR *)pDkFile->GetFileName();
        }
    }
    DebugPrintf(DLC_BOOKOPENMANAGER, "%s, %d, %s, %s pFileManager is NULL", __FILE__,  __LINE__, "BookOpenManager", __FUNCTION__);
	return NULL;
}

LPCSTR BookOpenManager::GetBookPath()
{
    DebugPrintf(DLC_BOOKOPENMANAGER, "%s, %d, %s, %s START", __FILE__,  __LINE__, "BookOpenManager", __FUNCTION__);
	CDKFileManager *pFileManager = CDKFileManager::GetFileManager();
	if(pFileManager)
	{
		PCDKFile pDkFile = pFileManager->GetFileById(m_iBookID);
        if(pDkFile)
        {
            DebugPrintf(DLC_BOOKOPENMANAGER, "%s, %d, %s, %s end", __FILE__,  __LINE__, "BookOpenManager", __FUNCTION__);
            return (CHAR *)pDkFile->GetFilePath();
        }
    }
    DebugPrintf(DLC_BOOKOPENMANAGER, "%s, %d, %s, %s pFileManager is NULL", __FILE__,  __LINE__, "BookOpenManager", __FUNCTION__);
	return NULL;
}


DkFileFormat BookOpenManager::GetBookFormat()
{
    DebugPrintf(DLC_BOOKOPENMANAGER, "%s, %d, %s, %s START", __FILE__,  __LINE__, "BookOpenManager", __FUNCTION__);
	CDKFileManager *pFileManager = CDKFileManager::GetFileManager();
	if(pFileManager)
	{
		PCDKFile pDkFile = pFileManager->GetFileById(m_iBookID);
		if (pDkFile)
        {
            DebugPrintf(DLC_BOOKOPENMANAGER, "%s, %d, %s, %s end", __FILE__,  __LINE__, "BookOpenManager", __FUNCTION__);
			return (pDkFile->GetFileFormat());
		}
    }
    DebugPrintf(DLC_BOOKOPENMANAGER, "%s, %d, %s, %s pFileManager is NULL", __FILE__,  __LINE__, "BookOpenManager", __FUNCTION__);
	return DFF_Unknown;
}

BOOL BookOpenManager::CheckIsNew()
{
    DebugPrintf(DLC_BOOKOPENMANAGER, "%s, %d, %s, %s START", __FILE__,  __LINE__, "BookOpenManager", __FUNCTION__);
	CDKFileManager *pFileManager = CDKFileManager::GetFileManager();
	if(pFileManager)
	{
		PCDKFile pDkFile = pFileManager->GetFileById(m_iBookID);
		if (pDkFile && pDkFile->GetFileLastReadTime())
        {
            DebugPrintf(DLC_BOOKOPENMANAGER, "%s, %d, %s, %s return FALSE", __FILE__,  __LINE__, "BookOpenManager", __FUNCTION__);
            return FALSE;
        }
    }
    DebugPrintf(DLC_BOOKOPENMANAGER, "%s, %d, %s, %s return TRUE", __FILE__,  __LINE__, "BookOpenManager", __FUNCTION__);
	return TRUE;
}

const CHAR  *BookOpenManager::GetAuthorName()
{
    DebugPrintf(DLC_BOOKOPENMANAGER, "%s, %d, %s, %s START", __FILE__,  __LINE__, "BookOpenManager", __FUNCTION__);
	// TODO: remove this test code
	return "Author ";
}

float BookOpenManager::GetProgress()
{
    DebugPrintf(DLC_BOOKOPENMANAGER, "%s, %d, %s, %s START", __FILE__,  __LINE__, "BookOpenManager", __FUNCTION__);
	CDKFileManager *pFileManager = CDKFileManager::GetFileManager();
	if(pFileManager)
	{
		PCDKFile pDkFile = pFileManager->GetFileById(m_iBookID);
		if (pDkFile)
        {
            DebugPrintf(DLC_BOOKOPENMANAGER, "%s, %d, %s, %s end", __FILE__,  __LINE__, "BookOpenManager", __FUNCTION__);
			return (pDkFile->GetFilePlayProcess());
        }
    }
    DebugPrintf(DLC_BOOKOPENMANAGER, "%s, %d, %s, %s pFileManager is NULL", __FILE__,  __LINE__, "BookOpenManager", __FUNCTION__);
	return 0;
}

LPCSTR BookOpenManager::GetFilePassword()
{
    DebugPrintf(DLC_BOOKOPENMANAGER, "%s, %d, %s, %s START", __FILE__,  __LINE__, "BookOpenManager", __FUNCTION__);
	CDKFileManager *pFileManager = CDKFileManager::GetFileManager();
	if(pFileManager)
	{
		PCDKFile pDkFile = pFileManager->GetFileById(m_iBookID);
		if(pDkFile)
        {
            DebugPrintf(DLC_BOOKOPENMANAGER, "%s, %d, %s, %s end", __FILE__,  __LINE__, "BookOpenManager", __FUNCTION__);
            return (CHAR *)pDkFile->GetFilePassword();
        }
    }
    DebugPrintf(DLC_BOOKOPENMANAGER, "%s, %d, %s, %s pFileManager is NULL", __FILE__,  __LINE__, "BookOpenManager", __FUNCTION__);
	return NULL;
}


BOOL BookOpenManager::GetFilePassword(UIContainer *pPage,INT32 nBookId,LPSTR pPassword,INT32 iPwdLen)
{
    DebugPrintf(DLC_BOOKOPENMANAGER, "%s, %d, %s, %s START", __FILE__,  __LINE__, "BookOpenManager", __FUNCTION__);
	if(NULL == pPage)
    {
        DebugPrintf(DLC_BOOKOPENMANAGER, "%s, %d, %s, %s NULL == pPage", __FILE__,  __LINE__, "BookOpenManager", __FUNCTION__);
		return FALSE;
	}

	CDKFileManager *pFileManager = CDKFileManager::GetFileManager();
	if(NULL == pFileManager)
    {
        DebugPrintf(DLC_BOOKOPENMANAGER, "%s, %d, %s, %s NULL == pFileManager", __FILE__,  __LINE__, "BookOpenManager", __FUNCTION__);
		return FALSE;
	}

	PCDKFile pDkFile = pFileManager->GetFileById(nBookId);
	if(NULL == pDkFile)
    {
        DebugPrintf(DLC_BOOKOPENMANAGER, "%s, %d, %s, %s NULL == pDkFile", __FILE__,  __LINE__, "BookOpenManager", __FUNCTION__);
		return FALSE;
	}

	LPCSTR pFilePwd = pDkFile->GetFilePassword();
	if(pFilePwd && strlen(pFilePwd))
	{
        strncpy(pPassword,pFilePwd,iPwdLen - 1);
        DebugPrintf(DLC_BOOKOPENMANAGER, "%s, %d, %s, %s end---pFilePwd && strlen(pFilePwd)", __FILE__,  __LINE__, "BookOpenManager", __FUNCTION__);
		return TRUE;
	}


	UISingleTextInputDialog PwdDlg(pPage);
	PwdDlg.SetDialogTitle(StringManager::GetPCSTRById(INPUT_PASSWORD));
	PwdDlg.MoveWindow(0, 0, 0, 0);
	if(0 == PwdDlg.DoModal())
	{
		return FALSE;
	}

    std::string password = PwdDlg.GetInputText();
	if(!password.empty())
	{
		strncpy(pPassword, password.c_str(), iPwdLen - 1);
		return TRUE;
    }
    DebugPrintf(DLC_BOOKOPENMANAGER, "%s, %d, %s, %s return FALSE", __FILE__,  __LINE__, "BookOpenManager", __FUNCTION__);
	return FALSE;
}



BOOL BookOpenManager::OpenArchiveBook()
{
    DebugPrintf(DLC_BOOKOPENMANAGER, "%s, %d, %s, %s START", __FILE__,  __LINE__, "BookOpenManager", __FUNCTION__);
	if(!m_pCurPage)
	{
		return FALSE;
	}

	CDisplay* display = CDisplay::GetDisplay();
	if(NULL == display)
	{
		return FALSE;
	}

	IFileContentsModel *pCurBookContent = NULL;
	if(S_FALSE == CFileContentsFactory::CreateFileContentsInstance(GetBookPath(),&pCurBookContent))
	{
		return FALSE;
	}

	if(NULL == pCurBookContent)
	{
		return FALSE;
	}

	char Password[128]={0};
	if(pCurBookContent->HasPassword())
	{
		if(GetFilePassword(m_pCurPage,m_iBookID,Password,128))
		{
			pCurBookContent->SetPassword(Password);
		}
		else
		{
			return TRUE;
		}
	}

	vector < CFileChapterInfo > ChapterList;
	if(!pCurBookContent->GetChapterList(ChapterList))
	{
		delete pCurBookContent;
		pCurBookContent = NULL;
		return FALSE;
	}

	int iChapterCount = ChapterList.size();
	FileListItem *pZipFileList = new FileListItem[iChapterCount];
	if(NULL == pZipFileList)
	{
		delete pCurBookContent;
		pCurBookContent = NULL;
		return FALSE;
	}

	INT iImageCount = 0;
	INT iAdjustIndex = 0;
	LPCSTR pExtension = NULL;
	LPCSTR pFileName = NULL;

	LPCSTR *pNameArray = new LPCSTR[iChapterCount];
	if(NULL == pNameArray)
	{
		delete pCurBookContent;
		pCurBookContent = NULL;

		delete[] pZipFileList;
		pZipFileList = NULL;
		return FALSE;
	}

	for(INT index= 0; index < iChapterCount; index++)
	{
		pExtension = ChapterList[index].GetChapterExName();
		if(DFC_Picture == GetFileCategoryForZipFile(pExtension))
		{
			pFileName = ChapterList[index].GetChapterPath();
			iAdjustIndex = AdjustFileList(pZipFileList, iImageCount, pNameArray, pFileName);

			pZipFileList[iAdjustIndex].uFileId = ChapterList[index].GetChapterId();
			pZipFileList[iAdjustIndex].eFileCategory = DFC_Picture;

			strncpy(pZipFileList[iAdjustIndex].pExtension, pExtension, 16);
			iAdjustIndex++;
			iImageCount++;
		}
	}

	DebugPrintf(DLC_BOOKOPENMANAGER, "BookOpenManager::OpenBook iImageCount %d\n", iImageCount);

	delete[] pNameArray;
	pNameArray = NULL;

	if(iChapterCount == iImageCount)
	{
		BookFileList * pZipBookList = new BookFileList();
		if(NULL == pZipBookList)
		{
			delete pCurBookContent;
			pCurBookContent = NULL;

			delete[] pZipFileList;
			pZipFileList = NULL;
			return FALSE;
		}

		pZipBookList->pFileList = pZipFileList;
		pZipBookList->iBookId= m_iBookID;
		pZipBookList->uLength = iImageCount;

		DkReaderPage *pReader = new DkReaderPage();
		if(NULL == pReader)
		{
			delete pCurBookContent;
			pCurBookContent = NULL;

			delete pZipBookList;
			pZipBookList = NULL;

			return FALSE;
		}

		if(!pReader->Initialize(SPtr<BookFileList>(pZipBookList), 0, TRUE,Password))
		{

			UIMessageBox messagebox(m_pCurPage, StringManager::GetStringById(CANNOT_OPEN_BOOK), MB_OK);
			messagebox.DoModal();

			delete pCurBookContent;
			pCurBookContent = NULL;

			delete pReader;
			pReader = NULL;
			return FALSE;
		}
		else
		{
			pReader->MoveWindow(0, 0, display->GetScreenWidth(), display->GetScreenHeight());
			CPageNavigator::Goto(pReader);
			delete pCurBookContent;
			pCurBookContent = NULL;
			return TRUE;
		}
	}
	else	//如果压缩包中含有非图片内容，则暂不支持！
    {
        UIMessageBox messagebox(m_pCurPage, StringManager::GetStringById(CANNOT_OPEN_BOOK), MB_OK);
        messagebox.DoModal();

		delete pCurBookContent;
		pCurBookContent = NULL;
		delete[] pZipFileList;
		pZipFileList = NULL;
		return FALSE;
    }
    DebugPrintf(DLC_BOOKOPENMANAGER, "%s, %d, %s, %s end", __FILE__,  __LINE__, "BookOpenManager", __FUNCTION__);
    return true;
}

BOOL BookOpenManager::OpenBook()
{
    DebugPrintf(DLC_BOOKOPENMANAGER, "%s, %d, %s, %s START", __FILE__,  __LINE__, "BookOpenManager", __FUNCTION__);
	if(!m_pCurPage)
	{
		return FALSE;
	}
	CDKFileManager * pclsFileManager = CDKFileManager::GetFileManager();
	if(!pclsFileManager)
	{
		return FALSE;
	}
	pclsFileManager->ConvertDKPToDKX(GetBookPath());

	//CHourglass* pIns = CHourglass::GetInstance();
	//if(m_iHourglassLeft == 0 && m_iHourglassTop == 0)
	//{
	//	pIns = NULL;
	//}

	//if(pIns)
	//{
	//	pIns->Start(m_iHourglassLeft, m_iHourglassTop);
	//}

	CDisplay* display = CDisplay::GetDisplay();
	if(NULL == display)
	{
		return FALSE;
	}

	DkFileFormat Format = GetBookFormat();
	switch (Format)
	{
	case DFF_Text:
		SQM::GetInstance()->IncCounter(SQM_ACTION_BOOKREADER_FORMAT_TXT);
		break;
	case DFF_ZipFile:
		SQM::GetInstance()->IncCounter(SQM_ACTION_BOOKREADER_FORMAT_ZIP);
		break;
	case DFF_RoshalArchive:
		SQM::GetInstance()->IncCounter(SQM_ACTION_BOOKREADER_FORMAT_RAR);
		break;
	case DFF_PortableDocumentFormat:
		SQM::GetInstance()->IncCounter(SQM_ACTION_BOOKREADER_FORMAT_PDF);
		break;            
	case DFF_ElectronicPublishing:
		SQM::GetInstance()->IncCounter(SQM_ACTION_BOOKREADER_FORMAT_EPUB);
		break;            
	case DFF_Serialized:
		SQM::GetInstance()->IncCounter(SQM_ACTION_BOOKREADER_FORMAT_SERIALIZED);
		break;         
	case DFF_Unknown:
		SQM::GetInstance()->IncCounter(SQM_ACTION_BOOKREADER_FORMAT_UNKNOWN);
		break;
	default:
		//DONOTHING for others
		break;
	}

	if(Format == DFF_ZipFile || Format == DFF_RoshalArchive || Format == DFF_Serialized)
	{

		BOOL bOpenSuccess = OpenArchiveBook();

		//if(pIns)
		//{
		//	pIns->Stop();
		//}

		return bOpenSuccess;
	}
	else
	{
		DkReaderPage *pReader = new DkReaderPage();
		if(NULL == pReader)
		{
			//if(pIns)
			//{
			//	pIns->Stop();
			//}

			return FALSE;
		}

		if (!pReader->Initialize(GetBookPath(), GetBookName(), m_iBookID))
		{
			delete pReader;
			pReader = NULL;
			//if(pIns)
			//{
			//	pIns->Stop();
			//}

			UIMessageBox messagebox(m_pCurPage, StringManager::GetStringById(CANNOT_OPEN_BOOK), MB_OK);
			messagebox.DoModal();
		}
		else
		{
			pReader->MoveWindow(0, 0, display->GetScreenWidth(), display->GetScreenHeight());
			//Here: we move the pIns->Stop() into UIBookReaderContainer::Draw() to fix bug like: KTV00026614
            CDisplay::GetDisplay()->SetFullRepaint(true);
			CPageNavigator::Goto(pReader);
		}
    }
    DebugPrintf(DLC_BOOKOPENMANAGER, "%s, %d, %s, %s end", __FILE__,  __LINE__, "BookOpenManager", __FUNCTION__);
	return TRUE;
}




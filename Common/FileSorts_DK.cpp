#include "Common/FileSorts_DK.h"
//#include "UTF8TOGBK.h"
#include "interface.h"
#include "Common/ConfigInfo.h"
#include "Utility.h"
#include "dkWinError.h"
#include "interface.h"
#include "Utility/EncodeUtil.h"
#include "Framework/CDisplay.h"
#include "I18n/StringManager.h"
#include "GUI/CTpGraphics.h"
#include "GUI/UIMessageBox.h"
#include "Utility/Misc.h"


using dk::utility::EncodeUtil;
//#define FILESORT_DEBUG

#ifdef FILESORT_DEBUG
#include <sys/time.h>
#endif

using namespace DkFormat;

#define PYMBINFOPATH    "res/pymb.ini" //首字母查询配置文件
CDKFileSorts::CDKFileSorts()
            : m_fileCategory(DFC_Unknown)
            , m_sort(LAST_ACCESS_TIME)
{
}

void CDKFileSorts::SetFileListAndNum(const std::vector<PCDKFile>& files)
{
    m_files = files;
}

void CDKFileSorts::SetFileSorts(Field sort)
{
    m_sort = sort;
}

void CDKFileSorts::SetFileCategory(DkFormatCategory category)
{
    m_fileCategory = category;
}


void CDKFileSorts::SortsFile()
{
    switch(m_sort)
    {
    case RECENTLY_ADD:
        SortsFileByRecentlyAdd();
        break;
    case LAST_ACCESS_TIME:
        SortsFileByRecentlyReadTime();
        break;
    case NAME:
        SortsFileByFileName();
        break;
    default:
        break;
    }
    return;
}


void CDKFileSorts::CreateFileListIndexbyDefault(bool isNameSort)
{
    if (m_files.empty())
    {
        return;
    }

	if(isNameSort && SystemSettingInfo::GetInstance()->GetMetadataOpen())
	{
		CDisplay* pDisplay = CDisplay::GetDisplay();
		if (pDisplay)
		{
			UITouchComplexButton btnTip;
			btnTip.Initialize(IDNO, StringManager::GetPCSTRById(SYNCING), 22);
			const int screenWidth = pDisplay->GetScreenWidth();
			const int screenHeight = pDisplay->GetScreenHeight();
			const int btnWidth = 250;
			const int btnHeight = 50;
			btnTip.MoveWindow((screenWidth - btnWidth)/2, (screenHeight - btnHeight)/2, btnWidth, btnHeight);
			DK_IMAGE img = pDisplay->GetMemDC();
			CTpGraphics grf(img);
			btnTip.Draw(img);
			DkRect rect = DkRect::FromLeftTopWidthHeight((screenWidth - btnWidth)/2, (screenHeight - btnHeight)/2, btnWidth, btnHeight);
			pDisplay->ForceDraw(rect, PAINT_FLAG_FULL);
			pDisplay->EndCache();
		}
	}
	
    FileCategoryMatcher matcher(m_fileCategory);
    m_sortedFiles.clear();
    copy_if(m_files.begin(), m_files.end(), std::back_inserter(m_sortedFiles), matcher);
    for (DK_AUTO(cur, m_sortedFiles.begin()); cur != m_sortedFiles.end(); ++cur)
    {
        if (isNameSort)
        {
            (*cur)->SyncFile();
        }
    }
    return;
}


void CDKFileSorts::SortsFileByRecentlyReadTime()
{
    CreateFileListIndexbyDefault(false);
    FileComparer comparer(FileComparer::CT_ReadingOrder);
    std::sort(m_sortedFiles.begin(), m_sortedFiles.end(), comparer);
}

void CDKFileSorts::SortsFileByRecentlyAdd()
{
    CreateFileListIndexbyDefault(false);
    FileComparer comparer(FileComparer::CT_AddingOrder);
    std::sort(m_sortedFiles.begin(), m_sortedFiles.end(), comparer);
}

void CDKFileSorts::SortsFileByFileNameDefault()
{
    CreateFileListIndexbyDefault(true);
    FileComparer comparer(FileComparer::CT_Name);
    std::sort(m_sortedFiles.begin(), m_sortedFiles.end(), comparer);
}

void CDKFileSorts::SortsFileByFileName()
{
    SortsFileByFileNameDefault();
}

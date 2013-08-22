#include "FontManager/DKFont.h"
#include "FontManager/dk_chst2cp.h"
#include "DKTTFInfo.h"
#include "FCFontInfo.h"
#include "interface.h"
#include <algorithm>
#include "Utility/EncodeUtil.h"
using namespace dk::utility;

using std::vector;
using std::string;
using std::wstring;

DKFont::DKFont()
    : m_pTTFInfo(DK_NULL)
{
}    

DKFont::~DKFont()
{
    Release();
}    

bool DKFont::LoadFont(string& wsFileName)
{
    DebugPrintf(DLC_JUGH, "DKFont::LoadFont START!!!");
    assert (wsFileName.length() > 0);
    
    m_pFilePath = wsFileName;      
    if (m_pFilePath.length() <= 4)
    {
        DebugPrintf(DLC_JUGH, "DKFont::LoadFont The filename is too short!!!");
        return DK_FALSE;
    }

    string csFileType = m_pFilePath.substr(m_pFilePath.size() - 4, 4);
    transform(csFileType.begin(), csFileType.end(), csFileType.begin(), ::tolower);
    TT_FONTFILE_TYPE eFontType = TT_FONTFILE_TTF;
    if (0 != strcmp(csFileType.c_str(), ".ttf"))
    {
        if (0 != strcmp(csFileType.c_str(), ".ttc"))
        {
            DebugPrintf(DLC_JUGH, "DKFont::LoadFont The filetype is not supported!!!");
            return DK_FALSE;
        }
        eFontType = TT_FONTFILE_TTC;
    }
	if(m_pTTFInfo != DK_NULL)
	{
		SafeDeletePointer(m_pTTFInfo);
	}
    m_pTTFInfo = new DKTTFInfo();  
    //m_pTTFInfo = new FCFontInfo ();
    if (DK_NULL != m_pTTFInfo && m_pTTFInfo->OpenFontFile(m_pFilePath, eFontType))
    {
        DebugPrintf(DLC_JUGH, "DKFont::LoadFont SUCCESSED!!!");
        return DK_TRUE;
    }
    DebugPrintf(DLC_JUGH, "DKFont::LoadFont FAILED!!!");
    return DK_FALSE;
}    

void DKFont::Release()
{
    if (DK_NULL != m_pTTFInfo)
    {
        m_pTTFInfo->CloseFontFile();
		SafeDeletePointer(m_pTTFInfo);
    }
    m_pFilePath.empty();
    m_pFileDir.empty();
}    

vector<wstring>* DKFont::GetFullFontName() const
{
    assert(DK_NULL != m_pTTFInfo);    
    return m_pTTFInfo->GetFullFontName();
}    

std::string DKFont::GetChineseFontName() const
{
    DK_AUTO(fullFontNames, GetFullFontName());
    std::string result;
    if (fullFontNames && !fullFontNames->empty())
    {
        result = EncodeUtil::ToString(fullFontNames->back());
    }
    return result;
}

vector<wstring>* DKFont::GetFontFamilyName()
{
    assert(DK_NULL != m_pTTFInfo);    
    return m_pTTFInfo->GetFontFamilyName();
}    
    
vector<wstring>* DKFont::GetFontSubFamilyName()
{
    assert(DK_NULL != m_pTTFInfo);    
    return m_pTTFInfo->GetFontSubFamilyName();
}    

bool DKFont::GetSupportedCodePage(vector<unsigned short>& lstCodepage)
{
    assert(DK_NULL != m_pTTFInfo);
    
    return m_pTTFInfo->GetSupportedCodePage(lstCodepage);
}    

bool DKFont::IsSupportingCharSet(DK_CHARSET_TYPE charSet)
{
    vector<unsigned short> lstCodePage;
    bool ret = m_pTTFInfo->GetSupportedCodePage(lstCodePage);
    if (ret)
    {
        vector<unsigned short>::iterator it;
        for (it = lstCodePage.begin(); it < lstCodePage.end(); ++it)
        {
            if (DK_GetChstByCP (*it) == charSet)
            {
                return true;
            }
        }
    }
    return false;
}

bool DKFont::GetSupportedCharSet(vector<unsigned short>& lstCharset)
{
    vector<unsigned short> lstCodePage;
    bool ret = m_pTTFInfo->GetSupportedCodePage(lstCodePage);
    if (ret)
    {
        vector<unsigned short>::iterator it;
        for (it = lstCodePage.begin(); it < lstCodePage.end(); ++it)
        {
            lstCharset.push_back (DK_GetChstByCP (*it));
        }
    }
    return ret;
}

int DKFont::GetFontStyle()
{
    assert(DK_NULL != m_pTTFInfo);
    
    return m_pTTFInfo->GetFontStyle();
}

const char* DKFont::GetFontPath(bool bFullPath)
{    
    if (bFullPath)
    {  
        return m_pFilePath.c_str(); 
    }
    else
    {     
        int iPathLen = m_pFilePath.rfind("/", m_pFilePath.length());
        if (iPathLen <= 0)
        {
            DebugPrintf(DLC_JUGH, "DKFont::GetFontPath ERROR %s!!!", m_pFilePath.c_str());
            return DK_FALSE;
        }
        ++iPathLen;
        m_pFileDir = m_pFilePath.substr(0, iPathLen);
        return m_pFileDir.c_str();
    }
}
    
bool DKFont::operator()(const DKFont* pLeft, const DKFont* pRight)
{
    return (*(pLeft->m_pTTFInfo)) < (*(pRight->m_pTTFInfo));
}

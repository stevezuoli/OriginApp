#include "FontManager/DKFont.h"
#include "FontManager/dk_chst2cp.h"
#include "DkArchiveFactory.h"
#include "KernelEncoding.h"
#include "DKEAPI.h"
#include "interface.h"
#include <algorithm>
#include <dirent.h>
#include <sys/stat.h>
#include <stdio.h>
#include "Utility/StringUtil.h"

using namespace dk::utility;

using std::vector;
using std::string;

//#define DEBUG_FONTMANAGER_INFO
#ifdef DEBUG_FONTMANAGER_INFO
#include <iostream.h>
#endif

// 存放默认codepage对应的字体 FaceName
typedef struct _tagDK_CHARSET2FACENAME
{
    DK_CHARSET_TYPE eCharset;
    wstring         strFaceName;
} DK_CHARSET2FACENAME;

static DK_CHARSET2FACENAME gs_lstCHARSET2FONT[5] = 
{
    {DK_CHARSET_ANSI,    wstring()},
    {DK_CHARSET_SHITJIS, wstring()},
    {DK_CHARSET_JOHAB,   wstring()},
    {DK_CHARSET_GB,      wstring()},
    {DK_CHARSET_BIG5,    wstring()}
};

DKFontManager* g_pFontManager = new DKFontManager;

DKFontManager::DKFontManager()
    : m_strSysDefFontFaceName(wstring()),
      m_iFontSizeRecord(0)
{
    m_lstFontDir.clear();
    m_lstFont.clear();
}

DKFontManager::~DKFontManager()
{
    m_lstFontDir.clear();
    m_lstFont.clear();
}

void DKFontManager::Release()
{ 
    vector<DKFont*>::iterator viFont;
    for (viFont = m_lstFont.begin(); viFont != m_lstFont.end();)
    {
        (*viFont)->Release();
        viFont = m_lstFont.erase(viFont);
    }
    m_lstFontDir.clear();
}

bool DKFontManager::AddFont(DKFont* pFont)
{
    assert(DK_NULL != pFont);
    if (m_lstFont.end() != find(m_lstFont.begin(), m_lstFont.end(), pFont))
    {
        DebugPrintf(DLC_JUGH, "DKFontManager::AddFont FONT IS EXISTING!!!");
        return TRUE;  
    }
    
    m_lstFont.push_back(pFont);
    string strFontPath(pFont->GetFontPath(false));
    AddDir(strFontPath);
    
    return m_lstFont.size() > 0;
}

void DKFontManager::RemoveFont(DKFont* pFont)
{
    assert(DK_NULL != pFont);
    vector<DKFont*>::iterator vi;
    vi = find(m_lstFont.begin(), m_lstFont.end(), pFont); 
    if (vi == m_lstFont.end())
        return;
    m_lstFont.erase(vi);
    // TODO: 这里dir不进行删除 
}        
    
DKFont* DKFontManager::GetFont(string& wsFontPath)
{
    assert (wsFontPath.length() > 0);
    
    DKFont* pFont = DK_NULL;
    for (unsigned int i=0; i < m_lstFont.size(); ++i)
    {
        pFont = m_lstFont[i];
        if (0 == strcmp(wsFontPath.c_str(), pFont->GetFontPath()))
        {
            return pFont;
        }
    }
    
    pFont = new DKFont();
    if (DK_NULL != pFont)
    {
        if (!pFont->LoadFont(wsFontPath) || !AddFont(pFont))
        {
            DebugPrintf(DLC_JUGH, "DKFontManager::GetFont ERROR %s!!!", wsFontPath.c_str());
            SafeReleasePointer(pFont);
            return DK_NULL;
        }                        
    }
    return pFont;
}

DKFont* DKFontManager::GetFontForFaceName(const wstring& wsFaceName)
{
    vector<DKFont*>::iterator viFont;
    vector<wstring>* lstFontName;
    for (viFont = m_lstFont.begin (); viFont < m_lstFont.end (); ++viFont)
    {
        lstFontName = (*viFont)->GetFullFontName ();
        if (!lstFontName)
        {
            continue;
        }
        
        for (unsigned int i = 0; i < lstFontName->size (); ++i)
        {
            if (0 == wcscmp (wsFaceName.c_str (), lstFontName->at (i).c_str ()))
            {
                return (*viFont);
            }
        }
    }
    return DK_NULL;
}

bool DKFontManager::GetAllFont(vector<DKFont*> &lstFont)
{
    lstFont.clear();
    lstFont = m_lstFont;
    return lstFont.size() > 0;
}

bool DKFontManager::AddFontsDir(string& wsFontDir)
{
    assert (wsFontDir.length() > 0);
    
    vector<string> lstFontPath;
    if (!GetAllFontsDir(wsFontDir, lstFontPath))
    {
        DebugPrintf(DLC_JUGH, "DKFontManager::AddFontsDir GetAllFontsDir ERROR!!!");
        return DK_FALSE;
    }
    
    vector<string>::iterator viFontPath;
    for (viFontPath = lstFontPath.begin(); viFontPath != lstFontPath.end(); viFontPath++)
    {                 
        DKFont* pFont = new DKFont();
        if (DK_NULL != pFont)
        {
            if (!pFont->LoadFont((*viFontPath)) || !AddFont(pFont))
            {
                DebugPrintf(DLC_JUGH, "DKFontManager::AddFontsDir LoadFont ERROR!!!");
                SafeReleasePointer(pFont);
            }                        
        }
        else
        {
            DebugPrintf(DLC_JUGH, "DKFontManager::AddFontsDir DKFont() ERROR!!!");
            return DK_FALSE;
        }   
    }
    m_iFontSizeRecord += GetDirFileSize (wsFontDir);
    return DK_TRUE;
}

void DKFontManager::RemoveFontsDir(string& wsFontDir)
{
    assert (wsFontDir.length() > 0);
    
    vector<DKFont*>::iterator viFont;
    for (viFont = m_lstFont.begin(); viFont != m_lstFont.end();)
    {
        const char* pFontPath = (*viFont)->GetFontPath(DK_FALSE);
        if (0 == strcmp(pFontPath, wsFontDir.c_str()))
        {
            (*viFont)->Release();
            viFont = m_lstFont.erase(viFont);  
            string csFontPath(pFontPath);
            RemoveDir(csFontPath);
        }
        else
        {
            ++viFont;
        }
    }
    m_iFontSizeRecord -= GetDirFileSize (wsFontDir);
}

bool DKFontManager::GetFontsDir(vector<string>& lstFontDir)
{
    lstFontDir = m_lstFontDir;
    return lstFontDir.size() > 0;
}

bool DKFontManager::GetAllFontsDir(string& wsFontPath, vector<string> &lstFontDir)
{    
    assert (wsFontPath.length() > 0);
    
    lstFontDir.clear();
    IDkArchive* pArchive = DkArchiveFactory::CreateInstance(IDkArchive::DK_FOLDER, wsFontPath.c_str());
    if (DK_NULL != pArchive)
    {
        DK_INT iFileNum = pArchive->GetEntryNum();
        for (DK_INT i = 0; i < iFileNum; ++i)
        {
            if (pArchive->IsFile(i))
            {
                DK_CHAR* pFileName = pArchive->GetEntryNameByIndex(i);
                if (DK_NULL != pFileName)
                {
                    string csFileName;
                    csFileName = wsFontPath + string(pFileName);                  
                    lstFontDir.push_back(csFileName);
                    
                    SafeFreePointer(pFileName);
                }                
            }
        }
        SafeDeletePointer(pArchive);
    }
    return lstFontDir.size() > 0;
}

bool DKFontManager::IsFontsDirUpdated ()
{
    //DebugPrintf(DLC_DIAGNOSTIC, "DKFontManager::IsFontsDirUpdated Begin");
    int iNewSize (0);
    vector<string>::iterator it;
    for (it = m_lstFontDir.begin (); it < m_lstFontDir.end (); ++it)
    {
        iNewSize += GetDirFileSize (*it);
    }
    //DebugPrintf(DLC_DIAGNOSTIC, "DKFontManager::IsFontsDirUpdated iNewSize = %d, m_iFontSizeRecord = %d", iNewSize, m_iFontSizeRecord);
    DebugPrintf (DLC_LIUHJ, "DKFontManager::IsFontsDirUpdated old size %d, new size %d", m_iFontSizeRecord, iNewSize);
    return (iNewSize != m_iFontSizeRecord);
}

void DKFontManager::RefreshAllFontsInfo()
{
    //DebugPrintf(DLC_DIAGNOSTIC, "DKFontManager::RefreshAllFontsInfo Begin");
    m_iFontSizeRecord = 0;
    DK_UINT iSize = m_lstFontDir.size();
    
    vector<DKFont*>::iterator viFont;
    for (viFont = m_lstFont.begin(); viFont != m_lstFont.end();)
    {
        (*viFont)->Release();
        viFont = m_lstFont.erase(viFont);
    }
    
    for (unsigned int iStart = 0; iStart < iSize; ++iStart)
    {
        string &csFontDir = m_lstFontDir[iStart];
        DebugPrintf(DLC_JUGH, "DKFontManager::RefreshAllFontsInfo %s!!!", csFontDir.c_str());  
        AddFontsDir(csFontDir);
    }
    DebugPrintf(DLC_DIAGNOSTIC, "DKFontManager::RefreshAllFontsInfo end : iSize = %d", iSize);
}

bool DKFontManager::GetFontListByCodePage(unsigned short wCodePage, vector<DKFont*> &lstFont)
{
    if (m_lstFont.size() <= 0)
    {
        DebugPrintf(DLC_JUGH, "DKFontManager::HAS FONTLIST IS NULL!!!");  
        return DK_FALSE;
    }
    
    lstFont.clear();
    vector<DKFont*>::iterator viFont;
    const char* namePrevents[] = {"/dk-symbol.ttf"};
    for (viFont = m_lstFont.begin(); viFont != m_lstFont.end(); viFont++)
    {
        vector<DK_UINT16> vCodepage;
        if ((1252 != wCodePage) && 0 == strcmp("/DuoKan/res/sysfonts/lingoes.ttf", (*viFont)->GetFontPath()))
        {
            continue;
        }
        bool prevent = false;
        for (size_t k = 0; k < DK_DIM(namePrevents); ++k)
        {
            if (StringUtil::EndWith((*viFont)->GetFontPath(), namePrevents[k]))
            {
                prevent = true;
                break;
            }
        }
        if (prevent)
        {
            continue;
        }
        
        
#ifdef DEBUG_FONTMANAGER_INFO
        cout  << "DKFontManager::GetFontPath = " << (*viFont)->GetFontPath() << endl;
#endif
        if ((*viFont)->GetSupportedCodePage(vCodepage))
        {
#ifdef DEBUG_FONTMANAGER_INFO
            cout << "DKFontManager::wCodePage = " << wCodePage << endl;
            for (vector<DK_UINT16>::iterator vi = vCodepage.begin(); vi != vCodepage.end(); ++vi)
            {
                cout << *vi << "\t";
            }
            cout << endl;
#endif
            vector<DK_UINT16>::iterator viCp = find(vCodepage.begin(), vCodepage.end(), wCodePage);
            if (viCp != vCodepage.end())
            {
                lstFont.push_back(*viFont);
            }
        }
        else
        {
            DebugPrintf(DLC_JUGH, "DKFontManager::GetFontListByCodePage FAILED!!!");
        }
    }

    // 对DKFont进行排序，将对DKFont中DKTTFInfo类型的成员变量进行大小比较
    sort (lstFont.begin (), lstFont.end (), DKFont());

    return lstFont.size() > 0;
}

bool DKFontManager::GetFontListByCharSet(DK_CHARSET_TYPE eCharSet, vector<DKFont*> &lstFont)
{
    DK_UINT16 uCodepage = DK_GetCPByChst(eCharSet);
    if (CODEPAGE_NOT_EXIST == uCodepage)
        return DK_FALSE;
    
    return GetFontListByCodePage(uCodepage, lstFont);
}

bool DKFontManager::AddDir(string& wsFontDir)
{
    assert (wsFontDir.length() > 0);
    if (std::find(m_lstFontDir.begin(), m_lstFontDir.end(), wsFontDir) != m_lstFontDir.end())
    {
        return true;
    }
    
    m_lstFontDir.push_back(wsFontDir);
    sort(m_lstFontDir.begin(), m_lstFontDir.end());
    return m_lstFontDir.size() > 0;
}

void DKFontManager::RemoveDir(string& wsFontDir)
{
    assert (wsFontDir.length() > 0);

    vector<string>::iterator vi = find(m_lstFontDir.begin(), m_lstFontDir.end(), wsFontDir); 
    if (vi != m_lstFontDir.end())
        m_lstFontDir.erase(vi);
}

bool DKFontManager::SetDefaultFontFaceNameForCodePage(unsigned short wCodePage, const wstring& strFaceName)
{
    return SetDefaultFontFaceNameForCharSet(DK_GetChstByCP(wCodePage), strFaceName); 
}

bool DKFontManager::SetDefaultFontFaceNameForCharSet(DK_CHARSET_TYPE eCharSet, const wstring& strFaceName) 
{
    DK_INT iEnd = sizeof(gs_lstCHARSET2FONT)/sizeof(DK_CHARSET2FACENAME);
    if (iEnd <= 0 )
    {
        DebugPrintf(DLC_JUGH, "DKFontManager::SetDefaultFontFaceNameForCharSet FAILED!!!");  
        return false;
    }
    
    for (DK_INT iStart = 0; iStart < iEnd; ++iStart)
    {
        DK_CHARSET2FACENAME &Temp = gs_lstCHARSET2FONT[iStart];
        if (Temp.eCharset == eCharSet)
        {
            Temp.strFaceName = strFaceName;
            return true;
        }
    }
    DebugPrintf(DLC_JUGH, "DKFontManager::SetDefaultFontFaceNameForCharSet eCharset ISNOT EXISTING!!!");  
    return false;
} 

bool DKFontManager::SetSystemDefaultFontFaceName(const wstring& strFaceName) 
{
    m_strSysDefFontFaceName = strFaceName;
    return true;
}

bool DKFontManager::GetDefaultFontFaceNameForCodePage(unsigned short wCodePage, wstring* pStrFaceName)
{
    return GetDefaultFontFaceNameForCharSet(DK_GetChstByCP(wCodePage), pStrFaceName); 
}

bool DKFontManager::GetDefaultFontFaceNameForCharSet(DK_CHARSET_TYPE eCharSet, wstring* pStrFaceName)
{
    assert (pStrFaceName);
    if (!pStrFaceName)
    {
        return false;
    }

    DK_INT iEnd = sizeof(gs_lstCHARSET2FONT)/sizeof(DK_CHARSET2FACENAME);
    if (iEnd <= 0 )
    {
        DebugPrintf(DLC_JUGH, "DKFontManager::GetDefaultFontFaceNameForCharSet FAILED!!!");  
        return false;
    }
    
    for (DK_INT iStart = 0; iStart < iEnd; ++iStart)
    {
        const DK_CHARSET2FACENAME &Temp = gs_lstCHARSET2FONT[iStart];
        if ((eCharSet == Temp.eCharset) && (NULL != GetFontForFaceName(Temp.strFaceName)))
        {
            *pStrFaceName = Temp.strFaceName;
            return true;
        }
    }
    DebugPrintf(DLC_JUGH, "DKFontManager::GetDefaultFontFaceNameForCharSet eCharset ISNOT EXISTING!!!");  
    return false;    
}

bool DKFontManager::GetSystemDefaultFontFaceName(wstring* pStrFaceName)
{
    assert (pStrFaceName);
    if (pStrFaceName)
    {
        *pStrFaceName = m_strSysDefFontFaceName;
    }
    return (DK_NULL != pStrFaceName);
}
    
int DKFontManager::GetDirFileSize (const std::string& wsFontDir)
{
    struct dirent* ent (NULL);
    DIR *pDir = opendir(wsFontDir.c_str ());
    if (!pDir)
    {
        return 0;
    }

    int iSumSize (0);
    char strFile [DK_MAX_PATH] = {0};

    while (NULL != (ent = readdir(pDir)))
    {
        if (!S_ISDIR (ent->d_type))
        {
            struct stat dirStat;
            if (strlen (wsFontDir.c_str ()) + strlen (ent->d_name) > DK_MAX_PATH)
            {
                continue;
            }

            snprintf (strFile, DK_MAX_PATH, "%s%s", wsFontDir.c_str (), ent->d_name);
            if (0 <= stat (strFile, &dirStat))
            {
                iSumSize += dirStat.st_size % 1000;     // 取整数末尾三位
            }
        }
    }

    closedir (pDir);

    return iSumSize;
}

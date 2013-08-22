///////////////////////////////////////////////////////////////////////
// idk_kdfnt.h
// Contact:
// Copyright (c) Duokan Corporation. All rights reserved.
//
////////////////////////////////////////////////////////////////////////

#ifndef _IDK_KDFNT_H_
#define _IDK_KDFNT_H_

#include "KernelBaseType.h"
#include "KernelType.h"
#include <vector>

using std::wstring;

typedef const DK_CHAR* DK_LPCSTR;

#define USER_FONT_PATH       "/mnt/us/DK_System/xKindle/res/userfonts/"
#define SYSTEM_FONT_PATH     "/DuoKan/res/sysfonts/"

// Classes Defined in other header
class DKTTFInfo;

// Classes Defined in this header
class DKFont;
class DKFontManager;

//Only for ttf/ttc font files
class DKFont
{
public:
    DKFont();
    ~DKFont();
    //LoadFont and parse to get the font information from ttf/ttc
    bool   LoadFont(std::string& wsFileName);
    void   Release();

    int      GetFontStyle();
    std::string GetChineseFontName() const;
    std::vector<std::wstring>*   GetFullFontName() const;
    std::vector<std::wstring>*   GetFontFamilyName();    
    std::vector<std::wstring>*   GetFontSubFamilyName();
    bool     GetSupportedCodePage(std::vector<unsigned short>& lstCodepage);
    bool     GetSupportedCharSet(std::vector<unsigned short>&  lstCharSet);
    bool IsSupportingCharSet(DK_CHARSET_TYPE charSet);
    const char* GetFontPath(bool bFullPath = DK_TRUE);
    bool operator()(const DKFont* pLeft, const DKFont* pRight);

private:
    DKTTFInfo* m_pTTFInfo;
    std::string    m_pFilePath;
    std::string    m_pFileDir;
};

class DKFontManager
{
public:
    DKFontManager();
    virtual ~DKFontManager();
    
    virtual void Release();
 
    virtual bool    AddFont(DKFont* pFont);    
    virtual void    RemoveFont(DKFont* pFont);        
    virtual DKFont* GetFont(std::string& wsFontPath);
    virtual DKFont* GetFontForFaceName(const std::wstring& wsFaceName);
    virtual bool    GetAllFont(std::vector<DKFont*> &lstFont);
    
    virtual bool    AddFontsDir(std::string& wsFontDir);    
    virtual void    RemoveFontsDir(std::string& wsFontDir);
    virtual bool    GetFontsDir(std::vector<std::string>& lstFontDir);
    
    virtual bool GetAllFontsDir(std::string& wsFontPath, std::vector<std::string> &lstFontDir);    
    
    // 已添加的文件夹下，字体文件是否有变动 （如添加、删除）
    virtual bool IsFontsDirUpdated ();
    virtual void RefreshAllFontsInfo();
    
    virtual bool GetFontListByCodePage(unsigned short wCodePage, std::vector<DKFont*> &lstFont);
    virtual bool GetFontListByCharSet(DK_CHARSET_TYPE eCharSet, std::vector<DKFont*> &lstFont);    

    virtual bool SetDefaultFontFaceNameForCodePage(unsigned short wCodePage, const wstring& strFaceName);
    virtual bool SetDefaultFontFaceNameForCharSet(DK_CHARSET_TYPE eCharSet, const wstring& strFaceName);
    virtual bool SetSystemDefaultFontFaceName(const wstring& strFaceName);

    virtual bool GetDefaultFontFaceNameForCodePage(unsigned short wCodePage, wstring* pStrFaceName);
    virtual bool GetDefaultFontFaceNameForCharSet(DK_CHARSET_TYPE eCharSet, wstring* pStrFaceName);
    virtual bool GetSystemDefaultFontFaceName(wstring* pStrFaceName);

    // 获取所有目录下文件的大小之和,用于判断文件夹下的文件更改
    int    GetDirFileSize (const std::string& wsFontDir = std::string(USER_FONT_PATH));

protected:
    bool   AddDir(std::string& wsFontDir);
    void   RemoveDir(std::string& wsFontDir);

    std::vector<DKFont*>       m_lstFont;
    std::vector<std::string>   m_lstFontDir;
    std::wstring               m_strSysDefFontFaceName;
    int                        m_iFontSizeRecord;       // 记录所有字体大小之和, 只针对通过 AddFontsDir ()添加的目录
};
extern DKFontManager* g_pFontManager;
#endif // _IDK_KDFNT_H_



#include "GUI/FCFontManager.h"
#include <dirent.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>		  	    
#ifdef KINDLE_FOR_TOUCH	  
#include "TouchAppUI/UISystemSettingPage.h"
#else					
#include "TouchAppUI/UISystemSettingPage.h"
#endif
#include "Common/ConfigInfo.h"
#include "Framework/BackgroundLoader.h"
#include "interface.h"

//#define DEBUG_FONT_LIST

#define PATTERN_SUPPORT_CNS                    (":lang=zh")
#define PATTERN_SUPPORT_EN                    (":lang=en")
#define USER_FONT_PATH                              ("/mnt/us/DK_System/xKindle/res/userfonts/")
#define SYS_FONT_PATH                                ("./res/sysfonts/")

#define CONF_FILE_PATH                              ("./res/fontconfig/conf.d/69-language-selector-zh-cn.conf")
#define CONF_ROOT_HEAD                            ("<?xml version=\"1.0\"?>\n<!DOCTYPE fontconfig SYSTEM \"fonts.dtd\">\n<fontconfig>")
#define CONF_ROOT_TAIL                              ("\n</fontconfig>")
#define CONF_MATCH_HEAD                         ("\n\t<match target=\"pattern\">")
#define CONF_MATCH_TAIL                           ("\n\t</match>")
#define CONF_TEST_HEAD                             ("\n\t\t<test compare=\"contains\" name=\"family\">")
#define CONF_TEST_TAIL                               ("\n\t\t</test>")
#define CONF_EDIT_HEAD                             ("\n\t\t<edit name=\"family\" mode=\"prepend\" binding=\"strong\">")
#define CONF_EDIT_TAIL                               ("\n\t\t</edit>")
#define FONT_TYPE_SERIF                            ("\n\t\t\t<string>serif</string>")
#define FONT_TYPE_SANS_SERIF                  ("\n\t\t\t<string>sans-serif</string>")
#define FONT_TYPE_MONO                           ("\n\t\t\t<string>sans-serif</string>")
#define CONF_FONT_TAG_MAX_LENGTH                (3 * MAX_FONT_NAME_LENGTH + 128)

static CHAR strCNSFontTable [MAX_FONT_COUNT][MAX_FONT_NAME_LENGTH] = {{0}};
static CHAR strENFontTable [MAX_FONT_COUNT][MAX_FONT_NAME_LENGTH] = {{0}};
static CHAR strLocalFontTable [MAX_FONT_COUNT][MAX_FONT_NAME_LENGTH] = {{0}};
static INT iCNSFontCount (0);
static INT iENFontCount (0);
static INT iLocalFontCount (0);
static INT iFontFileSizeRecord (0);

void CFCFontManager::InitFCAtSysStart ()
{
    // 记录当前字体文件夹大小，记录值将与插拔usb后的字体目录大小比较
    iFontFileSizeRecord = CFCFontManager::GetDirFilesSize (USER_FONT_PATH) + CFCFontManager::GetDirFilesSize (SYS_FONT_PATH);

    // 写入fontconfig配置文件，使fontconfig初始配置与lite系统配置同步 （系统加载时应保证系统配置文件已初始化完毕）
    CFCFontManager::WriteFCConfFile (
            SystemSettingInfo::GetInstance()->GetConfigValue(CKI_EnglishFont),
            strlen (SystemSettingInfo::GetInstance()->GetConfigValue(CKI_EnglishFont)),
            SystemSettingInfo::GetInstance()->GetConfigValue(CKI_LocalFont),
            strlen (SystemSettingInfo::GetInstance()->GetConfigValue(CKI_LocalFont)),
            SystemSettingInfo::GetInstance()->GetConfigValue(CKI_ChineseFont),
            strlen (SystemSettingInfo::GetInstance()->GetConfigValue(CKI_ChineseFont)));
}

HRESULT CFCFontManager::GetAvailableFontList (DK_FONT_CLASS eFontClass, PCHAR pstrFontNameTable, INT iTableMaxRow, INT iTableMaxCol, PUINT puListCount)
{
    if (NULL == pstrFontNameTable ||
        0 >= iTableMaxRow ||
        0 >= iTableMaxCol ||
        NULL == puListCount)
    {
        return E_INVALIDARG;
    }

    CBackgroundLoader::WaitForCheckingFontsCache();

    INT iFontCount (0);
    PCCH pstrFontTable (NULL);
    switch (eFontClass)
    {
    case DK_FONT_CLASS_ENGLISH:
        iFontCount = iENFontCount;
        pstrFontTable = (PCHAR)strENFontTable;
        break;

    case DK_FONT_CLASS_LOCAL:
        iFontCount = iLocalFontCount;
        pstrFontTable = (PCHAR)strLocalFontTable;
        break;

    case DK_FONT_CLASS_CHINESE:
        iFontCount = iCNSFontCount;
        pstrFontTable = (PCHAR)strCNSFontTable;
        break;

    default:
        iFontCount = iENFontCount;
        pstrFontTable = (PCHAR)strENFontTable;
    }

    UINT uCopyLength = iTableMaxCol < MAX_FONT_NAME_LENGTH ? iTableMaxCol : MAX_FONT_NAME_LENGTH;
    UINT uCopyCount = iTableMaxRow < iFontCount ? iTableMaxRow : iFontCount;

    for (UINT uIndex= 0; uIndex < uCopyCount; uIndex++)
    {
        snprintf (pstrFontNameTable + (iTableMaxCol * uIndex * sizeof (CHAR)), uCopyLength, "%s", pstrFontTable + (MAX_FONT_NAME_LENGTH * uIndex * sizeof (CHAR)));
    }

    *puListCount = iFontCount;

    return S_OK;
}

HRESULT CFCFontManager::CheckFontsCache()
{
    DebugPrintf (DLC_LIUHJ, "CheckFontsCache entrance");

   if (FAILED(CFCFontManager::ReinitializeFC ()))
    {
        return E_FAIL;
    }

    if (!FcInit ())
    {
        return E_FAIL;
    }

    InitCNSFontList();
    InitENFontList ();
    InitLocalFontList ();
    CleanMultiNames (strCNSFontTable, iCNSFontCount);
    CleanMultiNames (strENFontTable, iENFontCount);
    CleanMultiNames (strLocalFontTable, iLocalFontCount);

#ifdef DEBUG_FONT_LIST
    INT i (0);
    for (i=0; i < iCNSFontCount; i++)
    {
        DebugPrintf (DLC_LIUHJ, "CNS %s", strCNSFontTable[i]);
    }
    for (i=0; i < iENFontCount; i++)
    {
        DebugPrintf (DLC_LIUHJ, "EN %s", strENFontTable[i]);
    }
    for (i=0; i < iLocalFontCount; i++)
    {
        DebugPrintf (DLC_LIUHJ, "Local %s", strLocalFontTable[i]);
    }
#endif

    DebugPrintf (DLC_LIUHJ, "CheckFontsCache exits\n");

    return S_OK;
}

HRESULT CFCFontManager::InitCNSFontList()
{
    memset (strCNSFontTable, 0, MAX_FONT_COUNT * MAX_FONT_NAME_LENGTH);

    FcPattern *pattern = FcNameParse ((FcChar8*)PATTERN_SUPPORT_CNS);
    if (NULL == pattern)
    {
        return E_FAIL;
    }
    FcObjectSet *fcObjectSet = FcObjectSetBuild (FC_FAMILY, 0, (PCHAR) 0);
    if (NULL == fcObjectSet)
    {
        FcPatternDestroy (pattern);
        return E_FAIL;
    }
    FcFontSet *fcFontSet = FcFontList (0, pattern, fcObjectSet);
    FcObjectSetDestroy (fcObjectSet);
    FcPatternDestroy (pattern);

    if (NULL == fcFontSet)
    {
        return E_FAIL;
    }

    iCNSFontCount = 0;
    INT iFontMaxCount = fcFontSet->nfont > MAX_FONT_COUNT ? MAX_FONT_COUNT : fcFontSet->nfont;

    INT i (0);
    INT j (0);
    INT k (0);
    INT iStrLen (0);
    INT iCopyLen (0);
    while (i < fcFontSet->nfont &&
           iCNSFontCount < iFontMaxCount)
    {
        FcChar8 *strFontName = FcNameUnparse (fcFontSet->fonts[i]);

        if (NULL != strFontName)
        {
            iStrLen = 0;
            while ('\0' != strFontName[iStrLen])
            {
                iStrLen++;
            }

            iCopyLen = iStrLen  + 1 > MAX_FONT_NAME_LENGTH ? MAX_FONT_NAME_LENGTH : iStrLen + 1;

            if (0 == iCNSFontCount)
            {
                snprintf (strCNSFontTable[iCNSFontCount++], iCopyLen, "%s", strFontName);
            }
            else
            {
                for (j = 0; j < iCNSFontCount; j++)
                {
                    if (0 > strcmp ((PCHAR)strFontName, strCNSFontTable[j]))
                    {
                        break;
                    }
                }

                for (k = iCNSFontCount - 1; k >= j; k--)
                {
                    memcpy (strCNSFontTable[k + 1], strCNSFontTable[k], sizeof (CHAR) * MAX_FONT_NAME_LENGTH);
                }

                snprintf (strCNSFontTable[j], iCopyLen, "%s", strFontName);

                iCNSFontCount++;
            }

            free (strFontName);
        }
        i++;
    }

    FcFontSetDestroy (fcFontSet);
    return S_OK;
}

HRESULT CFCFontManager::InitENFontList ()
{
    memset (strENFontTable, 0, MAX_FONT_COUNT * MAX_FONT_NAME_LENGTH);

    FcPattern *pattern = FcNameParse ((FcChar8*)PATTERN_SUPPORT_EN);
    if (NULL == pattern)
    {
        return E_FAIL;
    }

    FcObjectSet *fcObjectSet = FcObjectSetBuild (FC_FAMILY, 0, (PCHAR) 0);
    if (NULL == fcObjectSet)
    {
        FcPatternDestroy (pattern);
        return E_FAIL;
    }

    FcFontSet *fcFontSet = FcFontList (0, pattern, fcObjectSet);
    FcObjectSetDestroy (fcObjectSet);
    FcPatternDestroy (pattern);

    if (NULL == fcFontSet)
    {
        return E_FAIL;
    }

    iENFontCount = 0;
    INT iFontMaxCount = fcFontSet->nfont > MAX_FONT_COUNT ? MAX_FONT_COUNT : fcFontSet->nfont;

    INT i (0);
    INT j (0);
    INT k (0);
    INT iStrLen (0);
    INT iCopyStart (0);
    INT iCopyLen (0);
    BOOL fIsCNSFont (FALSE);
    BOOL fIsMultiNames (FALSE);
    while (i < fcFontSet->nfont &&
           iENFontCount < iFontMaxCount)
    {
        FcChar8 *strFontName = FcNameUnparse (fcFontSet->fonts[i]);

        if (NULL != strFontName)
        {
            for (j = 0; j < iCNSFontCount; j++)
            {
                if (0 == strcmp ((PCHAR)strFontName, strCNSFontTable[j]))
                {
                    fIsCNSFont = TRUE;
                    break;
                }
            }

            if (fIsCNSFont)
            {
                free (strFontName);
                fIsCNSFont = FALSE;
                i++;
                continue;
            }

            iStrLen = 0;
            iCopyStart = 0;
            while ('\0' != strFontName[iStrLen])
            {
                if (127 < strFontName[iStrLen])
                {
                    fIsMultiNames = TRUE;
                    break;
                }

                if (',' == strFontName[iStrLen++])
                {
                    iCopyStart = iStrLen;
                }
            }

            if (fIsMultiNames)
            {
                free (strFontName);
                fIsMultiNames = FALSE;
                i++;
                continue;
            }

            iCopyLen = iStrLen - iCopyStart + 1 > MAX_FONT_NAME_LENGTH ? MAX_FONT_NAME_LENGTH : iStrLen - iCopyStart + 1;

            if (0 == iENFontCount)
            {
                snprintf (strENFontTable[iENFontCount++], iCopyLen, "%s", strFontName + iCopyStart);
            }
            else
            {
                for (j = 0; j < iENFontCount; j++)
                {
                    if (0 > strcmp ((PCHAR)strFontName + iCopyStart, strENFontTable[j]))
                    {
                        break;
                    }
                }

                for (k = iENFontCount - 1; k >= j; k--)
                {
                    memcpy (strENFontTable[k + 1], strENFontTable[k], sizeof (CHAR) * MAX_FONT_NAME_LENGTH);
                }

                snprintf (strENFontTable[j], iCopyLen, "%s", strFontName + iCopyStart);
                iENFontCount++;
            }

            free (strFontName);
        }
        i++;
    }

    FcFontSetDestroy (fcFontSet);
    return S_OK;
}

HRESULT CFCFontManager::InitLocalFontList ()
{
    memset (strLocalFontTable, 0, MAX_FONT_COUNT * MAX_FONT_NAME_LENGTH);

    FcPattern *pattern = FcPatternCreate ();
    if (NULL == pattern)
    {
        return E_FAIL;
    }

    FcObjectSet *fcObjectSet = FcObjectSetBuild (FC_FAMILY, 0, (PCHAR) 0);
    if (NULL == fcObjectSet)
    {
        FcPatternDestroy (pattern);
        return E_FAIL;
    }

    FcFontSet *fcFontSet = FcFontList (0, pattern, fcObjectSet);
    FcObjectSetDestroy (fcObjectSet);
    FcPatternDestroy (pattern);

    if (NULL == fcFontSet)
    {
        return E_FAIL;
    }

    iLocalFontCount = 0;
    INT iFontMaxCount = fcFontSet->nfont > MAX_FONT_COUNT ? MAX_FONT_COUNT : fcFontSet->nfont;

    INT i (0);
    INT j (0);
    INT k (0);
    INT iStrLen (0);
    INT iCopyStart (0);
    INT iCopyLen (0);
    BOOL fIsCNSFont (FALSE);
    while (i < fcFontSet->nfont &&
           iLocalFontCount < iFontMaxCount)
    {
        FcChar8 *strFontName = FcNameUnparse (fcFontSet->fonts[i]);

        if (NULL != strFontName)
        {
            for (j = 0; j < iCNSFontCount; j++)
            {
                if (0 == strcmp ((PCHAR)strFontName, strCNSFontTable[j]))
                {
                    fIsCNSFont = TRUE;
                    break;
                }
            }

            if (fIsCNSFont)
            {
                free (strFontName);
                fIsCNSFont = FALSE;
                i++;
                continue;
            }

            iStrLen = 0;
            iCopyStart = 0;
            while ('\0' != strFontName[iStrLen])
            {
                if (',' == strFontName[iStrLen++])
                {
                    iCopyStart = iStrLen;
                }
            }

            iCopyLen = iStrLen - iCopyStart + 1 > MAX_FONT_NAME_LENGTH ? MAX_FONT_NAME_LENGTH : iStrLen - iCopyStart + 1;

            if (0 == iLocalFontCount)
            {
                snprintf (strLocalFontTable[iLocalFontCount++], iCopyLen, "%s", strFontName + iCopyStart);
            }
            else
            {
                for (j = 0; j < iLocalFontCount; j++)
                {
                    if (0 > strcmp ((PCHAR)strFontName + iCopyStart, strLocalFontTable[j]))
                    {
                        break;
                    }
                }

                for (k = iLocalFontCount - 1; k >= j; k--)
                {
                    memcpy (strLocalFontTable[k + 1], strLocalFontTable[k], sizeof (CHAR) * MAX_FONT_NAME_LENGTH);
                }

                snprintf (strLocalFontTable[j], iCopyLen, "%s", strFontName + iCopyStart);
                iLocalFontCount++;
            }

            free (strFontName);
        }
        i++;
    }

    FcFontSetDestroy (fcFontSet);
    return S_OK;
}

void CFCFontManager::CleanMultiNames (CHAR strTable[MAX_FONT_COUNT][MAX_FONT_NAME_LENGTH], UINT uCount)
{
    UINT i (0);
    INT j (0);
    INT k (0);
    INT iCopyStart (0);
    for (; i < uCount; i++)
    {
        iCopyStart = 0;
        j = 0;
        while ('\0' != strTable[i][j])
        {
            if (',' == strTable[i][j++])
            {
                iCopyStart = j;
            }
        }

        if (0 == iCopyStart)
        {
            continue;
        }

        k = 0;
        j = iCopyStart;
        while ('\0' != strTable[i][j])
        {
            strTable[i][k++] = strTable[i][j++];
        }
        strTable[i][k] = '\0';
    }
}

HRESULT CFCFontManager::SetFontName (DK_FONT_CLASS eFontClass, PCCH pstrFontName, INT iLength)
{
    HRESULT hr (E_FAIL);

    switch (eFontClass)
    {
    case DK_FONT_CLASS_ENGLISH:
        hr = CFCFontManager::WriteFCConfFile (
                pstrFontName,
                iLength,
                SystemSettingInfo::GetInstance()->GetConfigValue(CKI_LocalFont),
                strlen (SystemSettingInfo::GetInstance()->GetConfigValue(CKI_LocalFont)),
                SystemSettingInfo::GetInstance()->GetConfigValue(CKI_ChineseFont),
                strlen (SystemSettingInfo::GetInstance()->GetConfigValue(CKI_ChineseFont)));
        break;

    case DK_FONT_CLASS_LOCAL:
        hr = CFCFontManager::WriteFCConfFile (
                SystemSettingInfo::GetInstance()->GetConfigValue(CKI_EnglishFont),
                strlen (SystemSettingInfo::GetInstance()->GetConfigValue(CKI_EnglishFont)),
                pstrFontName,
                iLength,
                SystemSettingInfo::GetInstance()->GetConfigValue(CKI_ChineseFont),
                strlen (SystemSettingInfo::GetInstance()->GetConfigValue(CKI_ChineseFont)));
        break;

    case DK_FONT_CLASS_CHINESE:
        hr = CFCFontManager::WriteFCConfFile (
                SystemSettingInfo::GetInstance()->GetConfigValue(CKI_EnglishFont),
                strlen (SystemSettingInfo::GetInstance()->GetConfigValue(CKI_EnglishFont)),
                SystemSettingInfo::GetInstance()->GetConfigValue(CKI_LocalFont),
                strlen (SystemSettingInfo::GetInstance()->GetConfigValue(CKI_LocalFont)),
                pstrFontName,
                iLength);
        break;

    default:
        hr = CFCFontManager::WriteFCConfFile (
                pstrFontName,
                iLength,
                SystemSettingInfo::GetInstance()->GetConfigValue(CKI_LocalFont),
                strlen (SystemSettingInfo::GetInstance()->GetConfigValue(CKI_LocalFont)),
                SystemSettingInfo::GetInstance()->GetConfigValue(CKI_ChineseFont),
                strlen (SystemSettingInfo::GetInstance()->GetConfigValue(CKI_ChineseFont)));
    }

    return hr;
}

HRESULT CFCFontManager::WriteFCConfFile (
        PCCH pstrENFontName,
        INT iENLength,
        PCCH pstrLocalFontName,
        INT iLocalLength,
        PCCH pstrCNSFontName,
        INT iCNSLength)
{
    DebugPrintf (DLC_LIUHJ, "WriteFCConfFile entrance");

    if (NULL == pstrENFontName ||
        NULL == pstrLocalFontName ||
        NULL == pstrCNSFontName ||
        MAX_FONT_NAME_LENGTH < iENLength ||
        MAX_FONT_NAME_LENGTH < iLocalLength ||
        MAX_FONT_NAME_LENGTH < iCNSLength)
    {
        return E_INVALIDARG;
    }

    CHAR strFontTag[CONF_FONT_TAG_MAX_LENGTH];
    strFontTag[0] = '0';
    snprintf (strFontTag,
              CONF_FONT_TAG_MAX_LENGTH,
              "\n\t\t\t<string>%s</string>\n\t\t\t<string>%s</string>\n\t\t\t<string>%s</string>",
             pstrENFontName,
             pstrLocalFontName,
             pstrCNSFontName);      // 英文字体名需放在中文字体名之前

    CHAR strConfContent[2048];
    strConfContent[0] = '0';
    snprintf (strConfContent,
              2048,
              "%s%s%s%s%s%s%s%s%s%s\n",
             CONF_ROOT_HEAD,
             CONF_MATCH_HEAD,
             CONF_TEST_HEAD,
             FONT_TYPE_SERIF,
             CONF_TEST_TAIL,
             CONF_EDIT_HEAD,
             strFontTag,
             CONF_EDIT_TAIL,
             CONF_MATCH_TAIL,
             CONF_ROOT_TAIL);

    SystemUtil::ExecuteCommand"mntroot rw \n");

    FILE *fp = fopen(CONF_FILE_PATH, "w");
    if (NULL == fp)
    {
        return E_FAIL;
    }

    fwrite (strConfContent, strlen(strConfContent), 1, fp);
    fclose(fp);

    DebugPrintf (DLC_LIUHJ, "WriteFCConfFile exits\n");

    return S_OK;
}

BOOL CFCFontManager::IsFontDirModified ()
{
    INT iCurFileSize = CFCFontManager::GetDirFilesSize (USER_FONT_PATH) + CFCFontManager::GetDirFilesSize (SYS_FONT_PATH);

    DebugPrintf (DLC_LIUHJ, "CFCFontManager::IsFontDirModified - old size count %d, new size count %d", iFontFileSizeRecord, iCurFileSize);

    if (0 == iFontFileSizeRecord)
    {
        iFontFileSizeRecord = iCurFileSize;
    }

    if ( 0 < iCurFileSize &&
         iCurFileSize != iFontFileSizeRecord)
    {
        iFontFileSizeRecord = iCurFileSize;

        return TRUE;
    }

    return FALSE;
}

INT CFCFontManager::GetDirFilesSize (PCCH pcchDir)
{
    struct dirent* ent (NULL);
    DIR *pDir = opendir(pcchDir);
    if (NULL == pDir)
    {
        return 0;
    }

    INT iTempSize (-1);

    CHAR strFile [MAX_FILE_NAME_LENGTH];
    strFile [0] = '0';

    while (NULL != (ent = readdir(pDir)))
    {
        if (!S_ISDIR (ent->d_type))
        {
            struct stat dirStat;

            if (strlen (pcchDir) + strlen (ent->d_name) > MAX_FILE_NAME_LENGTH)
            {
                continue;
            }

            snprintf (strFile, MAX_FILE_NAME_LENGTH, "%s%s", pcchDir, ent->d_name);
            if (0 <= stat (strFile, &dirStat))
            {
                iTempSize += dirStat.st_size % 1000;
            }
        }
    }

    closedir (pDir);

    return iTempSize;
}

HRESULT CFCFontManager::ReinitializeFC ()
{
    DebugPrintf (DLC_LIUHJ, "ReinitializeFC\n");

    SystemUtil::ExecuteCommand"mntroot rw \n");
    FcBool fIsSucceeded = FcInitReinitialize ();

    return fIsSucceeded ? S_OK : E_FAIL;
}



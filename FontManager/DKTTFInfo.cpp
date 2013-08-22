#include "DKTTFInfo.h"
#include "KernelMacro.h"
#include "DkStream.h"
#include "DkStreamFactory.h"
#include "KernelEncoding.h"
#include "interface.h"
#include <stdio.h>
#include <iostream>
#include <stdint.h>
#include "Utility/EncodeUtil.h"

using namespace dk::utility;
using std::vector;
using std::string;
using std::wstring;

//#define DEBUG_FONT_INFO
#ifdef DEBUG_FONT_INFO
#include <iostream.h> 
#endif

/************************************************
*              一些实现才需要的结构定义            *
************************************************/

// tables in ttf file and there placement and name
typedef struct _tagTTF_TABLE_ENTRY
{
    char  csTag[4];    // table name
    uint32_t ulCheckSum;  // Check sum
    uint32_t ulOffset;    // Offset from beginning of file
    uint32_t ulLength;    // length of the table in bytes
}TTF_TABLE_ENTRY;
typedef TTF_TABLE_ENTRY* LST_TTF_TABLE_ENTRY;

// this is ttf file header
typedef struct _tagTTF_TABLE_DIRECTORY
{
     unsigned short  uMajorVersion;
     unsigned short  uMinorVersion;
     unsigned short  uNumOfTables;    // Number of tables.
     unsigned short  uSearchRange;    // (Maximum power of 2 <= numTables) x 16
     unsigned short  uEntrySelector;  // Log2(maximum power of 2 <= numTables).
     unsigned short  uRangeShift;     // NumTables x 16-searchRange
} TTF_TABLE_DIRECTORY;

// this is ttc file header
typedef struct _tagTTC_TABLE_ENTRY
{
    char    csTag[4];      // "ttcf"
     unsigned short  uMajorVersion;
     unsigned short  uMinorVersion;
    unsigned long   ulNumFonts;    // Number of fonts in TTC
} TTC_TABLE_ENTRY;

// Header of names table
typedef struct _tagTTF_NAME_TABLE_HEADER
{
    unsigned short uFSelector;       // format selector. Always 0
    unsigned short uNameRecordCount;
    unsigned short uStorageOffset;   // offset from strings storage,
                                // from start of the table
} TTF_NAME_TABLE_HEADER;

// Record in names table
typedef struct _tagTT_NAME_RECORD
{
    unsigned short uPlatfromID;
    unsigned short uEncodingID;
    unsigned short uLanguageID;
    unsigned short uNameID;
    unsigned short uStringLength;
    unsigned short uStringOffset; // from start of storage area
} TT_NAME_RECORD;

// OS/2 table
#pragma pack(push,2)
typedef struct _tagTT_OS2_TABLE_0
{ 
    unsigned short        uVersion; 
    short         xAvgCharWidth;
    unsigned short        usWeightClass;
    unsigned short        usWidthClass;
    unsigned short        fsType;
    short         ySubscriptXSize;
    short         ySubscriptYSize;
    short         ySubscriptXOffset;
    short         ySubscriptYOffset;
    short         ySuperscriptXSize;
    short         ySuperscriptYSize;
    short         ySuperscriptXOffset;
    short         ySuperscriptYOffset;
    short         yStrikeoutSize;
    short         yStrikeoutPosition;
    short         sFamilyClass;
    unsigned char          panose[10];
    unsigned long         ulCharRange[4];
    char          achVendID[4];
    unsigned short        uSelection;          // Contains information concerning the nature of the font patterns
    unsigned short        usFirstCharIndex;
    unsigned short        usLastCharIndex;
    short         sTypoAscender;
    short         sTypoDescender;
    short         sTypoLineGap;
    unsigned short        usWinAscent;
    unsigned short        usWinDescent;
} TT_OS2_TABLE_0;

typedef struct _tagTT_OS2_TABLE_1
{ 
    unsigned short        uVersion; 
    short         xAvgCharWidth;
    unsigned short        usWeightClass;
    unsigned short        usWidthClass;
    unsigned short        fsType;
    short         ySubscriptXSize;
    short         ySubscriptYSize;
    short         ySubscriptXOffset;
    short         ySubscriptYOffset;
    short         ySuperscriptXSize;
    short         ySuperscriptYSize;
    short         ySuperscriptXOffset;
    short         ySuperscriptYOffset;
    short         yStrikeoutSize;
    short         yStrikeoutPosition;
    short         sFamilyClass;
    unsigned char          panose[10];
    unsigned long         ulUnicodeRange1;
    unsigned long         ulUnicodeRange2;
    unsigned long         ulUnicodeRange3;
    unsigned long         ulUnicodeRange4;
    char          achVendID[4];
    unsigned short        uSelection;          // Contains information concerning the nature of the font patterns
    unsigned short        usFirstCharIndex;
    unsigned short        usLastCharIndex;
    short         sTypoAscender;
    short         sTypoDescender;
    short         sTypoLineGap;
    unsigned short        usWinAscent;
    unsigned short        usWinDescent;
    unsigned long         ulCodePageRange1;    // Code Page Character Range Bits 0-31 
    unsigned long         ulCodePageRange2;    // Code Page Character Range Bits 32-63 
} TT_OS2_TABLE_1;

typedef struct _tagTT_OS2_TABLE_234
{ 
    unsigned short        uVersion; 
    short         xAvgCharWidth;
    unsigned short        usWeightClass;
    unsigned short        usWidthClass;
    unsigned short        fsType;
    short         ySubscriptXSize;
    short         ySubscriptYSize;
    short         ySubscriptXOffset;
    short         ySubscriptYOffset;
    short         ySuperscriptXSize;
    short         ySuperscriptYSize;
    short         ySuperscriptXOffset;
    short         ySuperscriptYOffset;
    short         yStrikeoutSize;
    short         yStrikeoutPosition;
    short         sFamilyClass;
    unsigned char          panose[10];
    unsigned long         ulUnicodeRange1;
    unsigned long         ulUnicodeRange2;
    unsigned long         ulUnicodeRange3;
    unsigned long         ulUnicodeRange4;
    char          achVendID[4];
    unsigned short        uSelection;          // Contains information concerning the nature of the font patterns
    unsigned short        usFirstCharIndex;
    unsigned short        usLastCharIndex;
    short         sTypoAscender;
    short         sTypoDescender;
    short         sTypoLineGap;
    unsigned short        usWinAscent;
    unsigned short        usWinDescent;
    unsigned long         ulCodePageRange1;    // Code Page Character Range Bits 0-31 
    unsigned long         ulCodePageRange2;    // Code Page Character Range Bits 32-63 
    short         sxHeight;
    short         sCapHeight;
    unsigned short        usDefaultChar;
    unsigned short        usBreakChar;  
    unsigned short        usMaxContext;  
} TT_OS2_TABLE_234;
#pragma pack(pop)

/************************************************
* ENCODING TYPE
************************************************/
typedef enum _tagTT_ENCODING
{
    eTT_ENCODING_CHAR = 0,
    eTT_ENCODING_GBK,
    eTT_ENCODING_BIG5,
    eTT_ENCODING_UCS2,
    eTT_ENCODING_UCS4
} TT_ENCODING;

#define   LANGUAGE_CHINESE_HK          0x0C04
#define   LANGUAGE_CHINESE_MACAO       0x1404
#define   LANGUAGE_CHINESE_PRC         0x0804
#define   LANGUAGE_CHINESE_SINGAPORE   0x1004
#define   LANGUAGE_CHINESE_TAIWAN      0x0404

/************************************************
* TTF files are stored in Big-Endian format
************************************************/
#define HIBYTE(w)   ((unsigned char) (((unsigned short) (w) >> 8) & 0xFF))
#define LOBYTE(w)   ((unsigned char) (w))
#define HIWORD(l)   ((unsigned short) (((unsigned int) (l) >> 16) & 0xFFFF))
#define LOWORD(l)   ((unsigned short) (l))

#define MAKEWORD(a, b)  ((unsigned short) (((unsigned char)(a)) | ((unsigned short) ((unsigned char)(b))) << 8)) 
#define SWAPWORD(x) MAKEWORD(HIBYTE(x), LOBYTE(x))

#define MAKELONG(a, b)  ((long) (((unsigned short)(a)) | ((unsigned int)((unsigned short)(b))) << 16))
#define SWAPLONG(x) MAKELONG(SWAPWORD(HIWORD(x)), SWAPWORD(LOWORD(x)))

#define TT_TAG_NAME "name"  // 命名表 版权说明、字体名、字体族名、风格名等说明
#define TT_TAG_OS2  "OS/2"  // OS/2和Windows特有风格 TrueType字体所需的规格集

static const DK_UINT16 gs_OSCodePageRange[] = 
{
    1252, // Latin 1
    1250, // Latin 2: Eastern Europe
    1251, // Cyrillic
    1253, // Greek
    1254, // Turkish
    1255, // Hebrew
    1256, // Arabic
    1257, // Windows Baltic
    1258, // Vietnamese
    0   , // Reserved for Alternate ANSI
    0   , // Reserved for Alternate ANSI
    0   , // Reserved for Alternate ANSI
    0   , // Reserved for Alternate ANSI
    0   , // Reserved for Alternate ANSI
    0   , // Reserved for Alternate ANSI
    0   , // Reserved for Alternate ANSI
    874 , // Thai
    932 , // JIS/Japan
    936 , // Chinese: Simplified chars--PRC and Singapore
    949 , // Korean Wansung
    950 , // Chinese: Traditional chars--Taiwan and Hong Kong
    1361, // Korean Johab
    0   , // Reserved for Alternate ANSI & OEM
    0   , // Reserved for Alternate ANSI & OEM
    0   , // Reserved for Alternate ANSI & OEM
    0   , // Reserved for Alternate ANSI & OEM
    0   , // Reserved for Alternate ANSI & OEM
    0   , // Reserved for Alternate ANSI & OEM
    0   , // Reserved for Alternate ANSI & OEM
    0   , // Macintosh Character Set (US Roman)
    0   , // OEM Character Set
    0   , // Symbol Character Set
    0   , // Reserved for OEM
    0   , // Reserved for OEM
    0   , // Reserved for OEM
    0   , // Reserved for OEM
    0   , // Reserved for OEM
    0   , // Reserved for OEM
    0   , // Reserved for OEM
    0   , // Reserved for OEM
    0   , // Reserved for OEM
    0   , // Reserved for OEM
    0   , // Reserved for OEM
    0   , // Reserved for OEM
    0   , // Reserved for OEM
    0   , // Reserved for OEM
    0   , // Reserved for OEM
    0   , // Reserved for OEM
    869 , // IBM Greek
    866 , // MS-DOS Russian
    865 , // MS-DOS Nordic
    864 , // Arabic
    863 , // MS-DOS Canadian French
    862 , // Hebrew
    861 , // MS-DOS Icelandic
    860 , // MS-DOS Portuguese
    857 , // IBM Turkish
    855 , // IBM Cyrillic; primarily Russian
    852 , // Latin 2
    775 , // MS-DOS Baltic
    737 , // Greek; former 437 G
    708 , // Arabic; ASMO 708
    850 , // WE/Latin 1
    437   // US
};

DKTTFInfo::DKTTFInfo()
    : m_pFile(NULL)
    , m_uSelection(0)
    , m_ulCodePageRange1(0)
    , m_ulCodePageRange2(0)
{
    m_lstFullFontName.clear();
    m_lstFontFamilyName.clear();
    m_lstSubFamilyName.clear();
}

DKTTFInfo::~DKTTFInfo()
{
    m_lstFullFontName.clear();
    m_lstFontFamilyName.clear();
    m_lstSubFamilyName.clear();
}
    
void DKTTFInfo::InitMember()
{
    CloseFontFile();
}

bool DKTTFInfo::OpenFontFile(string &wsTtfPath, TT_FONTFILE_TYPE iFileType)
{
    assert(NULL == m_pFile);
    
    InitMember();
    
    m_pFile = DkStreamFactory::GetFileStream(wsTtfPath.c_str());
    if (NULL == m_pFile)
    {
        DebugPrintf(DLC_JUGH, "DKTTFInfo::OpenTtfFile Open(FileStream) ERROR %s!!!", wsTtfPath.c_str());
        return false;
    }
    int iResult = m_pFile->Open();
    if (DK_FAILED(iResult))
    {
        DebugPrintf(DLC_JUGH, "DKTTFInfo::OpenTtfFile Open(File) ERROR %d!!!", iResult);
        return false;
    }

    switch(iFileType)
    {
    case TT_FONTFILE_TTC:
        return ParseTtcFile();
    case TT_FONTFILE_TTF: 
        return ParseTtfFile();
    default:
        break;
    }
    return false;
}

void DKTTFInfo::CloseFontFile()
{
    if (NULL != m_pFile)
    {
        m_pFile->Close();
        delete m_pFile;
        m_pFile = NULL;
    }
    
    m_lstFullFontName.clear();
    m_lstFontFamilyName.clear();
    m_lstSubFamilyName.clear();
    m_ulCodePageRange1 = 0;
    m_ulCodePageRange2 = 0;    
    m_uSelection       = 0;
}

bool DKTTFInfo::GetSupportedCodePage(std::vector<DK_UINT16>& lstCodepage)
{ 
    assert(NULL != m_pFile);
    
    // TODO:根据CMAP表里面的ID号可以对应平台 
    lstCodepage.clear();
    int iSize = DK_DIM(gs_OSCodePageRange);
    for (int i = 0; i < iSize; ++i)
    {
        unsigned long  ulCodepageRange = (i < 32) ? m_ulCodePageRange1 : m_ulCodePageRange2;
        unsigned long  ulBits          = (i < 32) ? (0x1 << i) : (0x1 << (i - 32));
        
        if ((ulCodepageRange & ulBits) && gs_OSCodePageRange[i] > 0)
        {
            lstCodepage.push_back(gs_OSCodePageRange[i]);
        }
    }
    return lstCodepage.size() > 0;
}

vector<wstring>* DKTTFInfo::GetFullFontName()
{
#ifdef DEBUG_FONT_INFO
    DebugPrintf(DLC_JUGH, "DKTTFInfo::GetFullFontName start %d!!!", m_lstFullFontName.size());
#endif
    sort(m_lstFullFontName.begin(), m_lstFullFontName.end());
    vector<wstring>::iterator vi = unique(m_lstFullFontName.begin(), m_lstFullFontName.end());
    m_lstFullFontName.erase(vi, m_lstFullFontName.end()); 
#ifdef DEBUG_FONT_INFO
    DebugPrintf(DLC_JUGH, "DKTTFInfo::GetFullFontName end %d!!!", m_lstFullFontName.size());
#endif
    return &m_lstFullFontName;
}

vector<wstring>* DKTTFInfo::GetFontFamilyName()
{
    sort(m_lstFontFamilyName.begin(), m_lstFontFamilyName.end());
    vector<wstring>::iterator vi = unique(m_lstFontFamilyName.begin(), m_lstFontFamilyName.end());
    m_lstFontFamilyName.erase(vi, m_lstFontFamilyName.end()); 
    return &m_lstFontFamilyName;
}

vector<wstring>* DKTTFInfo::GetFontSubFamilyName()
{
    sort(m_lstSubFamilyName.begin(), m_lstSubFamilyName.end());
    vector<wstring>::iterator vi = unique(m_lstSubFamilyName.begin(), m_lstSubFamilyName.end());
    m_lstSubFamilyName.erase(vi, m_lstSubFamilyName.end()); 
    return &m_lstSubFamilyName;
}

DK_FONT_STYLE DKTTFInfo::GetFontStyle()
{
    assert(NULL != m_pFile);

    if (m_uSelection & 0x40)  // REGULAR
    {
        return DK_FontStyleRegular;
    }
    else if (0x21 == (m_uSelection & 0x21))  // ITALIC & BOLD
    {
        return DK_FontStyleBoldItalic;
    }
    else if (m_uSelection & 0x1) // ITALIC
    {
        return DK_FontStyleItalic;
    }
    else if (m_uSelection & 0x20) // BOLD
    {
        return DK_FontStyleBold;
    }
        
    return DK_FontStyleRegular;
}

bool DKTTFInfo::ParseTtcFile()
{   
    assert(NULL != m_pFile);
  
    TTC_TABLE_ENTRY tTableEntry;    
    int iTableEntryLen = sizeof(TTC_TABLE_ENTRY);
    memset(&tTableEntry, 0, iTableEntryLen);
    
    m_pFile->Seek(0, IDkStream::DK_SEEK_SET);
    if (DK_FAILED(m_pFile->Read(&tTableEntry, iTableEntryLen)))
    {
        DebugPrintf(DLC_JUGH, "DKTTFInfo::ParseTtcFile Read(tTableEntry) ERROR!!!");
        return false;
    }
    
    char csTemp[5]  = {0};  
    memcpy(csTemp, tTableEntry.csTag, 4); 
    if (0 != strcmp(csTemp, "ttcf"))
    {
        DebugPrintf(DLC_JUGH, "DKTTFInfo::ParseTtcFile csTag ERROR :%s!!!", csTemp);
        return false;
    }
    unsigned long &ulNumFonts = tTableEntry.ulNumFonts;
    ulNumFonts = SWAPLONG(ulNumFonts);

    if (ulNumFonts <= 0)
    {
        DebugPrintf(DLC_JUGH, "DKTTFInfo::ParseTtcFile ulNumFont ERROR :%d!!!", ulNumFonts);
        return false;
    }

    int    iOffsetSize  = ulNumFonts*sizeof(unsigned long);
    unsigned long *pOffsetTable = (unsigned long*)DK_MALLOCZ(iOffsetSize);
    if (NULL == pOffsetTable || DK_FAILED(m_pFile->Read(pOffsetTable, iOffsetSize)))
    {
        DebugPrintf(DLC_JUGH, "DKTTFInfo::ParseTtcFile Read(pOffsetTable) ERROR :%d!!!", iOffsetSize);
        SafeFreePointer(pOffsetTable);
        return false;
    }
    
    for (unsigned long i = 0; i < ulNumFonts; ++i)
    {
        unsigned long &lOffset = pOffsetTable[i];
        lOffset = SWAPLONG(lOffset);
        DebugPrintf(DLC_JUGH, "pOffsetTable = %x!!!", lOffset);
        
        if (!ParseTtfFile(lOffset))
        {
            SafeFreePointer(pOffsetTable);
            return false;
        }
    }
    SafeFreePointer(pOffsetTable);
    
    DebugPrintf(DLC_JUGH, "DKTTFInfo::ParseTtcFile  SUCCESSED!!!");
    return DK_TRUE; 
    
    /*// TODO: 只处理TTC包含的第一个TTF
    unsigned long ulOffset = -1;    
    if (DK_FAILED(m_pFile->Read(&ulOffset, sizeof(unsigned long))) || ulOffset <= 0)
    {
        DebugPrintf(DLC_JUGH, "DKTTFInfo::ParseTtcFile Read(ulOffset) ERROR :%d!!!", ulOffset);
        return false;
    }
    ulOffset = SWAPLONG(ulOffset);
    return ParseTtfFile(ulOffset);*/
}

bool DKTTFInfo::ParseTtfFile(int iFileOffset)
{    
    assert(iFileOffset >= 0 && NULL != m_pFile);
    
    // 找到table的索引位置
    TTF_TABLE_DIRECTORY tTableDirectory;
    int iTableDirectoryLen = sizeof(tTableDirectory);
    m_pFile->Seek(iFileOffset, IDkStream::DK_SEEK_SET);
    
    if (DK_FAILED(m_pFile->Read(&tTableDirectory, iTableDirectoryLen)))
    {
        DebugPrintf(DLC_JUGH, "DKTTFInfo::ParseTtfFile Read(FileStream) ERROR!!!");
        return false;
    }

    tTableDirectory.uNumOfTables  = SWAPWORD(tTableDirectory.uNumOfTables);
    tTableDirectory.uMajorVersion = SWAPWORD(tTableDirectory.uMajorVersion);
    tTableDirectory.uMinorVersion = SWAPWORD(tTableDirectory.uMinorVersion);   

    if (tTableDirectory.uMajorVersion != 1 || tTableDirectory.uMinorVersion != 0)
    {
        DebugPrintf(DLC_JUGH, "DKTTFInfo::ParseTtfFile Version ERROR!!!");
        return false;
    }

    // 找到table name和OS/2对应的索引位置
    // ttf中对table名已经排完序了,tTablEntry的顺序应该按序存放
    TTF_TABLE_ENTRY  tTablEntry;
    int          iEntrySize  = sizeof(TTF_TABLE_ENTRY);  
    
    bool bFoundOS   = false, bFoundName = false;
    char csTemp[5]  = {0};      
    for (int i = 0; i < tTableDirectory.uNumOfTables; ++i)
    {
        memset(&tTablEntry, 0, iEntrySize);
        memset(csTemp, 0, 5);
        if (DK_FAILED(m_pFile->Read(&tTablEntry, iEntrySize)))
        {
            break;        
        }
        tTablEntry.ulCheckSum = SWAPLONG(tTablEntry.ulCheckSum);
        tTablEntry.ulOffset = SWAPLONG(tTablEntry.ulOffset);
        tTablEntry.ulLength = SWAPLONG(tTablEntry.ulLength);
        memcpy(csTemp, tTablEntry.csTag, 4);
        
        //记录当前位置，解析完后重新回到当前位置读取 
        int iPosition = m_pFile->Tell();
        
        if (0 == strcmp(csTemp, TT_TAG_OS2))
        {
            bFoundOS = ParseOSTable(tTablEntry.ulOffset);
        }
        else if (0 == strcmp(csTemp, TT_TAG_NAME))
        {
            bFoundName = ParseNameTable(tTablEntry.ulOffset);
            break;
        }
        m_pFile->Seek(iPosition, IDkStream::DK_SEEK_SET);
    }
    
    return bFoundOS && bFoundName;
}

bool DKTTFInfo::ParseNameTable(int iNameTableOffset)
{
    assert(iNameTableOffset >= 0 && NULL != m_pFile);
    TTF_NAME_TABLE_HEADER tNTHeader;
    m_pFile->Seek(iNameTableOffset, IDkStream::DK_SEEK_SET);
    
    if (DK_FAILED(m_pFile->Read(&tNTHeader, sizeof(TTF_NAME_TABLE_HEADER))))
    {
        DebugPrintf(DLC_JUGH, "DKTTFInfo::ParseNameTable ReadFile ERROR!!!");
        return false;
    }
    
    unsigned short &uFSelector = tNTHeader.uFSelector;
    uFSelector = SWAPWORD(uFSelector);
    if (0 != uFSelector && 1 != uFSelector)
    {
        DebugPrintf(DLC_JUGH, "DKTTFInfo::ParseNameTable uFSelector ERROR!!!");
        return false;
    }
    tNTHeader.uNameRecordCount = SWAPWORD(tNTHeader.uNameRecordCount);
    tNTHeader.uStorageOffset   = SWAPWORD(tNTHeader.uStorageOffset);
            
    bool        bParseResult = DK_TRUE;
    TT_NAME_RECORD tNameRecord;
    int iNameRecordSize = sizeof(TT_NAME_RECORD);
    for (int i = 0; i < tNTHeader.uNameRecordCount; ++i)
    {
        if (DK_FAILED(m_pFile->Read(&tNameRecord, iNameRecordSize)))
        {
            bParseResult = false;
            break;
        }
        unsigned short &uPlatfromID   = tNameRecord.uPlatfromID;
        unsigned short &uEncodingID   = tNameRecord.uEncodingID;
        unsigned short &uLanguageID   = tNameRecord.uLanguageID;
        unsigned short &uNameID       = tNameRecord.uNameID;
        unsigned short &uStringLength = tNameRecord.uStringLength;
        unsigned short &uStringOffset = tNameRecord.uStringOffset;
        uPlatfromID   = SWAPWORD(uPlatfromID);
        uEncodingID   = SWAPWORD(uEncodingID);
        uLanguageID   = SWAPWORD(uLanguageID);
        uNameID       = SWAPWORD(uNameID);
        uStringLength = SWAPWORD(uStringLength);
        uStringOffset = SWAPWORD(uStringOffset);
        
        //记录当前位置，解析完后重新回到当前位置读取 
        int iPosition = m_pFile->Tell();

         // TODO: 目前只需要获取fontname
        if (1 == uNameID || 2 == uNameID || 4 == uNameID)
        { 
#ifdef DEBUG_FONT_INFO
            std::cout << "\nuPlatfromID = 0x" << uPlatfromID << std::endl;
            std::cout << "uEncodingID = 0x" << uEncodingID << std::endl;
            std::cout << "uLanguageID = 0x" << uLanguageID << std::endl;
            std::cout << "uNameID     = 0x" << uNameID << std::endl;
            std::cout << "uStringOffset = 0x" << uStringOffset << std::endl;
            std::cout << "uStringLength = 0x" << uStringLength << std::endl;
#endif
            // Font Family name. Up to four fonts can share the Font Family name, forming a font style linking group 
            int iOffset = iNameTableOffset + tNTHeader.uStorageOffset + uStringOffset;
#ifdef DEBUG_FONT_INFO
            std::cout << "iOffset = 0x" << iOffset << std::endl;
#endif
            int iEncodingType = GetEncodingType(uPlatfromID, uEncodingID, uLanguageID);
            wstring strName;
            if (GetName(strName, iEncodingType, iOffset, uStringLength))
            {
                switch(uNameID)
                {
                case 1:
                    m_lstFontFamilyName.push_back(strName);
                    break;
                case 2:
                    m_lstSubFamilyName.push_back(strName);
                    break;
                case 4:
                    m_lstFullFontName.push_back(strName);
                    break;
                default:
#ifdef DEBUG_FONT_INFO
                    std::cout << "uNameID = " << uNameID << std::endl;
#endif
                    break;
                }
            }
        }
        m_pFile->Seek(iPosition, IDkStream::DK_SEEK_SET);

    }
    return bParseResult;
}

int DKTTFInfo::GetEncodingType(int iPlatformID, int iEncodingID, int iLanguageID)
{
    // TODO: 没有包含所有情况，考虑直接读表进行判断
    int iEncodingType = eTT_ENCODING_CHAR;
    if (iPlatformID == 0)
    {
        iEncodingType = eTT_ENCODING_CHAR;
    }
    else if (iPlatformID == 1)
    {
        if (iEncodingID == 2)
        {
            iEncodingType = eTT_ENCODING_BIG5;
        }
        else if (iEncodingID == 25)
        {
            iEncodingType = eTT_ENCODING_GBK;
        }
    }
    else if (iPlatformID == 3)
    {
        if (iEncodingID == 1)
        {
            iEncodingType = eTT_ENCODING_UCS2;                    
        }
        else if (iEncodingID == 3)
        {
            iEncodingType = eTT_ENCODING_GBK;
        }
        else if (iEncodingID == 4)
        {
            iEncodingType = eTT_ENCODING_BIG5;
        }
        else if (iEncodingID == 10)
        {
            iEncodingType = eTT_ENCODING_UCS4;
        }
    }
#ifdef DEBUG_FONT_INFO
    std::cout << "iEncodingType = " << iEncodingType << std::endl;
#endif
    return iEncodingType;
}

bool DKTTFInfo::GetName(wstring& strName, int iEncodingType, int iOffset, int iLength)
{    
    DebugPrintf(DLC_JUGH, "DKTTFInfo::GetString  start!!!");
    assert(NULL != m_pFile && iEncodingType >= eTT_ENCODING_CHAR && iEncodingType <= eTT_ENCODING_UCS4);
    if (iOffset < 0 || iLength <= 0)
    {
        DEBUG_POS;
        return false;
    }
    
    strName.empty();    
    m_pFile->Seek(iOffset, IDkStream::DK_SEEK_SET);

    DK_FILESIZE_T   iSize = iLength + 2;
    DK_CHAR* pName = (DK_CHAR*)DK_MALLOCZ(iSize);
    if (NULL == pName)
        return false; 
    
    DK_FILESIZE_T  iReadLen = -1;
    bool bParse = DK_SUCCEEDED(m_pFile->Read(pName, iLength, &iReadLen)) && (iReadLen > 0);
    if (bParse)
    {
        DebugPrintf(DLC_JUGH, "DKTTFInfo::GetString CharToWChar %d!!!", iEncodingType);
        EncodingUtil::Encoding eEncoding = EncodingUtil::SYSDEFAULT;
        
        TT_ENCODING ttEncoding = (TT_ENCODING)iEncodingType;
        switch(ttEncoding)
        {
        case eTT_ENCODING_GBK:
            eEncoding= EncodingUtil::GBK;
            break;
        case eTT_ENCODING_BIG5:
            eEncoding = EncodingUtil::BIG5;
            break;
        case eTT_ENCODING_UCS2:
            eEncoding = EncodingUtil::UNICODE_BE;
            break;
        case eTT_ENCODING_UCS4:
            eEncoding = EncodingUtil::WCHAR;
            break;
        default:
            break;
        }
        
        if (bParse)
        {
            DK_WCHAR* strTemp = EncodingUtil::CharToWChar(pName, eEncoding);
            bParse = (NULL != strTemp);
            if (bParse)
            {
                strName = strTemp;
                DK_FREE(strTemp);
            }
        }
    }
           
    SafeFreePointer(pName);
    return bParse;
}

bool DKTTFInfo::ParseOSTable(int iOSTableOffset)
{
    assert(iOSTableOffset >= 0 && NULL != m_pFile);

    m_pFile->Seek(iOSTableOffset, IDkStream::DK_SEEK_SET);
    unsigned short version = 0;
    unsigned short uSelection = 0;
    unsigned long  ulCodePageRange1 = 0;
    unsigned long  ulCodePageRange2 = 0;

    if (DK_FAILED(m_pFile->Read(&version, sizeof(unsigned short))))
        return false;

    version = SWAPWORD(version);
    m_pFile->Seek(iOSTableOffset, IDkStream::DK_SEEK_SET);
    switch(version)
    {
    case 0:
        {
            /* 没有对应的codepage的字段
            TT_OS2_TABLE_0 tOSTable;
            if (DK_FAILED(m_pFile->Read(&tOSTable, sizeof(TT_OS2_TABLE_0))))
                return false;*/
            return false;
        }
        break;
    case 1:
        {
            TT_OS2_TABLE_1 tOSTable;
            if (DK_FAILED(m_pFile->Read(&tOSTable, sizeof(TT_OS2_TABLE_1))))
                return false;

            uSelection       = tOSTable.uSelection;
            ulCodePageRange1 = tOSTable.ulCodePageRange1;
            ulCodePageRange2 = tOSTable.ulCodePageRange2;
        }
        break;
    case 2:
    case 3:
    case 4:
        {
            TT_OS2_TABLE_234 tOSTable;
            if (DK_FAILED(m_pFile->Read(&tOSTable, sizeof(TT_OS2_TABLE_234))))
                return false;

            uSelection       = tOSTable.uSelection;
            ulCodePageRange1 = tOSTable.ulCodePageRange1;
            ulCodePageRange2 = tOSTable.ulCodePageRange2;
        }
        break;
    default:
        return false;
    }

    m_uSelection       = SWAPWORD(uSelection);
    m_ulCodePageRange1 = SWAPLONG(ulCodePageRange1);
    m_ulCodePageRange2 = SWAPLONG(ulCodePageRange2);

    return DK_TRUE;
}


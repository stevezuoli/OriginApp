//===========================================================================
// Summary:
//	    DKEEPUBDef.h
// Usage:
//      定义EPUB在解析过程中使用到的一些枚举或结构体
//  		...
// Remarks:
//	    Null
// Date:
//	    2011-09-21
// Author:
//	    Zhang Jiafang(zhangjf@duokan.com)
//===========================================================================

#ifndef __KERNEL_EPUBKIT_EPUBLIB_EXPORT_DKEEPUBDEF_H__
#define __KERNEL_EPUBKIT_EPUBLIB_EXPORT_DKEEPUBDEF_H__

#include "KernelRenderType.h"
#include "KernelType.h"
#include "RichTextString.h"

class IDKEFlexPage;
class IDKESection;

// EPUB的容器类型
enum DKE_CONTAINER_TYPE
{
    DKE_CONTAINER_FOLDER,           // 文件系统目录容器
    DKE_CONTAINER_ZIP,              // ZIP归档容器
    DKE_CONTAINER_UNKNOWN           // 未知的容器
};

// HTML的风格类型
enum DKE_HTMLSTYLE_TYPE
{
    DKE_HTMLSTYLE_STANDARD,         // 标准的HTML风格
    DKE_HTMLSTYLE_MOBI              // MOBI似的使用了许多废弃标签的HTML风格
};

// ePub metadata中的角色（opf:role）
enum DKE_METAROLE_TYPE
{
    DKE_METAROLE_UNKNOWN = -1,
    DKE_METAROLE_EDT,
    DKE_METAROLE_TRL,
    DKE_METAROLE_COUNT
};

// EPUB文件的基本信息
struct DKEBOOKINFO
{
    DK_WCHAR* pTitle;
    DK_WCHAR* pIdentifierID;
	DK_WCHAR* pIdentifierSchema;
    DK_WCHAR* pIdentifier;
    DK_WCHAR* pLanguage;
    DK_WCHAR* pCreator;
	DK_WCHAR* pAuthor;
    DK_WCHAR* pSubject;
	DK_WCHAR* pPublisher;
	DK_WCHAR* pDescription;
	DK_WCHAR* pCoverage;
	DK_WCHAR* pSource;
	DK_WCHAR* pDate;
	DK_WCHAR* pRights;
	DK_WCHAR* pContributor;
	DK_WCHAR* pTypeStr;
	DK_WCHAR* pFormat;
	DK_WCHAR* pRelation;
	DK_WCHAR* pBuilder;
	DK_WCHAR* pBuilderVersion;

    // 通过枚举DKE_METAROLE_TYPE来获取每个role的值
    DK_WCHAR* pRoles[DKE_METAROLE_COUNT];

    DKEBOOKINFO()
        : pTitle(DK_NULL)
        , pIdentifierID(DK_NULL)
        , pIdentifierSchema(DK_NULL)
        , pIdentifier(DK_NULL)
        , pLanguage(DK_NULL)
        , pCreator(DK_NULL)
        , pAuthor(DK_NULL)
        , pSubject(DK_NULL)
        , pPublisher(DK_NULL)
        , pDescription(DK_NULL)
        , pCoverage(DK_NULL)
        , pSource(DK_NULL)
        , pDate(DK_NULL)
        , pRights(DK_NULL)
        , pContributor(DK_NULL)
        , pTypeStr(DK_NULL)
        , pFormat(DK_NULL)
        , pRelation(DK_NULL)
        , pBuilder(DK_NULL)
        , pBuilderVersion(DK_NULL)
    {
        DkZero(pRoles);
    }
};

struct DKE_DRMKEY
{
    DK_BYTE* key;
    DK_UINT length;

    DKE_DRMKEY() 
        : key(DK_NULL)
        , length(0)
    {
        // do nothing
    }
};

enum DKE_PAGEOBJ_TYPE
{
    DKE_PAGEOBJ_UNKNOWN = 0,
    DKE_PAGEOBJ_TEXT,                   // 文字（原子对象）
    DKE_PAGEOBJ_IMAGE,                  // 普通图（原子对象）
    DKE_PAGEOBJ_STATICIMAGE,            // 静态图（原子对象）
    DKE_PAGEOBJ_FOOTNOTE,               // 脚注（原子对象）
    DKE_PAGEOBJ_INTERACTIVEIMAGE,       // 可交互图（原子对象）
    DKE_PAGEOBJ_CROSSPAGE,              // 对页图（原子对象）
    DKE_PAGEOBJ_GALLERY,                // 画廊（原子聚合对象）
    DKE_PAGEOBJ_GRAPH,                  // 图形（原子对象）
    DKE_PAGEOBJ_PATH,                   // 路径（原子对象）
    DKE_PAGEOBJ_INVISIBLE,              // 隐藏对象（原子对象）
    DKE_PAGEOBJ_BLOCK,                  // 块对象（原子聚合对象）
    DKE_PAGEOBJ_TABLE,                  // 表格对象（原子聚合对象）
    DKE_PAGEOBJ_VIDEO,                  // 视频对象（原子对象）
    DKE_PAGEOBJ_AUDIO,                  // 音频对象（原子对象）
    DKE_PAGEOBJ_BGIMAGE,                // 背景图像（原子对象）
    DKE_PAGEOBJ_PREBLOCK,               // PRE块对象（原子聚合对象）
    DKE_PAGEOBJ_IMAGEBLOCK              // 多看扩展图像块（原子聚合对象）
};


enum DKE_IMAGEBLOCK_TYPE
{
    DKE_IMAGEBLOCK_UNKNOWN = 0,
    DKE_IMAGEBLOCK_STATIC,              // 静态图对应的图像块
    DKE_IMAGEBLOCK_SINGLE,              // 交互图对应的图像块 
    DKE_IMAGEBLOCK_CROSSPAGE,           // 折页图对应的图像块
    DKE_IMAGEBLOCK_MULTICALLOUT         // 多点交互图对应的图像块
};

struct DKE_HITTEST_STREAMINGMEDIA_INFO
{
    DKSTREAMINFO* pSrcData;             // 音视频的原始数据。如果是网络音视频，pSrcData为空
    DK_WCHAR* pSrcURL;                  // 音视频URL
    DK_MIME_TYPE type;                  // MIME 音视频类型
};

enum DKE_TRACK_KIND
{
    DKE_TRACK_KIND_UNKNOWN = 0,
    DKE_TRACK_KIND_SUBTITLES,
    DKE_TRACK_KIND_CAPTIONS,
    DKE_TRACK_KIND_DESCRIPTIONS,
    DKE_TRACK_KIND_CHAPTERS,
    DKE_TRACK_KIND_METADATA,
};

// 字幕信息
struct DKE_TRACK_INFO
{
    DKE_TRACK_KIND kind;
    DKSTREAMINFO* pSrcData;
    DK_WCHAR* pSrc;
    DK_LANG_CODE srcLang;
    DK_WCHAR* pLabel;
    DK_BOOL trackDefault;

    DKE_TRACK_INFO()
        : kind(DKE_TRACK_KIND_UNKNOWN),
          srcLang(DK_LANG_UNKNOWN),
          trackDefault(DK_FALSE)
    {
        // do nothing
    }
};

enum DKE_MEDIA_PRELOAD_TYPE
{
    DKE_MEDIA_PRELOAD_AUTO,          // Indicates that the audio/video should start loading as soon as the page loads
    DKE_MEDIA_PRELOAD_METADATA,      // Indicates that only the metadata for the audio/video should be loaded when the page loads
    DKE_MEDIA_PRELOAD_NONE           // Indicates that the audio/video should NOT start loading as soon as the page loads
};

struct DKE_SMIL_PAR_INFO
{
    DK_FLOAT clipBegin;             // audio begin
    DK_FLOAT clipEnd;               // audio end
    DK_WCHAR* textURL;              // text URL
    DK_WCHAR* textAnchor;           // text anchor
    DK_WCHAR* audioURL;             // audio URL

    DKE_SMIL_PAR_INFO()
        : clipBegin(0.0), clipEnd(0.0), textURL(DK_NULL), textAnchor(DK_NULL), audioURL(DK_NULL)
    {}
};

struct DKE_HITTEST_MEDIA_INFO
{
    DKE_HITTEST_STREAMINGMEDIA_INFO* pSources;              // 音视频数据源数组，网页中的音视频标签可以指定多个数据源
    DK_UINT sourcesCount;                                   // pSources 数组元素个数

    DKE_TRACK_INFO* pTracks;                                // 字幕数据，网页中的音视频标签可以指定多个字幕
    DK_UINT tracksCount;                                    // pTracks 数组元素个数

    DK_BOOL autoplay;                                       // 是否自动播放
    DK_BOOL controls;                                       // 是否显示播放暂定等按钮
    DK_BOOL loop;                                           // 是否循环播放
    DK_BOOL muted;                                          // 是否静音
    DKE_MEDIA_PRELOAD_TYPE preloadType;
    DK_WCHAR* pMediaGroup;
    DK_WCHAR* pTitle;                                       // 多看扩展音频节点的标题

    DKFILEINFO* pPoster;                                    // 展位图的文件数据
    DKFILEINFO* pActiveState;                               // 音频在活动状态下的占位图数据

    DKE_HITTEST_MEDIA_INFO()
        : pSources(DK_NULL)
        , sourcesCount(0)
        , pTracks(DK_NULL)
        , tracksCount(0)
        , autoplay(DK_FALSE)
        , controls(DK_FALSE)
        , loop(DK_FALSE)
        , muted(DK_FALSE)
        , preloadType(DKE_MEDIA_PRELOAD_AUTO)
        , pMediaGroup(DK_NULL)
        , pTitle(DK_NULL)
        , pPoster(DK_NULL)
        , pActiveState(DK_NULL)
    {
        // do nothing
    }
};

struct DKE_HITTEST_OBJECTINFO
{
    DKE_PAGEOBJ_TYPE objType;           // 对象类型
    DK_BOX boundingBox;                 // 外接矩形
    DK_MATRIX transformMatrix;          // 变换矩阵
    DK_BOX clipBox;                     // 裁剪区，不存在时，值等于图像区域    
    
    union
    {
        DK_WCHAR *altText;              // 当 objType 为 DKE_PAGEOBJ_FOOTNOTE 时有效

        // 当 objType 为 DKE_PAGEOBJ_STATICIMAGE 或 DKE_PAGEOBJ_INTERACTIVEIMAGE 或 DKE_PAGEOBJ_CROSSPAGE 时有效
        struct
        {
            DKFILEINFO* srcImageData;           // 图片的原始数据
            DKFILEINFO* extImageData;           // @Deprecated
            DK_WCHAR* srcImagePath;				// 图片在EPUB包内的全路径
            DK_UINT srcImageWidth;              // 图片的宽度
            DK_UINT srcImageHeight;             // 图片的高度
            RichTextString* mainTitle;          // 图片的图注主标题，没有则为空
            RichTextString* subTitle;           // 图片的图注副标题，没有则为空
        };

        // 当 objType 为 DKE_PAGEOBJ_VIDEO 或 DKE_PAGEOBJ_AUDIO 时有效
        DKE_HITTEST_MEDIA_INFO* pMedia;
    };

    DKE_HITTEST_OBJECTINFO()
        : objType(DKE_PAGEOBJ_UNKNOWN)
        , srcImageData(DK_NULL)
        , extImageData(DK_NULL)
		, srcImagePath(DK_NULL)
        , srcImageWidth(0)
        , srcImageHeight(0)
        , mainTitle(DK_NULL)
        , subTitle(DK_NULL)
    {
        // do nothing
    }
};

struct DKE_CHAR_INFO
{
    DK_BOX boundingBox;                 // 外接矩形
    DK_WCHAR charCode;                  // 文字的 Unicode 编码
    DK_FLOWPOSITION pos;                // 文字的索引位置

    DKE_CHAR_INFO()
        : charCode(0)
    {
        // do nothing
    }
};

enum DKE_SHOWCONTENT_TYPE
{
    DKE_SHOWCONTENT_FLEXPAGE,           // 以可伸缩的页展示内容
    DKE_SHOWCONTENT_FIXEDPAGE           // 以固定页分页的形式展示内容
};

union DKE_SHOWCONTENT_HANDLE
{
    IDKEFlexPage* pFlexPage;            // 当请求的DKE_SHOWCONTENT_TYPE为DKE_SHOWCONTENT_FLEXPAGE时句柄为可伸缩页
    IDKESection* pSection;              // 当请求的DKE_SHOWCONTENT_TYPE为DKE_SHOWCONTENT_FIXEDPAGE时句柄为可分页的HTML片段
};

enum DKE_FOOTNOTECONTENT_TYPE
{
    DKE_FOOTNOTECONTENT_ALTTEXT,
    DKE_FOOTNOTECONTENT_RICHTEXT
};

struct DKE_FOOTNOTE_INFO
{
    DK_BOX boundingBox;                     // 外接矩形
    DKE_FOOTNOTECONTENT_TYPE noteType;      // 脚注内容的类型

    union
    {
        DK_WCHAR* noteText;                 // 注释文本，当noteType为DKE_FOOTNOTECONTENT_ALTTEXT时有效
        DK_WCHAR* noteId;                   // 富文本注释指向的ID，可以通过该ID获取展示富文本的句柄，当noteType为DKE_FOOTNOTECONTENT_RICHTEXT时有效
    };

    DKFILEINFO* srcImageData;           // 图片的原始数据
    DK_WCHAR* srcImagePath;				// 图片在EPUB包内的全路径

    DKE_FOOTNOTE_INFO()
        : noteType(DKE_FOOTNOTECONTENT_ALTTEXT)
        , noteText(DK_NULL)
        , srcImageData(DK_NULL)
        , srcImagePath(DK_NULL)
    {
        // do nothing
    }
};

struct DKE_PREBLOCK_INFO
{
    DK_BOX boundingBox;                     // pre块在页面内的外接矩形
    DK_FLOWPOSITION startPos;               // pre块在页面内的起始流式坐标
    DK_FLOWPOSITION endPos;                 // pre块在页面内的结束流式坐标
    DK_DOUBLE maxPreWidth;                  // pre块在设置的排版参数下的最大宽度，排版参数改变是此值无效
    IDKEFlexPage* pPreFlexPage;             // pre块对应的可伸缩页

    DKE_PREBLOCK_INFO()
        : maxPreWidth(0)
        , pPreFlexPage(DK_NULL)
    {
        // do nothing
    }
};

struct DKE_CALLOUT_INFO
{
    DK_DOUBLE imageScale;               // 交互后图片的缩放比
    DK_POS position;                    // callout标题的中心点在原图上的坐标
    DK_POS target;                      // 指示目标在原图上的坐标
    DK_INT width;                       // callout区域的宽度
    DK_INT maxHeight;                   // callout区域的最大宽度

    DKE_CALLOUT_INFO()
        : imageScale(1.0)
        , width(0)
        , maxHeight(0)
    {
        // do nothing
    }
};

class IDKEMultiCallout;

struct DKE_IMAGEBLOCK_INFO
{
    DKE_IMAGEBLOCK_TYPE imgBlockType;               // 图片块的类型

    union
    {
        IDKEMultiCallout* pMultiCallout;            // 多点交互图信息，imgBlockType==DKE_IMAGEBLOCK_MULTICALLOUT时有效
        // TODO: 添加其他图片块扩展
    };

    DKE_IMAGEBLOCK_INFO()
        : imgBlockType(DKE_IMAGEBLOCK_UNKNOWN)
        , pMultiCallout(DK_NULL)
    {
        // do nothing
    }
};

// the type of the chapter
enum DKE_CHAPTER_TYPE
{
    DKE_CHAPTER_UNKNOWN,
    DKE_CHAPTER_NORMAL,
    DKE_CHAPTER_FULLSCREEN,
    DKE_CHAPTER_FITWINDOW
};

enum DKE_LINK_TYPE
{
    DKE_LINK_UNKNOWN,
    DKE_LINK_URL,
    DKE_LINK_FILEPOS
};

struct DKE_LINK_INFO
{
    DKE_LINK_TYPE linkType;         // 链接的类型
    union
    {
        struct
        {
            DK_WCHAR* linkTarget;           // 当linkType为DKE_LINK_URL时, linkTarget为链接URL
            DK_WCHAR* linkAnchor;           // 当linkType为DKE_LINK_URL时, linkAnchor为锚点名称
        };
                                        
        DK_UINT filePos;                    // 当linkType为DKE_LINK_FILEPOS时，filePos为文件内部偏移量
    };

    // 以下开始与结束为左闭右开区间
    DK_FLOWPOSITION startPos;       // 链接对应的页面元素的起始坐标
    DK_FLOWPOSITION endPos;         // 链接对应的页面元素的结束坐标

    DKE_LINK_INFO()
        : linkType(DKE_LINK_UNKNOWN)
        , linkTarget(DK_NULL)
        , linkAnchor(DK_NULL)
    {
        // do nothing
    }
};

// 解析器的分页模式
enum DKE_PARSER_PAGETABLEMODE
{
    DKE_PARSER_PAGETABLE_BOOK,      // 需要对整本书进行分页
    DKE_PARSER_PAGETABLE_CHAPTER,   // 需要对某章节进行分页
    DKE_PARSER_PAGETABLE_BUILTIN    // 分页表数据内置
};

// 分页表数据生成环境
struct DKE_PAGEBREAK_ENV
{
    DK_UINT         pageWidth;      // 页面区域宽度
    DK_UINT         pageHeight;     // 页面区域高度
    DK_DOUBLE       bodyFontSize;   // 正文字号
};

// 内容解析器选项参数
struct DKE_PARSER_OPTION
{
    DK_BOX                      pageBox;            // 页面区域。
    DK_BOX                      paddingBox;         // 页面内padding区域。
    DK_MEASURE_TYPE             msType;             // 度量单位，参见 DK_MEASURE_TYPE 枚举值说明。
    DK_LONG                     dpi;                // 输出设备的DPI值，当 msType = DK_MEASURE_PIXEL 时无效。

    DKE_PARSER_PAGETABLEMODE    ptMode;             // 分页模式，参见 DKE_PARSER_PAGETABLEMODE 枚举值说明。
    DK_LONG                     chapter;            // 起始章节索引，从 0 开始，当 ptMode = DKE_PARSER_PAGETABLE_CHAPTER 时有效。
    const DK_WCHAR*             ptFilePath;         // 内置分页表数据文件路径，当 ptMode = DKE_PARSER_PAGETABLE_BUILTIN 时有效。

    DKE_PARSER_OPTION() : msType(DK_MEASURE_PIXEL),
                          dpi(RenderConst::SCREEN_DPI),
                          ptMode(DKE_PARSER_PAGETABLE_BOOK),
                          chapter(0),
                          ptFilePath(DK_NULL)
    {}
};

enum DKE_BOOKCONTENT_TYPE
{
    DKE_BOOKCONTENT_UNKNOWN,
    DKE_BOOKCONTENT_RELEASE,
    DKE_BOOKCONTENT_TRIAL
};

enum DKE_SELECTION_MODE
{
    DKE_SELECTION_UNKNOWN,
    DKE_SELECTION_MIDDLECROSS,                      // 当坐标点越过中线时即选中当前文字 
    DKE_SELECTION_INTERCROSS,                       // 当坐标与当前字有交叉时即选中当前文字
    DKE_SELECTION_INTERCROSS_WORD                   // 当坐标点与当前字有交叉时，选中包含当前字在内的词
};

enum DKE_HITTEST_TEXT_MODE
{
    DKE_HITTEST_TEXT_UNKNOWN,
    DKE_HITTEST_TEXT_SENTENCE,                      // 以整句文本方式 HitTest 页面文字
    DKE_HITTEST_TEXT_WORD                           // 以文本分词方式 HitTest 页面文字
};

struct DKE_CSS_ERROR
{
    DK_WCHAR* pFilePath;                            // 错误所在的CSS文件
    DK_UINT lineNum;                                // 错误在文件内的行号
    DK_UINT columnNum;                              // 错误在行内的列号
    DK_WCHAR* pErrorMsg;                            // 错误信息

    DKE_CSS_ERROR()
        : pFilePath(DK_NULL)
        , lineNum(0)
        , columnNum(0)
        , pErrorMsg(DK_NULL)
    {
        // do nothing
    }
};

enum DKE_FOLLOWELEM_TYPE
{
    DKE_FOLLOWELEM_UNKNOWN = 0,
    DKE_FOLLOWELEM_IMAGE,               // 随文图
    DKE_FOLLOWELEM_DUOKAN_FOOTNOTE,     // 多看扩展的文内注
    DKE_FOLLOWELEM_AUDIO                // 随文音频
};

struct DKE_FOLLOWELEM_INFO
{
    DKE_FOLLOWELEM_TYPE type;           // 随文元素类型
    DK_UINT offsetInTextContent;        // 随文元素在所属文本中的位置

    DK_WCHAR* pAltText;                 // 随文图的alt文本

    DKE_FOLLOWELEM_INFO()
        : type(DKE_FOLLOWELEM_UNKNOWN),
          offsetInTextContent(0),
          pAltText(DK_NULL)
    {
        // do nothing
    }
};

struct DKE_TEXT_CONTENT_INFO
{
    DK_WCHAR* pTextContent;                     // 从文档中获取的文本
    DKE_FOLLOWELEM_INFO* pFollowElemInfo;       // 原文档文本 pTextContent 之间包含的随文元素（如随文图）数组
    DK_UINT followElemInfoCount;                // 随文元素数组中元素个数

    DKE_TEXT_CONTENT_INFO()
        : pTextContent(DK_NULL),
          pFollowElemInfo(DK_NULL),
          followElemInfoCount(0)
    {
        // do nothing
    }
};

enum DKE_WRITING_MODE_TYPE
{
    DKE_WRITING_MODE_HTB,   // horizontal-tb
    DKE_WRITING_MODE_VRL,   // vertical-rl
    DKE_WRITING_MODE_VLR    // vertical-lr
};

enum DKE_WRITING_DIRECTION_TYPE
{
    DKE_WRITING_DIRECTION_LTR,  // ltr
    DKE_WRITING_DIRECTION_RTL   // rtl
};

struct DKEWRITINGOPT
{
    DKE_WRITING_MODE_TYPE writingMode;
    DKE_WRITING_DIRECTION_TYPE writingDirection;

    DKEWRITINGOPT()
        : writingMode(DKE_WRITING_MODE_HTB), writingDirection(DKE_WRITING_DIRECTION_LTR)
    {}
};

enum DKE_BOOKLAYOUT_TYPE
{
    DKE_BOOKLAYOUT_REFLOW,
    DKE_BOOKLAYOUT_FIXED_PAGE,
    DKE_BOOKLAYOUT_COMICS_FRAME
};

enum DKE_ORIENTATIONLOCK_TYPE
{
    DKE_ORIENTATIONLOCK_NONE,
    DKE_ORIENTATIONLOCK_LANDSCAPE_ONLY,
    DKE_ORIENTATIONLOCK_PORTRAIT_ONLY
};

struct DKEDISPLAYOPT
{
    DKE_BOOKLAYOUT_TYPE layout;                 // 页面排版方式
    DKE_ORIENTATIONLOCK_TYPE orinentationLock;  // 屏幕方向锁定类型
    DK_BOOL bOpenToSpread;                      // 是否是对页模式
    DK_BOOL bFirstOnRight;                      // 在bOpenToSpread为true即对页模式下有效

    DKEDISPLAYOPT()
        : layout(DKE_BOOKLAYOUT_REFLOW)
        , orinentationLock(DKE_ORIENTATIONLOCK_NONE)
        , bOpenToSpread(DK_FALSE)
        , bFirstOnRight(DK_TRUE)
    {
        // do nothing
    }
};

struct DKEOPFManifestItem
{
    DK_WCHAR* pItemId;                  // manifest项的ID标识，对于章节即为chapter ID
    DK_WCHAR* pFullPath;                // 在ePub container中的全路径

    DKEOPFManifestItem()
        : pItemId(DK_NULL)
        , pFullPath(DK_NULL)
    {
        // do nothing
    }
};

struct DKEOPFData
{
    DK_WCHAR* pDuokanBookId;                // duokan book id
    DKEOPFManifestItem* pManifest;          // manifest数据的数组
    DK_INT manifestItemCount;               // manifest数据数组的大小
    DK_WCHAR** pSpine;                      // spine数据，存储所有章节指向的manifest item id，决定阅读索引
    DK_INT spineItemCount;                  // spine数组的大小  

    DKEOPFData()
        : pDuokanBookId(DK_NULL)
        , pManifest(DK_NULL)
        , manifestItemCount(0)
        , pSpine(DK_NULL)
        , spineItemCount(0)
    {
        // do nothing
    }
};

struct DKEEncryptionData
{
    DK_WCHAR* pCipherData;                    // 用于本地加密时获取AES密钥的字符串
    DK_WCHAR** pEncryptedFiles;               // 所有已加密文件在ePub container中的全路径
    DK_INT encryptedFileCount;                // 已加密的文件数

    DKEEncryptionData()
        : pCipherData(DK_NULL)
        , pEncryptedFiles(DK_NULL)
        , encryptedFileCount(0)
    {
        // do nothing
    }
};

struct DKEExtensionData
{
    DK_WCHAR* pExtensionVersion;            // extension 版本号
    DKEWRITINGOPT writingOpt;               // 行文方式
    DKEDISPLAYOPT displayOpt;               // 书籍展现方式

    DKEExtensionData()
        : pExtensionVersion(DK_NULL)
    {
        // do nothing
    }
};

enum DKE_PACK_MODE
{
    DKE_PACK_ENCRYPT_COMPRESS,      // 先加密再压缩
    DKE_PACK_COMPRESS_ENCRYPT       // 先压缩再加密
};

//===========================================================================

enum DKE_PAGEUNIT_TYPE
{
    DKE_PAGEUNIT_UNSUPPORT,
    DKE_PAGEUNIT_TEXT,
    DKE_PAGEUNIT_FOOTNOTE,
    DKE_PAGEUNIT_IMAGE,
    DKE_PAGEUNIT_PATH,
    DKE_PAGEUNIT_BLOCK
};

struct DKEPageTextInfo
{
    DK_POS          basePos;
    DK_WCHAR        textChar;
    DK_WCHAR*       fontName;
    DK_FLOAT        fontSize;
    DK_FONT_STYLE   fontStyle;
    DK_ARGBCOLOR    fontColor;

    DKEPageTextInfo()
        : textChar(0),
          fontName(DK_NULL)
    {}
};

enum DKE_PAGEATOM_TYPE
{
    DKE_PAGEATOM_TEXT,
    DKE_PAGEATOM_IMAGE,
    DKE_PAGEATOM_PATH
};

enum DKE_PAGEPATHNODE_TYPE
{
    DKE_PAGEPATHNODE_MOVETO,
    DKE_PAGEPATHNODE_LINETO,
    DKE_PAGEPATHNODE_QBEZIER           // 二次Bezier曲线
};

struct DKEPagePathNode
{
    DKE_PAGEPATHNODE_TYPE   nodeType;
    DK_POS                  endPoint;
    DK_POS                  controlPoint;               // 二次Bezier曲线的控制点
};

enum DKE_LINESTYLE_TYPE
{
    DKE_LINESTYLE_DOTTED,
    DKE_LINESTYLE_DASHED,
    DKE_LINESTYLE_SOLID,
    DKE_LINESTYLE_DOUBLE
};

struct DKEPagePathInfo
{
    DKEPagePathNode*    pathNodes;
    DK_UINT             pathNodeCount;
    DK_BOOL             fillPath;
    DK_ARGBCOLOR        fillColor;
    DK_BOOL             strokePath;
    DK_ARGBCOLOR        strokeColor;
    DKE_LINESTYLE_TYPE  lineType;
    DK_FLOAT            lineWidth;

    DKEPagePathInfo()
        : pathNodes(DK_NULL), pathNodeCount(0), lineType(DKE_LINESTYLE_SOLID)
    {}
};

#endif

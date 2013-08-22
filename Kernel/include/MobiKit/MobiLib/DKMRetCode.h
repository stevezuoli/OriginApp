//===========================================================================
// Summary:
//      MobiLib错误代码定义。
// Usage:
//      Null
// Remarks:
//      Null
// Date:
//      2012-02-25
// Author:
//      Zhai Guanghe (zhaigh@duokan.com)
//===========================================================================

#ifndef __KERNEL_MOBIKIT_MOBILIB_EXPORT_DKMRETCODE_H__
#define __KERNEL_MOBIKIT_MOBILIB_EXPORT_DKMRETCODE_H__

#include "KernelRetCode.h"

//===========================================================================
// 错误码分配

#define DKR_EPUB_CONTAINER_BASEVALUE    DKR_EPUB_BASEVALUE          // 包结构
#define DKR_EPUB_PARSER_BASEVALUE       DKR_EPUB_BASEVALUE + 200    // 解析
#define DKR_EPUB_RENDER_BASEVALUE       DKR_EPUB_BASEVALUE + 400    // 渲染
#define DKR_EPUB_INTERACT_BASEVALUE     DKR_EPUB_BASEVALUE + 600    // 页面交互
#define DKR_EPUB_HTMLPARSE_BASEVALUE    DKR_EPUB_BASEVALUE + 800    // HTML解析错误

//===========================================================================
// 包结构部分的错误码

// 找不到入口项
#define DKR_EPUB_CONTAINER_NOENTRYFOUND                  (DKR_EPUB_CONTAINER_BASEVALUE + 1L)
// 初始化错误
#define DKR_EPUB_CONTAINER_INIT_ERR                      (DKR_EPUB_CONTAINER_BASEVALUE + 2L)
// Entry名字长度为0
#define DKR_EPUB_CONTAINER_NAMELENZERO                   (DKR_EPUB_CONTAINER_BASEVALUE + 3L)
// Entry头数据读取失败
#define DKR_EPUB_CONTAINER_READENTRY_ERR                 (DKR_EPUB_CONTAINER_BASEVALUE + 4L)
// Entry头数据重名
#define DKR_EPUB_CONTAINER_ENTRYNAMEEXISTS               (DKR_EPUB_CONTAINER_BASEVALUE + 5L)
// 找不到对应的节点
#define DKR_EPUB_CONTAINER_NOENTRYMATCHED                (DKR_EPUB_CONTAINER_BASEVALUE + 6L)
// 创建流缓存失败
#define DKR_EPUB_CONTAINER_GETSTREAM_ERR                 (DKR_EPUB_CONTAINER_BASEVALUE + 7L)
// 创建文件缓存失败
#define DKR_EPUB_CONTAINER_CREATEFILECACHE_ERR           (DKR_EPUB_CONTAINER_BASEVALUE + 8L)
// 压缩方式不符合要求
#define DKR_EPUB_CONTAINER_COMPRESSMETHOD_ERR            (DKR_EPUB_CONTAINER_BASEVALUE + 9L)
// 包的实现方式不匹配
#define DKR_EPUB_CONTAINER_PACTYPE_ERR                   (DKR_EPUB_CONTAINER_BASEVALUE + 10L)
// 包mimetype错误
#define DKR_EPUB_CONTAINER_MIMETYPE_ERR                  (DKR_EPUB_CONTAINER_BASEVALUE + 11L)
// 初始化xml parser错误
#define DKR_EPUB_CONTAINER_INITXMLPARSER_ERR             (DKR_EPUB_CONTAINER_BASEVALUE + 12L)
// 从流获取内容错误
#define DKR_EPUB_CONTAINER_GETCONTENT_ERR                (DKR_EPUB_CONTAINER_BASEVALUE + 13L)
// open xml错误
#define DKR_EPUB_CONTAINER_OPENXML_ERR                   (DKR_EPUB_CONTAINER_BASEVALUE + 14L)
// xml root获取错误
#define DKR_EPUB_CONTAINER_GETXMLROOT_ERR                (DKR_EPUB_CONTAINER_BASEVALUE + 15L)
// 错误opf package
#define DKR_EPUB_CONTAINER_OPF_GETPACKAGE_ERR            (DKR_EPUB_CONTAINER_BASEVALUE + 16L)
#define DKR_EPUB_CONTAINER_OPF_GETMETADATA_ERR           (DKR_EPUB_CONTAINER_BASEVALUE + 17L)
#define DKR_EPUB_CONTAINER_OPF_GETADDITIONALMETADATA_ERR (DKR_EPUB_CONTAINER_BASEVALUE + 18L)
#define DKR_EPUB_CONTAINER_OPF_GETMENIFEST_ERR           (DKR_EPUB_CONTAINER_BASEVALUE + 19L)
#define DKR_EPUB_CONTAINER_OPF_PARSEMENIFEST_ERR         (DKR_EPUB_CONTAINER_BASEVALUE + 20L)
#define DKR_EPUB_CONTAINER_OPF_GETSPINE_ERR              (DKR_EPUB_CONTAINER_BASEVALUE + 21L)
#define DKR_EPUB_CONTAINER_OPF_PARSESPINE_ERR            (DKR_EPUB_CONTAINER_BASEVALUE + 22L)
#define DKR_EPUB_CONTAINER_ENCRYPTION_NOKEY              (DKR_EPUB_CONTAINER_BASEVALUE + 23L)
#define DKR_EPUB_CONTAINER_ENCRYPTED                     (DKR_EPUB_CONTAINER_BASEVALUE + 24L)
#define DKR_EPUB_MOBI_NO_TOC                             (DKR_EPUB_CONTAINER_BASEVALUE + 25L)
#define DKR_EPUB_MOBI_NO_MAIN_STREAM                     (DKR_EPUB_CONTAINER_BASEVALUE + 26L)
#define DKR_EPUB_BOOK_OPEN_FAIL                          (DKR_EPUB_CONTAINER_BASEVALUE + 27L)
#define DKR_EPUB_DRM_CHECK_VERSION_FAIL                  (DKR_EPUB_CONTAINER_BASEVALUE + 28L)

//===========================================================================
// 解析部分返回值错误码

// 静态变量初始化错误
#define DKR_EPUB_PARSER_STATICINIT_ERR                  DKR_EPUB_PARSER_BASEVALUE
// 初始化错误
#define DKR_EPUB_PARSER_BASEBOOKINIT_ERR                (DKR_EPUB_PARSER_BASEVALUE + 1L)
// 多次调用解析基本信息且上次解析已出错
#define DKR_EPUB_PARSER_LASTBASIC_ERR                   (DKR_EPUB_PARSER_BASEVALUE + 2L)
// 没有找到OCF主入口文件路径
#define DKR_EPUB_PARSER_NOOCFPATH                       (DKR_EPUB_PARSER_BASEVALUE + 3L)
// 页面解析器指针为空
#define DKR_EPUB_PARSER_PAGEPARSERNULL                  (DKR_EPUB_PARSER_BASEVALUE + 4L)
// 找不到页面对应包内文件
#define DKR_EPUB_PARSER_PAGELOCNULL                     (DKR_EPUB_PARSER_BASEVALUE + 5L)
// 基本信息解析入口参数不正确
#define DKR_EPUB_PARSER_BASICPARSERPARAM_ERR            (DKR_EPUB_PARSER_BASEVALUE + 6L)
// 页面文件解析入口参数不正确
#define DKR_EPUB_PARSER_PAGEPARSERPARAM_ERR             (DKR_EPUB_PARSER_BASEVALUE + 7L)
// CSS文件解析入口参数不正确
#define DKR_EPUB_PARSER_CSSPARSERPARAM_ERR              (DKR_EPUB_PARSER_BASEVALUE + 8L)
// 基本信息内容获取失败
#define DKR_EPUB_PARSER_BASICPARSERGETDATA_ERR          (DKR_EPUB_PARSER_BASEVALUE + 9L)
// 页面内容获取失败
#define DKR_EPUB_PARSER_PAGEPARSERGETDATA_ERR           (DKR_EPUB_PARSER_BASEVALUE + 10L)
// CSS内容获取失败
#define DKR_EPUB_PARSER_CSSPARSERGETDATA_ERR            (DKR_EPUB_PARSER_BASEVALUE + 11L)
// CSS编码不可识别或转码失败
#define DKR_EPUB_PARSER_CSSDECODING_ERR                 (DKR_EPUB_PARSER_BASEVALUE + 12L)
// 解析基本信息失败
#define DKR_EPUB_PARSER_BASICPARSE_ERR                  (DKR_EPUB_PARSER_BASEVALUE + 13L)
// 解析页面失败
#define DKR_EPUB_PARSER_PAGEPARSE_ERR                   (DKR_EPUB_PARSER_BASEVALUE + 14L)
// 解析CSS失败
#define DKR_EPUB_PARSER_CSSPARSE_ERR                    (DKR_EPUB_PARSER_BASEVALUE + 15L)
// 解析根文件状态错误
#define DKR_EPUB_PARSER_ROOTPARSESTATUS_ERR             (DKR_EPUB_PARSER_BASEVALUE + 16L)
// 解析目录文件状态错误
#define DKR_EPUB_PARSER_NCXPARSESTATUS_ERR              (DKR_EPUB_PARSER_BASEVALUE + 17L)
// 解析OPF状态错误
#define DKR_EPUB_PARSER_OPFPARSESTATUS_ERR              (DKR_EPUB_PARSER_BASEVALUE + 18L)
// 解析OPF找不到页面描述节点
#define DKR_EPUB_PARSER_OPFPARSENOPAGENODE              (DKR_EPUB_PARSER_BASEVALUE + 19L)
// 不认识的XHTML标记
#define DKR_EPUB_PARSER_PAGEUNKNOWNTAG                  (DKR_EPUB_PARSER_BASEVALUE + 20L)
// 解析页面状态错误
#define DKR_EPUB_PARSER_PAGESTATUS_ERR                  (DKR_EPUB_PARSER_BASEVALUE + 21L)
// 解析XML打开文件流错误
#define DKR_EPUB_PARSER_OPENFILESTREAM_ERR              (DKR_EPUB_PARSER_BASEVALUE + 22L)
// 解析XML读取文件流数据错误
#define DKR_EPUB_PARSER_READFILESTREAM_ERR              (DKR_EPUB_PARSER_BASEVALUE + 23L)
// 解析XML解析器返回错误（一般是语法错误）
#define DKR_EPUB_PARSER_PARSESTATUS_ERR                 (DKR_EPUB_PARSER_BASEVALUE + 24L)
// 没有找到页面Body内容
#define DKR_EPUB_PARSER_PAGENOBODYCONTENT               (DKR_EPUB_PARSER_BASEVALUE + 25L)
// 保存分页表失败
#define DKR_EPUB_PARSER_SAVEPAGEBREAK_ERR               (DKR_EPUB_PARSER_BASEVALUE + 26L)
// 获取data provider失败
#define DKR_EPUB_PARSER_GETDATAPROVIDER_ERR             (DKR_EPUB_PARSER_BASEVALUE + 27L)
// 获取structure doc失败
#define DKR_EPUB_PARSER_GETSTRUCTUREDOC_ERR             (DKR_EPUB_PARSER_BASEVALUE + 28L)
// 获取html piece info失败
#define DKR_EPUB_PARSER_GETPIECEINFO_ERR                (DKR_EPUB_PARSER_BASEVALUE + 29L)
// structure doc为空
#define DKR_EPUB_PARSER_STRUCTUREDOCEMPTY_ERR           (DKR_EPUB_PARSER_BASEVALUE + 30L)

//===========================================================================
// 渲染部分返回值错误码

// 初始化设置失败
#define DKR_EPUB_RENDER_INIT_ERR                        (DKR_EPUB_RENDER_BASEVALUE + 1L)
// 解析调用状态非法
#define DKR_EPUB_RENDER_STATUS_ERR                      (DKR_EPUB_RENDER_BASEVALUE + 2L)
// 未正确打开
#define DKR_EPUB_RENDER_OPEN_ERR                        (DKR_EPUB_RENDER_BASEVALUE + 3L)
// 未正确解析流式信息
#define DKR_EPUB_RENDER_PARSESTRUCTURE_ERR              (DKR_EPUB_RENDER_BASEVALUE + 4L)
// 未正确生成版式信息
#define DKR_EPUB_RENDER_CREATEFIXED_ERR                 (DKR_EPUB_RENDER_BASEVALUE + 5L)
// 构建outputSystem错误
#define DKR_EPUB_RENDER_OUTPUTSYSTEM_ERR                (DKR_EPUB_RENDER_BASEVALUE + 6L)
// 构建device错误
#define DKR_EPUB_RENDER_DEVICE_ERR                      (DKR_EPUB_RENDER_BASEVALUE + 7L)
// 获取字体引擎失败
#define DKR_EPUB_RENDER_FONTENGINE_ERR                  (DKR_EPUB_RENDER_BASEVALUE + 8L)
// 获取文件内容失败
#define DKR_EPUB_RENDER_GETFILE_ERR                     (DKR_EPUB_RENDER_BASEVALUE + 9L)
// 图片输出失败
#define DKR_EPUB_RENDER_IMAGEOUTPUT_ERR                 (DKR_EPUB_RENDER_BASEVALUE + 10L)
// 构建画笔失败
#define DKR_EPUB_RENDER_PEN_ERR                         (DKR_EPUB_RENDER_BASEVALUE + 11L)
// 构建画刷失败
#define DKR_EPUB_RENDER_BRUSH_ERR                       (DKR_EPUB_RENDER_BASEVALUE + 12L)
// 构建渲染路径失败
#define DKR_EPUB_RENDER_PATH_ERR                        (DKR_EPUB_RENDER_BASEVALUE + 13L)

// 页面交互部分返回值错误码
#define DKR_EPUB_INTERACT_GETSTREAM_ERR                 (DKR_EPUB_INTERACT_BASEVALUE + 1L)
#define DKR_EPUB_INTERACT_GETLINKINFO_ERR               (DKR_EPUB_INTERACT_BASEVALUE + 2L)
#define DKR_EPUB_INTERACT_LINKUNKNOWN_ERR               (DKR_EPUB_INTERACT_BASEVALUE + 3L)

// HTML解析错误
#define DKR_EPUB_HTMLPARSE_READCHAR_ERR                 (DKR_EPUB_HTMLPARSE_BASEVALUE + 1L)
#define DKR_EPUB_HTMLPARSE_OFFSETCONVERT_ERR            (DKR_EPUB_HTMLPARSE_BASEVALUE + 2L)
#define DKR_EPUB_HTMLPARSE_OFFSETCHECK_ERR              (DKR_EPUB_HTMLPARSE_BASEVALUE + 3L)
//===========================================================================

#endif // #__KERNEL_MOBIKIT_MOBILIB_EXPORT_DKMRETCODE_H__

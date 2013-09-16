/**@file FileSorts_DK.h
 *
 * @brief 多看系统底层文件搜索排序处理接口
 *
 * @version 1.0
 *
 * @author wh
 * @date 2011-4-12
 * \n
 * @b 版权信息：
 *
 * - 北京多看科技有限公司 Copyright (C) 2011 - 2012
 *
 * _______________________________________________________________________
 */

#ifndef _DK_FILESORTS_H_
#define _DK_FILESORTS_H_

#include "Common/File_DK.h"



//搜索文件方式
//enum DK_FileSorts
//{
//    UnknowSort =0,          //获取某一类型文件 比如书（图）、音、视频
//    RecentlyAdd,            //最近加入
//    RecentlyReadTime,      //最近阅读
//    Name,                   //书名关键字
//    DIRECTORY               //目录
//};

enum Field
{
    FIELD_NONE = 0,                ///< Do not sort.
    LAST_ACCESS_TIME,
    BY_DIRECTORY,
    RECENTLY_ADD,
    NAME,
    TITLE,
    DESCRIPTION,
    SIZE,
    RATING,
    READ_COUNT,
    CREATE_TIME,
    NODE_TYPE,
    NODE_STATUS,
    NOT_ON_LOCAL,
    EXPAND
};

class CDKFileSorts
{
public:

public:
    CDKFileSorts();

public:
    void    SortsFile();

public:
    void SetFileListAndNum(const DKFileList& files);
    void SetFileSorts(Field Sort);
    void SetFileCategory(DkFormatCategory category);
    const DKFileList& GetFileSortResult() const
    {
        return m_sortedFiles;
    }

private:
    void    SortsFileByRecentlyReadTime();
    void    SortsFileByFileNameDefault();
    void    SortsFileByRecentlyAdd();
    void    SortsFileByFileName();

	//isNameSort 是判断是由按书名排序进入此函数还是由字母查找进入
	//只有按书名是才需要全部解析文件
    void    CreateFileListIndexbyDefault(bool isNameSort);


private:
    DkFormatCategory  m_fileCategory;
    Field m_sort;
    DKFileList m_files;
    DKFileList m_sortedFiles;
};

#endif /*_DK_FILESORTS_H_*/


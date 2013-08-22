//===========================================================================
// Summary:
//        工厂类用于生成一条阅读数据
// Usage:
//        不同的方法生成不同的阅读数据
// Remarks:
//        NULL
// Date:
//        2011-12-17
// Author:
//        Xu Kai (xukai@duokan.com)
//===========================================================================

#ifndef _DKXREADINGDATAITEMFACTORY_H_
#define _DKXREADINGDATAITEMFACTORY_H_
#include "DKXManager/ReadingInfo/ICT_ReadingDataItem.h"

class DKXReadingDataItemFactory
{
public:
    //----------------------------------------------------
    //  Summary:
    //      用于生成一条阅读数据
    //  Parameters:
    //      [in] _eType    生成的阅读数据的类型
    //  Return Value:
    //      返回生成的阅读数据对象的指针
    //  Remarks:
    //      NULL
    //----------------------------------------------------
    static ICT_ReadingDataItem * CreateReadingDataItem(ICT_ReadingDataItem::UserDataType _eType = ICT_ReadingDataItem::BOOKMARK);

    //----------------------------------------------------
    //  Summary:
    //      用于生成一条阅读数据
    //  Parameters:
    //      [in] _eType           本次创建Item的类型
    //      [in] _clsStartPos     起始三级索引
    //      [in] _clsEndPos       结束三级索引，对于书签和批注，可设置该对象内容和_clsStartPos相同
    //      [in] _strBookComment  Item的内容，为起始索引附近的文本，方便内核升级时利用该内容获得准确的三级索引
    //      [in] _strUserComment  对于批注，其为用户的输入数据，对于书摘/书签，该字段被忽略
    //  Return Value:
    //      返回生成的阅读数据对象的指针
    //  Remarks:
    //      NULL
    //----------------------------------------------------
    static ICT_ReadingDataItem * CreateReadingDataItem(ICT_ReadingDataItem::UserDataType _eType, const CT_RefPos &_clsStartPos, const CT_RefPos &_clsEndPos, 
                                                       const string &_strBookComment, const string &_strUserComment);

    static void DestoryReadingDataItem(ICT_ReadingDataItem *&_pReadingData);
};
#endif

#include "DKXManager/DKX/DKXReadingDataItemFactory.h"
#include "DKXManager/ReadingInfo/CT_ReadingDataItemImpl.h"
#include "Utility.h"
#include "time.h"

ICT_ReadingDataItem * DKXReadingDataItemFactory::CreateReadingDataItem(ICT_ReadingDataItem::UserDataType _eType)
{
    return new CT_ReadingDataItemImpl(_eType);
}

ICT_ReadingDataItem * DKXReadingDataItemFactory::CreateReadingDataItem(ICT_ReadingDataItem::UserDataType _eType, const CT_RefPos &_clsStartPos, const CT_RefPos &_clsEndPos, const string &_strBookComment, const string &_strUserComment)
{
    ICT_ReadingDataItem *pRet = new CT_ReadingDataItemImpl(_eType);
    if(pRet)
    {
        pRet->SetBeginPos(_clsStartPos);
        pRet->SetEndPos(_clsEndPos);
        pRet->SetBookContent(_strBookComment);
        if(ICT_ReadingDataItem::COMMENT == _eType) // 如果为批注，说明为用户输入内容
        {
            pRet->SetUserContent(_strUserComment);
        }

        time_t tTime = time(NULL);
        string strTime  = DkFormat::CUtility::TransferTimeToString(tTime);
        pRet->SetCreator("DuoKan");
        pRet->SetCreateTime(strTime);
        pRet->SetLastModifyTime(strTime);
    }

    return pRet;
}

void DKXReadingDataItemFactory::DestoryReadingDataItem(ICT_ReadingDataItem *&_pReadingData)
{
    if(NULL != _pReadingData)
    {
        delete _pReadingData;
        _pReadingData = NULL;
    }
}


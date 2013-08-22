//===========================================================================
// Summary:
//        IPdfHandle.h pdf库接口.
// Version:
//        0.1 draft - creating.
// Usage:
//        Null
// Remarks:
//        Null
// Date:
//        2011-12-06
// Author:
//        Null
//===========================================================================

#ifndef __KINDLEAPP_INC_IPDFHANDLE_H__
#define __KINDLEAPP_INC_IPDFHANDLE_H__

class IPdfHandle
{
public:
    IPdfHandle(){}
    virtual ~IPdfHandle(){}

    virtual DK_BOOL Initialize(LPCSTR lpszFilePath)= 0;
    virtual DK_BOOL GetPage()=0;
    virtual DK_BOOL SetPage()=0;
    virtual DK_INT GetCurPageNum()=0;
    virtual DK_BOOL SetScreenRes(UINT uWidth, UINT uHeight)=0;
    virtual DK_BOOL SetRotation(int iRotation)=0;
    virtual DK_INT GetPageCount(UINT *puPageCount)=0;
    virtual DK_BOOL GetPageText(UINT uPageNum, BYTE **ppbTextBuf, UINT *puTextLen)=0;
    virtual DK_BOOL GetChapterList(std::vector<IBookmark *> &rChapterList)=0;
    virtual void CloseCache()=0; //文本模式调用此接口停止图片的输出
};

#endif

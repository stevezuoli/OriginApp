//===========================================================================
// Summary:
//	    XMLDomDocument.h
// Usage:
//      封装DkTiXmlDocument, 可从文件或内存中加载一颗XML树，获取XML的根节点，保存XML到一个文件等
//  		...
// Remarks:
//	    该解析不支持DTD，只支持UTF8编码和standalone的XML
// Date:
//	    2011-09-15
// Author:
//	    Zhang Jiafang(zhangjf@duokan.com)
//===========================================================================

#ifndef __KERNEL_COMMONLIBS_XMLBASE_XMLDOMDOCUMENT_H__
#define __KERNEL_COMMONLIBS_XMLBASE_XMLDOMDOCUMENT_H__

//===========================================================================

#include "XMLBase.h"
#include "XMLDomNode.h"

class XMLDomDocument
{
public:
    XMLDomDocument();
    virtual ~XMLDomDocument();

public:
    //-------------------------------------------
    //	Summary:
    //	    load xml from a file, overloaded
    //	Parameters:
    //	    [in]pFileName               - the utf8 file name
    //      [in]condenseWhiteSpace      - whether to condense the white space
    //	Remarks:
    //	   null
    //	Return Value:
    //	    true, no errors occur in the parsing process
    //		false, else
    //-------------------------------------------
    DK_BOOL LoadXmlFromFile(const DK_CHAR* pFilePath, DK_BOOL condenseWhiteSpace);
    DK_BOOL LoadXmlFromFile(const DK_WCHAR* pFilePath, DK_BOOL condenseWhiteSpace);

    //-------------------------------------------
    //	Summary:
    //	    Load xml from buffer, overloaded
    //	Parameters:
    //	    [in]pXmlBuf                 - the xml buffer, utf8 or unicode
    //      [in]condenseWhiteSpace      - whether to condense the white space
    //	Remarks:
    //      null
    //	Return Value:
    //	    true, no errors occur in the parsing process
    //		false, else
    //-------------------------------------------
    DK_BOOL LoadXmlFromBuffer(const DK_CHAR* pXmlBuf, DK_BOOL condenseWhiteSpace);
    DK_BOOL LoadXmlFromBuffer(const DK_WCHAR* pXmlBuf, DK_BOOL condenseWhiteSpace);

    //-------------------------------------------
    //	Summary:
    //	    Save xml string to a file, overloaded
    //	Parameters:
    //	    [in]pFileName               - the file name
    //      or
    //      [in]strFileName             - the unicode file name, 
    //                                    and will be convert to UTF8 char file name
    //	Remarks:
    //	    null
    //	Return Value:
    //	    true, no errors occur when saving
    //	    false, else
    //-------------------------------------------
    DK_BOOL SaveXmlToFile(const DK_CHAR* pFileName) const;
    DK_BOOL SaveXmlToFile(const DK_WCHAR* pFileName) const;

    //-------------------------------------------
    //	Summary:
    //	    Save xml string to a buffer
    //	Parameters:
    //	    [in]ppXmlBuffer               - the pointer to the address of the buffer
    //      [in]pLength                   - the length of the buffer 
    //	Remarks:
    //	    Call FreeXmlBuffer to free the buffer
    //	Return Value:
    //	    true, no errors occur when saving
    //	    false, else
    //-------------------------------------------
    DK_BOOL SaveXmlToBuffer(DK_CHAR** ppXmlBuffer, DK_SIZE_T* pLength) const;

    //-------------------------------------------
    //	Summary:
    //	    Free the xml buffer
    //	Parameters:
    //	    [in]pXmlBuffer               - the address of the buffer
    //	Remarks:
    //	    DK_NULL
    //	Return Value:
    //	    DK_VOID
    //-------------------------------------------
    DK_VOID FreeXmlBuffer(DK_CHAR* pXmlBuffer);

    //-------------------------------------------
    //	Summary:
    //	    free the mem of all the dom node contained in the document
    //	Parameters:
    //	    null
    //	Remarks:
    //	    null
    //	Return Value:
    //	    DK_VOID
    //-------------------------------------------
    DK_VOID FreeAllDomNode();

public:
    //-------------------------------------------
    //	Summary:
    //	    Get the root element of the dom tree, overloacded
    //	Parameters:
    //	    null
    //	Remarks:
    //	    null
    //	Return Value:
    //	    the pointer of the root element 
    //-------------------------------------------
    const XMLDomNode* RootElement() const;
    XMLDomNode* RootElement();

    //-------------------------------------------
    //	Summary:
    //	    Convert the document to a dom node, overloacded
    //	Parameters:
    //	    null
    //	Remarks:
    //	    When just reading a dom tree, call const function.
    //      When writing to a dom tree, call common function.
    //	Return Value:
    //	    the pointer of the node 
    //-------------------------------------------
    const XMLDomNode* ToDomNode() const;
    XMLDomNode* ToDomNode();

public:
    //-------------------------------------------
    //	Summary:
    //	    get the declaration version
    //	Parameters:
    //	    NULL
    //	Remarks:
    //	    null
    //	Return Value:
    //	    the  xml declaration version
    //-------------------------------------------
    const DK_CHAR* GetDeclVersion() const;

    //-------------------------------------------
    //	Summary:
    //	    get the declaration version
    //	Parameters:
    //	    NULL
    //	Remarks:
    //	    null
    //	Return Value:
    //	    the xml declaration encoding
    //-------------------------------------------
    const DK_CHAR* GetDeclEncoding() const;
    

    //-------------------------------------------
    //	Summary:
    //	    get the declaration standalone
    //	Parameters:
    //	    NULL
    //	Remarks:
    //	    null
    //	Return Value:
    //	    the xml declaration standalone
    //-------------------------------------------
    const DK_CHAR* GetDeclStandalone() const;

public:
    //-------------------------------------------
    //	Summary:
    //	   static, Create an XMLDomDocuent
    //	Parameters:
    //	    [in]condenseWhiteSpace      - whether to condense the white space
    //	Remarks:
    //	    The difference between this function and new operator is that
    //      this function will create the root node of the dom tree, but 
    //      the new operator will not. If just reading an xml to a dom tree,
    //      could use new operator and then call load function. If needing 
    //      to generate an xml dom tree, must call this function to create
    //      the document.
    //	Return Value:
    //	    the pointer of the node which is newed inside,
    //      and must call ReleaseDocument to free the mem
    //-------------------------------------------
    static XMLDomDocument* CreateDocument(DK_BOOL condenseWhiteSpace = DK_TRUE);

    //-------------------------------------------
    //	Summary:
    //	   static, Release an XMLDomDocuent
    //	Parameters:
    //	    null
    //	Remarks:
    //	    null
    //	Return Value:
    //	    DK_VOID
    //-------------------------------------------
    static DK_VOID ReleaseDocument(XMLDomDocument* pDocument);

private:
    DK_VOID* m_pDocument;
    XMLDomNode* m_pRootElement;
    XMLDomNode* m_pSelfNode;
};

#endif

//===========================================================================
// Summary:
//	    XMLSaxParser.h
// Usage:
//      XML的SAX解析器，可从文件中加载XML或从缓冲区中加载XML，
//      并生成XMLDomDocument对象
//  		...
// Remarks:
//	    该解析不支持DTD, 只支持UTF8编码和standalone的XML
// Date:
//	    2011-09-27
// Author:
//	    Zhang Jiafang(zhangjf@duokan.com)
//===========================================================================

#ifndef __KERNEL_COMMONLIBS_XMLBASE_XMLSAXPARSER_H__
#define __KERNEL_COMMONLIBS_XMLBASE_XMLSAXPARSER_H__

#include "XMLSaxHandler.h"

enum XMLSaxStatus
{
    XML_PARSER_ERROR,
    XML_PARSER_OK
};

class XMLSaxParser
{
public:
    XMLSaxParser();
    virtual ~XMLSaxParser();

public:
    //-------------------------------------------
    //	Summary:
    //		Create a sax parser, which dose not support namespace
    //	Parameters:
    //		[in] encoding			- the encoding
    //	Remarks:
    //		only support utf8
    //	Return Value:
    //		DK_ReturnCode
    //-------------------------------------------
    DK_ReturnCode CreateParser(EncodingUtil::Encoding encoding = EncodingUtil::UTF8);

    //-------------------------------------------
    //	Summary:
    //		Create a sax parser, which dose support namespace
    //	Parameters:
    //		[in] encoding			- the encoding
    //      [in] nsSeperator        - the seperator char to seperate
    //                                the namespace and the element name 
    //                                or attribute in the elementhandler args
    //	Remarks:
    //		only support utf8
    //	Return Value:
    //		DK_ReturnCode
    //-------------------------------------------
    DK_ReturnCode CreateParserWithNS(DK_CHAR nsSeperator, EncodingUtil::Encoding encoding = EncodingUtil::UTF8);

    //-------------------------------------------
    //	Summary:
    //		destroy the parser;
    //	Parameters:
    //		null
    //	Remarks:
    //		null
    //	Return Value:
    //		DK_VOID
    //-------------------------------------------
    DK_VOID DestroyParser();

    //-------------------------------------------
    //	Summary:
    //		reset the parser;
    //	Parameters:
    //		null
    //	Remarks:
    //		null
    //	Return Value:
    //		DK_ReturnCode
    //-------------------------------------------
    DK_ReturnCode ResetParser();

    //-------------------------------------------
    //	Summary:
    //		parse the buffer passed in, overloaded
    //	Parameters:
    //		[in] pXMLBuffer			- the utf8 buffer
    //      or      
    //      [in] strXMLBuffer       - the unicode buffer
    //      [in] length             - the buffer length
    //      [in] isLast             - if is the last piece of the buffer
    //	Remarks:
    //		null
    //	Return Value:
    //		XMLSaxStatus
    //-------------------------------------------
    XMLSaxStatus Parse(const DK_CHAR* pXMLBuffer, DK_INT length, DK_BOOL isLast = DK_TRUE);
    XMLSaxStatus Parse(const DK_WCHAR* pXMLBuffer, DK_BOOL isLast = DK_TRUE);

    //-------------------------------------------
    //	Summary:
    //		stop the parser
    //	Parameters:
    //		[in] resumable         - if the parser can be resume after stopped
    //	Remarks:
    //		null
    //	Return Value:
    //		XMLSaxStatus
    //-------------------------------------------
    XMLSaxStatus StopParser(DK_BOOL resumable);

    //-------------------------------------------
    //	Summary:
    //		resume the parser
    //	Parameters:
    //		null
    //	Remarks:
    //		null
    //	Return Value:
    //		XMLSaxStatus
    //-------------------------------------------
    XMLSaxStatus ResumeParser();

    //-------------------------------------------
    //	Summary:
    //		get current byte offset while parsing
    //	Parameters:
    //		null
    //	Remarks:
    //		null
    //	Return Value:
    //		DK_LONG the byte offset
    //-------------------------------------------
    DK_LONG GetCurByteOffset();

public:
    //-------------------------------------------
    //	Summary:
    //		set the sax call backs
    //	Parameters:
    //		null
    //	Remarks:
    //		if using, must be called before the Parse
    //	Return Value:
    //		DK_VOID
    //-------------------------------------------
    DK_VOID SetElementHandler(StartElementHandler startElementHndl, EndElementHandler endElementHndl);
    DK_VOID SetStartElementHandler(StartElementHandler startElementHndl);
    DK_VOID SetEndElementHandler(EndElementHandler endElementHndl);
    DK_VOID SetCharacterDataHandler(CharacterDataHandler characterHndl);
    DK_VOID SetCommentHandler(CommentHandler commentHndl);
    DK_VOID SetCDATAHandler(StartCDATAHandler startCDATAHndl, EndCDATAHandler endCDATAHndl);
    DK_VOID SetSkippedEntityHandler(SkippedEntityHandler skippedEntityHndl);
    DK_VOID SetNamespaceDeclHandler(StartNamespaceDeclHandler startNSDeclHndl, EndNamespaceDeclHandler endNSDeclHndl);
    DK_VOID SetDeclarationHandler(XMLDeclarationHandler xmlDeclarationHndl);

public:
    //-------------------------------------------
    //	Summary:
    //		set the user data
    //	Parameters:
    //		[in]pUserData       - the pointer to the user data
    //	Remarks:
    //		if using, Must be called before Parse
    //	Return Value:
    //		DK_VOID
    //-------------------------------------------
    DK_VOID SetUserData(DK_VOID* pUserData);

    //-------------------------------------------
    //	Summary:
    //		get the user data
    //	Parameters:
    //		null
    //	Remarks:
    //		null
    //	Return Value:
    //		DK_VOID
    //-------------------------------------------
    DK_VOID* GetUserData();

    //-------------------------------------------
    //	Summary:
    //		set the parse whether to skip some undefined entity
    //	Parameters:
    //		[in]ifSkipEntity        - whether to skip some undefined entity
    //	Remarks:
    //		Must be called before Parse
    //	Return Value:
    //		DK_VOID
    //-------------------------------------------
    DK_ReturnCode SetSkipppingEntity(DK_BOOL ifSkipEntity);

    //-------------------------------------------
    //	Summary:
    //		set the form of the NS infomation of the element name or attribute 
    //      that is passed to the element handler.
    //      if 
    //	Parameters:
    //		[in]IsWithPrefix        - whether to carry the NS prefix of 
    //                                the element name or attribute.
    //                                if TRUE, the form is URI+nsSeperator+name+nsSeperator+prefix
    //                                else URI+nsSeperator+name
    //	Remarks:
    //		Must be called before Parse
    //	Return Value:
    //		DK_VOID
    //-------------------------------------------
    DK_VOID SetReturnNSTriplet(DK_BOOL IsWithPrefix);

private:
    DK_VOID* m_pParser;
    DK_BOOL m_isNSParser;

    StartElementHandler m_pStartElementHndl;
    EndElementHandler m_pEndElementHndl;
    CharacterDataHandler m_pCharacterDataHndl;
    CommentHandler m_pCommentHndl;
    StartCDATAHandler m_pStartCDATAHndl;
    EndCDATAHandler m_pEndCDATAHndl;
    SkippedEntityHandler m_pSkippedEntityHndl;
    StartNamespaceDeclHandler m_pStartNSDeclHndl;
    EndNamespaceDeclHandler m_pEndNSDeclHndl;
    XMLDeclarationHandler m_pXMLDeclarationHndl;
};

#endif

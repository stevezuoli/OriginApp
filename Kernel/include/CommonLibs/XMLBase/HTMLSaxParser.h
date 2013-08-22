//===========================================================================
// Summary:
//		HTMLParser.h
// Usage:
//		htmlParser
// Remarks:
//		Null
// Date:
//		2012-01-06
// Author:
//		pengf(pengf@duokan.com)
// Modified£º
//      ZhangJiafang(zhangjf@duokan.com)
//===========================================================================

#ifndef __KERNEL_COMMONLIBS_XMLBASE_HTMLSAXPARSER_H__
#define __KERNEL_COMMONLIBS_XMLBASE_HTMLSAXPARSER_H__

#include "XMLBase.h"
#include "XMLSaxHandler.h"

class HTMLSaxParser
{
public:
    HTMLSaxParser();
    ~HTMLSaxParser();

    enum OFFSET_TYPE
    {
        OFFSET_TYPE_START,
        OFFSET_TYPE_END
    };

public:
    //-------------------------------------------
    //	Summary:
    //		Create a sax parser
    //	Parameters:
    //		[in] encoding			- the encoding
    //	Remarks:
    //		only call once
    //	Return Value:
    //		DK_ReturnCode
    //-------------------------------------------
    DK_ReturnCode CreateParser(EncodingUtil::Encoding encoding = EncodingUtil::UTF8);

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
    //		parse the buffer passed in, overloaded
    //	Parameters:
    //		[in] htmlBuffer			- the html buffer
    //      [in] length             - the buffer length
    //      [in] isLast             - if is the last piece of the buffer
    //	Remarks:
    //		null
    //	Return Value:
    //		HTMLSaxStatus
    //-------------------------------------------
    DK_ReturnCode Parse(const DK_CHAR* pHtmlBuffer, DK_INT length, DK_BOOL isLast = DK_TRUE);

    //-------------------------------------------
    //	Summary:
    //		get current byte offset while parsing
    //	Parameters:
    //		null
    //	Remarks:
    //		For html segment, like: "<p>text</p>",
    //          when parsing start element p, the offset will be the start byte offset of "<p>";
    //          when parsing end element p, the offset will be the start byte offset of "</p>";
    //          when parsing text contained in the p tag,, the offset will be the start byte offset of "text".
    //      For html segment, like: "<br />"
    //          the offset will always be the start byte offset of "<br />"
    //	Return Value:
    //		DK_LONG the byte offset
    //-------------------------------------------
    DK_LONG GetCurByteOffset(OFFSET_TYPE offsetStatus);

    //-------------------------------------------
    //	Summary:
    //		get current byte offset while parsing
    //	Parameters:
    //		null
    //	Remarks:
    //      Can be only called when parsing tag.
    //		For html segment, like: "<p>text</p>",
    //          when parsing start element p, the offset will be the end byte offset of "<p>";
    //          when parsing end element p, the offset will be the end byte offset of "</p>";
    //      For html segment, like: "<br />"
    //          the offset will always be the end byte offset of "<br />"
    //	Return Value:
    //		DK_LONG the byte offset
    //-------------------------------------------
    DK_LONG GetTagPostByteOffset(OFFSET_TYPE offsetStatus);

public:
    //-------------------------------------------
    //	Summary:
    //		set the sax call backs
    //	Parameters:
    //		null
    //	Remarks:
    //		if using, must be called before CreateParser
    //	Return Value:
    //		DK_VOID
    //-------------------------------------------
    DK_VOID SetStartElementHandler(StartElementHandler startElementHndl);
    DK_VOID SetEndElementHandler(EndElementHandler endElementHndl);
    DK_VOID SetCharacterDataHandler(CharacterDataHandler characterHndl);

public:
    //-------------------------------------------
    //	Summary:
    //		set the user data
    //	Parameters:
    //		[in]userData       - the pointer to the user data
    //	Remarks:
    //		if using, Must be called before Parse
    //	Return Value:
    //		DK_VOID
    //-------------------------------------------
    DK_VOID SetUserData(DK_VOID* pUserData);

private:
    DK_VOID* m_pHandler;
    DK_VOID* m_pUserData;
    DK_VOID* m_pParserContext;

};
#endif

//===========================================================================
// Summary:
//		RichTextString.h
// Usage:
//	    RichTextString, the list of text in rich text style
// Remarks:
//		Null
// Date:
//		2011-9-1
// Author:
//		Zhang Jiafang(zhangjf@duokan.com)
//===========================================================================
#ifndef __KERNEL_COMMONLIBS_KERNELBASE_RICHTEXTSTRING_H__
#define __KERNEL_COMMONLIBS_KERNELBASE_RICHTEXTSTRING_H__

#include "KernelType.h"

struct DK_RICHTEXTNODE;

class RichTextString
{
public:
    RichTextString(DK_BOOL isCHSToCHT = DK_FALSE);
    RichTextString(const RichTextString& richTextString, DK_BOOL isCHSToCHT = DK_FALSE);
    ~RichTextString();

public:
    DK_VOID AppendRichText(const DK_RICHTEXT& richText);
    const DK_RICHTEXT* GetRichText(DK_INT index) const;
    DK_SIZE_T GetSize() const;
    DK_BOOL IsEmpty() const;
    DK_VOID Clear();

public:
    const DK_WCHAR* GetPureText();

public:
    RichTextString& operator=(const RichTextString& rhs);
    RichTextString& operator+=(const DK_RICHTEXT& rhs)
    {
        AppendRichText(rhs);
        return *this;
    }

private:
    DK_RICHTEXTNODE* CreateRichTextNode();
    DK_VOID AppendRichTextNode(DK_RICHTEXTNODE* pNode);
    const DK_WCHAR* AppendBaseText(const DK_WCHAR* pText, DK_UINT* pTextLength);
    DK_VOID CloneRichTextString(const RichTextString& richTextString);

private:
    DK_RICHTEXTNODE* m_pHead;
    DK_RICHTEXTNODE* m_pTail;
    DK_SIZE_T m_size;
    DK_WCHAR** m_ppBaseText;
    DK_WCHAR* m_pPureText;
    DK_UINT m_pPureTextLength;
    DK_BOOL m_isChanged;
    DK_BOOL m_isCHSToCHT;
};

#endif

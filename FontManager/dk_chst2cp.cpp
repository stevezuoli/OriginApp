#include "FontManager/dk_chst2cp.h"
           
static const DK_CHST2CP gs_CHST2CP[] = 
{
    {DK_CHARSET_ANSI,    1252},
    {DK_CHARSET_SHITJIS, 932},
    {DK_CHARSET_JOHAB,   1361},
    {DK_CHARSET_GB,      936},
    {DK_CHARSET_BIG5,    950}
};

DK_UINT16 DK_GetCPByChst(DK_CHARSET_TYPE eCharset)
{
    DK_INT32 iEnd = sizeof(gs_CHST2CP)/sizeof(DK_CHST2CP) - 1;
    DK_ASSERT(iEnd >= 0);
    DK_INT32 iStart = 0, iMid = 0;
    do
    {
        iMid = (iStart + iEnd)/2;
        const DK_CHST2CP &cp = gs_CHST2CP[iMid];
        if (eCharset == cp.eCharset)
            return cp.uCodepage;
        else if (eCharset < cp.eCharset)
            iEnd = iMid - 1;
        else
            iStart = iMid + 1;            
    } while (iStart <= iEnd);
    
    return CODEPAGE_NOT_EXIST;
}

DK_CHARSET_TYPE DK_GetChstByCP(DK_UINT16 uCodepage)
{
    DK_INT32 iEnd = sizeof(gs_CHST2CP)/sizeof(DK_CHST2CP) - 1;
    DK_ASSERT(iEnd >= 0);
    for (DK_INT32 iStart = 0; iStart <= iEnd; iStart++)
    {
        const DK_CHST2CP &cp = gs_CHST2CP[iStart];
        if (cp.uCodepage == uCodepage)
            return cp.eCharset;
    }
    
    return DK_CHARSET_GB;
}


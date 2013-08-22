#ifndef FCFONTMANAGER_H
#define FCFONTMANAGER_H

#include "dkBaseType.h"
#include "dkWinError.h"
#include "fontconfig/fontconfig.h"

#define MAX_FONT_COUNT               (25)
#define MAX_FONT_NAME_LENGTH         (80)
#define MAX_FILE_NAME_LENGTH         (256)

typedef enum {
    DK_FONT_CLASS_ENGLISH,
    DK_FONT_CLASS_LOCAL,
    DK_FONT_CLASS_CHINESE
} DK_FONT_CLASS;

class CFCFontManager
{
public:
    static void InitFCAtSysStart ();
    static HRESULT GetAvailableFontList (DK_FONT_CLASS eFontClass, PCHAR pstrFontNameTable, INT iTableMaxRow, INT iTableMaxCol, PUINT puListCount);
    static HRESULT CheckFontsCache ();
    static HRESULT SetFontName (DK_FONT_CLASS eFontClass, PCCH pstrFontName, INT iLength);
    static BOOL IsFontDirModified ();
    static HRESULT ReinitializeFC ();

private:
    static HRESULT InitCNSFontList ();
    static HRESULT InitENFontList ();
    static HRESULT InitLocalFontList ();
    static INT GetDirFilesSize (PCCH pcchDir);
    static void CleanMultiNames (CHAR strTable[MAX_FONT_COUNT][MAX_FONT_NAME_LENGTH], UINT uCount);
    static HRESULT WriteFCConfFile (
            PCCH pstrENFontName,
            INT iENLength,
            PCCH pstrLocalFontName,
            INT iLocalLength,
            PCCH pstrCNSFontName,
            INT iCNSLength);
};

#endif // FCFONTMANAGER_H

#ifndef __KERNEL_TESTS_TESTEPUBLIB_DKETESTCONFIG_H__
#define __KERNEL_TESTS_TESTEPUBLIB_DKETESTCONFIG_H__

#include "KernelType.h"
#include <iterator>
#include "IDKEBook.h"
#include <map>
#include <vector>
#include "DKSimulatePageBehaviorIterator.h"

class DKETestConfig
{
public: 
    DKSimulatePageBehaviorIterator SimulateBegin()
    {
        return DKSimulatePageBehaviorIterator(m_simulateSeries.c_str());
    }

    DKSimulatePageBehaviorIterator SimulateEnd()
    {
        return DKSimulatePageBehaviorIterator(DK_NULL);
    }

    DK_BOX GetPageBox() const
    {
        return m_pageBox;
    }
    DK_BOX GetPaddingBox() const
    {
        return m_paddingBox;
    }
    DK_UINT GetWidth() const
    {
        return m_width;
    }
    DK_UINT GetHeight() const
    {
        return m_height;
    }
    DK_DOUBLE GetFontSize() const
    {
        return m_fontSize;
    }
    DK_DOUBLE GetParaSpacing() const
    {
        return m_paraSpacing;
    }
    DK_DOUBLE GetTabStop() const
    {
        return m_tabStop;
    }
    DK_DOUBLE GetFirstLineIndent() const
    {
        return m_firstLineIndent;
    }
    DK_ARGBCOLOR GetTextColor() const
    {
        return m_textColor;
    }
    const DK_CHAR* GetGbFont() const
    {
        return m_gbFont.c_str();
    }
    const DK_CHAR* GetAnsiFont() const
    {
        return m_ansiFont.c_str();
    }
    DK_UINT GetDpi() const
    {
        return m_dpi;
    }
    DK_BOOL DoSimulate() const
    {
        return !m_simulateSeries.empty();
    }
    DK_DOUBLE GetLineGap() const
    {
        return m_lineGap;
    }

    DK_BOOL UseOriginColor() const
    {
        return m_useOriginColor;
    }

    static DKETestConfig& GetInstance();

    DK_BOOL LoadConfig(const DK_CHAR* configFile);
    const std::map<DK_STRING, DK_STRING>& GetFontMap() const
    {
        return m_fontMap;
    }

    DK_BOOL HasFindKeyword() const
    {
        return !m_findKeyword.empty();
    }
    const DK_WCHAR* GetFindKeyword() const
    {
        return m_findKeyword.c_str();
    }
    DK_FLOWPOSITION GetFindStartPosition() const
    {
        return m_findStartPosition;
    }
    DK_BOOL GetShowTOC() const
    {
        return m_showTOC != 0;
    }
    DK_BOOL GetDoRender() const
    {
        return m_doRender != 0;
    }
    DK_BOOL GetDump() const
    {
        return m_dump != 0;
    }


    DK_BOOL GetLogEpub() const
    {
        return m_logEpub != 0;
    }

    DK_BOOL GetLogTest() const
    {
        return m_logTest != 0;
    }

    DK_BOOL GetLogMobi() const
    {
        return m_logMobi != 0;
    }

    DK_BOOL GetLogDump() const
    {
        return m_logDump != 0;
    }

    DK_BOOL GetDoParse() const
    {
        return m_doParse != 0;
    }

    DK_BOOL GetDoGeneratePageTable() const
    {
        return m_doGeneratePageTable != 0;
    }

    DK_BOOL GetDoSavePageBmp() const
    {
        return m_doSavePageBmp != 0;
    }

    DK_BOOL GetDoSavePageTxt() const
    {
        return m_doSavePageTxt != 0;
    }

    const DK_CHAR* GetPageTableFileName() const
    {
        return m_pageTableFile.c_str();
    }

    const std::vector<DKE_PAGEBREAK_ENV>& GetPageBreakEnvs() const
    {
        return m_pageBreakEnvs;
    }

private:
    DKETestConfig();
    DISALLOW_COPY_AND_ASSIGN(DKETestConfig);

    DK_BOX m_pageBox;
    DK_BOX m_paddingBox;
    DK_UINT m_width;
    DK_UINT m_height;
    DK_DOUBLE m_fontSize;
    DK_DOUBLE m_paraSpacing;
    DK_DOUBLE m_tabStop;
    DK_DOUBLE m_firstLineIndent;
    DK_DOUBLE m_lineGap;
    DK_ARGBCOLOR m_textColor;
    DK_STRING m_gbFont;
    DK_STRING m_ansiFont;
    DK_STRING m_simulateSeries;
    DK_UINT m_dpi;
    std::map<DK_STRING, DK_STRING> m_fontMap;
    DK_BOOL m_useOriginColor;
    DK_WSTRING m_findKeyword;
    DK_FLOWPOSITION m_findStartPosition;
    DK_INT m_showTOC;
    DK_INT m_doRender;
    DK_INT m_logEpub;
    DK_INT m_logTest;
    DK_INT m_logDump;
    DK_INT m_logMobi;
    DK_INT m_dump;
    DK_INT m_doParse;
    DK_INT m_doGeneratePageTable;
    DK_STRING m_pageTableFile;
    std::vector<DKE_PAGEBREAK_ENV> m_pageBreakEnvs;

    DK_INT m_doSavePageBmp;
    DK_INT m_doSavePageTxt;
};
#endif

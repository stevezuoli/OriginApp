#ifndef __COMMONUI_UIBOOKLISTPROGRESSBAR_H__
#define __COMMONUI_UIBOOKLISTPROGRESSBAR_H__

#include "GUI/UIWindow.h"
#include "GUI/UIContainer.h"

class UIBookListProgressBar : public UIWindow
{
public:
    virtual const char* GetClassName() const
    {
        return "UIBookListProgressBar";
    }

    UIBookListProgressBar();
    UIBookListProgressBar(UIContainer* parent);
    void SetDotInterval(int dotInterval);
    int GetDotInterval() const;
    void SetBarHeight(int barHeight);
    int GetBarHeight() const;
    

    void SetMaxDot(int maxDot);
    void SetPercentageProgess(int percentageProgress);

    virtual HRESULT Draw(DK_IMAGE drawingImg);

    void SetDrawBold(bool drawBold);
    
private:
    bool IsDrawBold() const;
    /// max value possible for the progress bar
    int m_barHeight;
    int m_maxDot;
    int m_percentageProgress;
    int m_dotInterval;

    bool m_drawBold;
    ////////////////////Field Section/////////////////////////
};
#endif

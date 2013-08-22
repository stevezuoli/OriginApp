#include "PersonalUI/UISingleLine.h"
#include "GUI/CTpGraphics.h"
#include "GUI/UISizer.h"
#include "Common/WindowsMetrics.h"
#include "GUI/ITpGraphics.h"
#include "GUI/UIImage.h"
#include "Utility/ColorManager.h"

using namespace WindowsMetrics;

UISingleLine::UISingleLine(UIContainer* pParent)
	: UIContainer(pParent, IDSTATIC)
{
}

UISingleLine::~UISingleLine()
{

}

HRESULT UISingleLine::DrawBackGround(DK_IMAGE drawingImg)
{
    HRESULT hr(S_OK);
    CTpGraphics grf(drawingImg);
	RTN_HR_IF_FAILED(grf.EraserBackGround());
	RTN_HR_IF_FAILED(grf.FillRect(0, 0, m_iWidth, m_iHeight, ColorManager::knBlack));
	return hr;
}


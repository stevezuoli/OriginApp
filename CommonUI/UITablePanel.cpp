////////////////////////////////////////////////////////////////////////
// UITablePanel.h
// Contact:
////////////////////////////////////////////////////////////////////////

#include <tr1/functional>
#include "CommonUI/UITablePanel.h"
#include "Framework/CDisplay.h"
#include "GUI/CTpGraphics.h"
#include "GUI/UISizer.h"
#include "Common/WindowsMetrics.h"

using namespace WindowsMetrics;

const char* UITablePanel::EventTablePanelChange = "TablePanelChange";

UITablePanel::UITablePanel()
            : UIContainer()
{
}

UITablePanel::~UITablePanel()
{
}

HRESULT UITablePanel::DrawBackGround(DK_IMAGE drawingImg)
{
    CTpGraphics grf(drawingImg);
    grf.EraserBackGround();
    return S_OK;
}

#ifdef KINDLE_FOR_TOUCH
bool UITablePanel::OnFling(MoveEvent* moveEvent1, MoveEvent* moveEvent2, FlingDirection direction, int vx, int vy)
{
	return true;
}
#endif


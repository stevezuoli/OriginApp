#include "TouchAppUI/UIPdfTrimColumnedDlg.h"
#include "GUI/UIContainer.h"
#include "CommandID.h"
#include "I18n/StringManager.h"
#include "GUI/CTpGraphics.h"
#include "drivers/DeviceInfo.h"

#define PDFTRIM_MARGIN 15
#define PDFTRIM_BUTTONSIZE 60
#define PDFTRIM_FONTSIZE 30

UIPdfTrimColumnedDlg::UIPdfTrimColumnedDlg(UIContainer* pParent, IBookReader* pBookReader, PdfTrimColumnedAction iPdfTrimColumnedAction) :
	UIDialog(pParent) ,
	m_pBookReader(pBookReader) ,
	m_iCurAction(iPdfTrimColumnedAction) ,
	m_iCurMovingCorner(MovingCornerInvalid) ,
	m_iCurMovingDir(MovingDirInvalid) ,
	m_bIsActionFinished(false)
{
	this->MoveWindow(0,0,GetScreenWidth(),GetScreenHeight());

	m_iTopCuttingEdge = PDFTRIM_MARGIN;
	m_iLeftCuttingEdge = PDFTRIM_MARGIN;
	m_iBottomCuttingEdge = m_iHeight - 2*PDFTRIM_MARGIN;
	m_iRightCuttingEdge = m_iWidth - 2*PDFTRIM_MARGIN;

	m_iColumnedX = m_iWidth>>1;
	m_iColumnedY = m_iHeight>>1;

	if (iPdfTrimColumnedAction == PdfColumnedComicOrderLR || iPdfTrimColumnedAction == PdfColumnedComicOrderRL) {
		m_iSplitPagesCounter = 2;
	} else if (iPdfTrimColumnedAction == PdfColumnedOrder1 || iPdfTrimColumnedAction == PdfColumnedOrder2 
			|| iPdfTrimColumnedAction == PdfColumnedOrder3 || iPdfTrimColumnedAction == PdfColumnedOrder4) {
		m_iSplitPagesCounter = 4;
	} else {
		m_iSplitPagesCounter = -1;
	}

	m_btnConfirmed.Initialize(ID_BTN_TOUCH_CONFIRMED, StringManager::GetPCSTRById(TOUCH_BOOKSETTING_CONFIRM), PDFTRIM_FONTSIZE);
	m_btnCancel.Initialize(ID_BTN_TOUCH_CANCEL, StringManager::GetPCSTRById(TOUCH_BOOKSETTING_CANCEL), PDFTRIM_FONTSIZE);
	m_btnTips.Initialize(IDSTATIC, StringManager::GetPCSTRById(TOUCH_PDFTRIM_TIPS), PDFTRIM_FONTSIZE);
	m_btnTips.SetCornerStyle(true,true,true,true);
	m_btnTips.SetAlign(ALIGN_CENTER);

	AppendChild(&m_btnConfirmed);
	AppendChild(&m_btnCancel);

	DebugPrintf(DLC_ZHY, "pdftrimdlg: %d,%d,%d,%d", PDFTRIM_MARGIN<<1, m_iTop+m_iHeight-PDFTRIM_BUTTONSIZE-PDFTRIM_MARGIN*2, PDFTRIM_BUTTONSIZE, PDFTRIM_BUTTONSIZE<<1);
	DebugPrintf(DLC_ZHY, "pdftrimdlg: %d,%d,%d,%d", m_iWidth-(PDFTRIM_MARGIN<<1)-(PDFTRIM_BUTTONSIZE<<1), m_iTop+m_iHeight-PDFTRIM_BUTTONSIZE - PDFTRIM_MARGIN*2,
		PDFTRIM_BUTTONSIZE<<1, PDFTRIM_BUTTONSIZE);
	m_btnConfirmed.MoveWindow(PDFTRIM_MARGIN<<1, m_iTop+m_iHeight-PDFTRIM_BUTTONSIZE-PDFTRIM_MARGIN*2, PDFTRIM_BUTTONSIZE<<1, PDFTRIM_BUTTONSIZE);
	m_btnCancel.MoveWindow(m_iWidth-(PDFTRIM_MARGIN<<1)-(PDFTRIM_BUTTONSIZE<<1), m_iTop+m_iHeight-PDFTRIM_BUTTONSIZE - PDFTRIM_MARGIN*2,
		PDFTRIM_BUTTONSIZE<<1, PDFTRIM_BUTTONSIZE);
	m_btnTips.MoveWindow(60, m_iHeight-PDFTRIM_BUTTONSIZE -5, m_iWidth-60*2, PDFTRIM_BUTTONSIZE);
	m_btnTips.SetVisible(true);
}

UIPdfTrimColumnedDlg::~UIPdfTrimColumnedDlg()
{
}

HRESULT UIPdfTrimColumnedDlg::Draw(DK_IMAGE drawingImg)
{
	DebugPrintf(DLC_ZHY,"enter %s::%s,%s(%d)",GetClassName(), __FUNCTION__, __FILE__, __LINE__);
	if (!IsVisible())
		return S_OK;
	HRESULT hr(S_OK);
	CTpGraphics grf(drawingImg);
	DK_IMAGE imgSelf;
	DK_RECT rcSelf={m_iLeft, m_iTop, m_iLeft + m_iWidth, m_iTop + m_iHeight};
	RTN_HR_IF_FAILED(CropImage(drawingImg, rcSelf, &imgSelf));

	RTN_HR_IF_FAILED(DrawBackGround(imgSelf));

	//draw flag
	SPtr<ITpImage> spImageCursor = ImageManager::GetImage(IMAGE_PDF_LOCATE_CURSOR);
	int w = spImageCursor->GetWidth();
	int h = spImageCursor->GetHeight();

	if (m_bIsActionFinished) 
    {
		m_btnConfirmed.SetVisible(true);
		m_btnCancel.SetVisible(true);
		RTN_HR_IF_FAILED(m_btnConfirmed.Draw(imgSelf));
		RTN_HR_IF_FAILED(m_btnCancel.Draw(imgSelf));
	} 
    else 
    {
		m_btnConfirmed.SetVisible(false);
		m_btnCancel.SetVisible(false);
	}

	//draw lines
	if (m_iCurAction == PdfTrimAction) 
    {
		//Trim
		grf.DrawImage(spImageCursor.Get(), m_iLeftCuttingEdge - w/2, m_iTopCuttingEdge -h/2);
		grf.DrawImage(spImageCursor.Get(), m_iRightCuttingEdge - w/2, m_iBottomCuttingEdge - h/2);

		DebugPrintf(DLC_ZHY,"rect,left,top,right,bottom:%d,%d,%d,%d",m_iLeftCuttingEdge, m_iTopCuttingEdge, m_iRightCuttingEdge, m_iBottomCuttingEdge);
		grf.DrawLine(m_iLeftCuttingEdge, m_iTop, 2, m_iHeight, SOLID_STROKE);
		grf.DrawLine(m_iRightCuttingEdge, m_iTop, 2, m_iHeight, SOLID_STROKE);
		grf.DrawLine(m_iLeft, m_iTopCuttingEdge, m_iWidth, 2, SOLID_STROKE);
		grf.DrawLine(m_iLeft, m_iBottomCuttingEdge, m_iWidth, 2, SOLID_STROKE);
	}
    else 
    {
		grf.DrawLine(m_iColumnedX, m_iTop, 2, m_iHeight, SOLID_STROKE);
		grf.DrawImage(spImageCursor.Get(), m_iColumnedX - w/2, m_iColumnedY - h/2);

		if (m_iCurAction==PdfColumnedComicOrderLR || m_iCurAction==PdfColumnedComicOrderRL) 
        {
			//split to 2 pages
		} 
        else 
        {
			//split to 4 pages
			grf.DrawLine(m_iLeft, m_iColumnedY, m_iWidth, 2, SOLID_STROKE);
		}
	}
	if (m_btnTips.IsVisible())
		m_btnTips.SetVisible(false);

	DebugPrintf(DLC_ZHY,"leave %s::%s,%s(%d)",GetClassName(), __FUNCTION__, __FILE__, __LINE__);

	return hr;
}

HRESULT UIPdfTrimColumnedDlg::DrawBackGround(DK_IMAGE drawingImg) 
{
	DebugPrintf(DLC_ZHY,"enter %s:%s,%s(%d)",GetClassName(),__FUNCTION__,__FILE__,__LINE__);
	HRESULT hr(S_OK);
	DK_IMAGE imgSelf;
	DK_RECT rcSelf={m_iLeft, m_iTop, m_iWidth, m_iHeight};

	//drawing original image
	DK_IMAGE* backGroundImage = m_pBookReader->GetPageBMP();
	rcSelf.top     = m_iHeight > backGroundImage->iHeight? (m_iHeight - backGroundImage->iHeight) >> 1: m_iTop;
	rcSelf.bottom  = rcSelf.top + backGroundImage->iHeight;
	rcSelf.left    = m_iWidth > backGroundImage->iWidth? (m_iWidth - backGroundImage->iWidth) >> 1: m_iLeft;
	rcSelf.right   = rcSelf.left + backGroundImage->iWidth;
	CTpGraphics grf(drawingImg);
	grf.EraserBackGround(ColorManager::knWhite);
	RTN_HR_IF_FAILED(CropImage(drawingImg, rcSelf, &imgSelf));
	CopyImage(imgSelf, *backGroundImage);

	DebugPrintf(DLC_ZHY,"leave %s:%s,%s(%d)",GetClassName(),__FUNCTION__,__FILE__,__LINE__);
	return hr;
}

void UIPdfTrimColumnedDlg::OnCommand(DWORD dwCmdId, UIWindow * pSender, DWORD dwParam)
{
	DebugPrintf(DLC_ZHY,"enter %s::%s,%s(%d)",GetClassName(), __FUNCTION__, __FILE__, __LINE__);
	switch (dwCmdId) {
	case ID_BTN_TOUCH_CONFIRMED:
	case ID_BTN_TOUCH_CANCEL: 
		{
			bool r = dwCmdId==ID_BTN_TOUCH_CONFIRMED ? true : false;
			MaybeSaveChanges(r);
			break;
		}
	default:
		DebugPrintf(DLC_ZHY,"error!worng ID %s::%s,%s(%d)",GetClassName(), __FUNCTION__, __FILE__ ,__LINE__);
		return;
	}
	DebugPrintf(DLC_ZHY,"leave %s::%s,%s(%d)",GetClassName(), __FUNCTION__, __FILE__, __LINE__);
}

bool UIPdfTrimColumnedDlg::OnTouchEvent(MoveEvent* moveEvent)
{
    int action = moveEvent->GetActionMasked();
	DebugPrintf(DLC_GESTURE,"enter %s::%s,%s(%d), action: %d",GetClassName(), __FUNCTION__, __FILE__, __LINE__, action);
    if (action != MoveEvent::ACTION_DOWN && action != MoveEvent::ACTION_UP)
    {
        //avoid too much draw-action on the screen when GETSTURE_MOVE
        return true;
    }

	int x = moveEvent->GetX();
	int y = moveEvent->GetY();
	int recogsize = 100;//only points in the recognize-rect cause the lines moved
	int finishsize = 200;//points in the finish-rect mean showing save-quit btns
	//int	finishCenterX = m_iWidth>>1;
	int	finishCenterY = m_iHeight>>1;
	//bool lastStatus;

    if (m_iCurAction == PdfTrimAction) 
    {
        if (MoveEvent::ACTION_DOWN == action)
        {
            //lastStatus  = m_bIsActionFinished;
            m_bIsActionFinished = false;


            m_iCurMovingCorner = HitTest(x, y);
            DebugPrintf(DLC_GESTURE, "MovingCorner: (%d, %d): %d", x, y, m_iCurMovingCorner);
            if (m_iCurMovingCorner != MovingCornerInvalid)
            {
                UpdateMovingCorners(); 
                return true;
            }
            else if (y>=(finishCenterY - finishsize) && y<=(finishCenterY + finishsize)) 
            {
                m_bIsActionFinished = true;
            }
        }
        else if (MoveEvent::ACTION_UP == action && !m_bIsActionFinished) 
        {

			if (m_iCurMovingCorner == MovingTopLeft) 
            {
				m_iLeftCuttingEdge = x;
				m_iTopCuttingEdge = y;
			}
            else if (m_iCurMovingCorner == MovingBottomRight) 
            {
				m_iRightCuttingEdge = x;
				m_iBottomCuttingEdge = y;
			}
            else 
            {
				return true;
			}

            m_iCurMovingCorner = MovingCornerInvalid;
		}
	}
    else 
    {
		if (MoveEvent::ACTION_DOWN == action) 
        {
            DebugPrintf(DLC_GESTURE, "Column point: (%d,%d)", m_iColumnedX, m_iColumnedY);
            if (DeviceInfo::IsKPW())
            {
                recogsize = 50;
            }
            else
            {
                recogsize = 30;
            }
			//lastStatus  = m_bIsActionFinished;
			m_bIsActionFinished = false;
			m_iCurMovingDir = MovingDirInvalid;
			bool m_bIsAlreadyRecognized = false;

			if (x>=m_iColumnedX-recogsize && x<=m_iColumnedX+recogsize) 
            {
				m_iCurMovingDir |= MovingDirHorizontal;
				DebugPrintf(DLC_GESTURE,"start Horizon,cure moving:%d, %d",m_iCurMovingDir, (m_iCurMovingDir | MovingDirHorizontal));
				m_bIsAlreadyRecognized = true;
			}
			if (y>=m_iColumnedY-recogsize && y<=m_iColumnedY+recogsize) 
            {
				m_iCurMovingDir |= MovingDirVertical;
				DebugPrintf(DLC_GESTURE,"start Vertical,cure moving:%d, %d",m_iCurMovingDir, (m_iCurMovingDir | MovingDirHorizontal));
				m_bIsAlreadyRecognized = true;
			}
			if (/*x>=(finishCenterX - finishsize) && x<=(finishCenterX + finishsize) &&*/
				y>=(finishCenterY - finishsize) && y<=(finishCenterY + finishsize) && !m_bIsAlreadyRecognized) 
            {
                DebugPrintf(DLC_GESTURE, "split finished");
                m_bIsActionFinished = true;
			}
		} 
        else if (MoveEvent::ACTION_UP == action && !m_bIsActionFinished) 
        {
			DebugPrintf(DLC_GESTURE,"end cure moving:%d",m_iCurMovingDir);
			DebugPrintf(DLC_GESTURE, "before: x:%d,y:%d",m_iColumnedX, m_iColumnedY);
			if ((m_iCurMovingDir&MovingDirHorizontal) == MovingDirHorizontal) 
            {
				DebugPrintf(DLC_GESTURE,"hor: %d,%d",m_iCurMovingDir&MovingDirHorizontal, MovingDirHorizontal);
				m_iColumnedX = x;
			}
			if ((m_iCurMovingDir&MovingDirVertical) == MovingDirVertical) 
            {
				DebugPrintf(DLC_GESTURE,"ver: %d,%d",m_iCurMovingDir&MovingDirVertical, MovingDirVertical);
				m_iColumnedY = y;
			}
			DebugPrintf(DLC_ZHY, "before: x:%d,y:%d",m_iColumnedX, m_iColumnedY);
			m_iCurMovingDir = MovingDirInvalid;
		}
	}

	
	UpdateWindow();

	return true;
	DebugPrintf(DLC_ZHY,"leave %s::%s,%s(%d)",GetClassName(), __FUNCTION__, __FILE__, __LINE__);
}


void UIPdfTrimColumnedDlg::GetTrimColumnedChangedArgs(PdfModeController& pdfModeController)
{
	DebugPrintf(DLC_ZHY,"enter %s::%s,%s(%d), action:%d",GetClassName(), __FUNCTION__, __FILE__, __LINE__, m_iCurAction);
    DK_IMAGE* pCurImage = m_pBookReader->GetPageBMP();
    pdfModeController = *(m_pBookReader->GetPdfModeController());
    const int imageWidth = pCurImage->iWidth;
    const int imageHeight = pCurImage->iHeight;
	int iImgLeft   = m_iWidth > imageWidth ? (m_iWidth - imageWidth) >> 1: 0;
	int iImgTop    = m_iHeight > imageHeight ? (m_iHeight - imageHeight) >> 1: 0;
	int iImgRight  = iImgLeft + dk_min(imageWidth, m_iWidth);
	int iImgBottom = iImgTop + dk_min(imageHeight, m_iHeight);

	if (m_iCurAction==PdfTrimAction) {
        if (pdfModeController.m_eCuttingEdgeMode == PDF_CEM_Smart)
        {
            const double& leftEdge = pdfModeController.m_dLeftEdge;
            const double& topEdge = pdfModeController.m_dTopEdge;
            const double& rightEdge = pdfModeController.m_dRightEdge;
            const double& bottomEdge = pdfModeController.m_dBottomEdge;
            const double& verticalEdge = topEdge + bottomEdge;
            const double& horizontalEdge = leftEdge + rightEdge;
            const double& initWidth = imageWidth / (1.0 - horizontalEdge);
            const double& initHeight = imageHeight / (1.0 - verticalEdge);

            if (m_iTopCuttingEdge > iImgTop)
            {
                pdfModeController.m_dTopEdge = (DK_DOUBLE)(m_iTopCuttingEdge - iImgTop + (initHeight * topEdge)) / initHeight;
            }
            if (m_iBottomCuttingEdge < iImgBottom)
            {
                pdfModeController.m_dBottomEdge = (DK_DOUBLE)(iImgBottom - m_iBottomCuttingEdge + (initHeight * bottomEdge)) / initHeight;
            }
            if (m_iLeftCuttingEdge > iImgLeft)
            {
                pdfModeController.m_dLeftEdge = (DK_DOUBLE)(m_iLeftCuttingEdge - iImgLeft + (initWidth * leftEdge)) / initWidth;
            }
            if (m_iRightCuttingEdge < iImgRight)
            {
                pdfModeController.m_dRightEdge = (DK_DOUBLE)(iImgRight - m_iRightCuttingEdge + (initWidth * rightEdge)) / initWidth; 
            }
        }
        else
        {
            pdfModeController.m_dTopEdge = (m_iTopCuttingEdge > iImgTop) ? (DK_DOUBLE)(m_iTopCuttingEdge - iImgTop) / imageHeight : 0;
            pdfModeController.m_dBottomEdge = (m_iBottomCuttingEdge < iImgBottom) ? (DK_DOUBLE)(iImgBottom - m_iBottomCuttingEdge) / imageHeight : 0;
            pdfModeController.m_dLeftEdge = (m_iLeftCuttingEdge > iImgLeft) ? (DK_DOUBLE)(m_iLeftCuttingEdge - iImgLeft) / imageWidth : 0;
            pdfModeController.m_dRightEdge = (m_iRightCuttingEdge < iImgRight) ? (DK_DOUBLE)(iImgRight - m_iRightCuttingEdge) / imageWidth : 0;
        }
		pdfModeController.m_eCuttingEdgeMode = PDF_CEM_Custom;
	} else if (m_iCurAction==PdfColumnedComicOrderLR || m_iCurAction==PdfColumnedComicOrderRL) {
		pdfModeController.m_eReadingMode = PDF_RM_Split2Page;
		pdfModeController.m_uSubPageOrder = m_iCurAction - PdfColumnedComicOrderLR;
		pdfModeController.m_dHeight = 0.50;
		if (m_iColumnedX < iImgLeft || m_iColumnedX > iImgRight)
		{
			pdfModeController.m_dWidth = 0.50;
		}
		else
		{
			pdfModeController.m_dWidth = (DK_DOUBLE)(m_iColumnedX - iImgLeft) / imageWidth;
		}

	} else {
		pdfModeController.m_eReadingMode = PDF_RM_Split4Page;
		pdfModeController.m_uSubPageOrder = m_iCurAction - PdfColumnedOrder1;

		if (m_iColumnedX < iImgLeft || m_iColumnedX > iImgRight 
			|| m_iColumnedY < iImgTop || m_iColumnedY > iImgBottom)
		{
			pdfModeController.m_dWidth = 0.50;
			pdfModeController.m_dHeight = 0.50;
		}
		else
		{
			pdfModeController.m_dWidth = (DK_DOUBLE)(m_iColumnedX - iImgLeft) / imageWidth;
			pdfModeController.m_dHeight = (DK_DOUBLE)(m_iColumnedY - iImgTop) / imageHeight;
		}
	}
	DebugPrintf(DLC_ZHY,"leave %s::%s,%s(%d), action:%d",GetClassName(), __FUNCTION__, __FILE__, __LINE__, m_iCurAction);
}

void UIPdfTrimColumnedDlg::GetCuttingEdgeArgs(DK_DOUBLE& top, DK_DOUBLE& left, DK_DOUBLE& bottom, DK_DOUBLE& right)
{
	if (m_iCurAction!=PdfTrimAction) {
		DebugPrintf(DLC_ZHY,"error! %s::%s,%s(%d)",GetClassName(), __FUNCTION__, __FILE__, __LINE__);
		return;
    }
    top = left = bottom = right = 0;
	DK_IMAGE* pCurImage = m_pBookReader->GetPageBMP();
    if (pCurImage)
    {
        int iImgLeft   = m_iWidth > pCurImage->iWidth? (m_iWidth - pCurImage->iWidth) >> 1: 0;
        int iImgTop    = m_iHeight > pCurImage->iHeight? (m_iHeight - pCurImage->iHeight) >> 1: 0;
        int iImgRight  = iImgLeft + (m_iWidth > pCurImage->iWidth? pCurImage->iWidth: m_iWidth);
        int iImgBottom = iImgTop + (m_iHeight > pCurImage->iHeight? pCurImage->iHeight: m_iHeight);

        top	   = m_iTopCuttingEdge < iImgTop? 0: (DK_DOUBLE)(m_iTopCuttingEdge - iImgTop) / pCurImage->iHeight;
        bottom = m_iBottomCuttingEdge > iImgBottom? 0: (DK_DOUBLE)(iImgBottom - m_iBottomCuttingEdge) / pCurImage->iHeight;
        left = m_iLeftCuttingEdge < iImgLeft? 0: (DK_DOUBLE)(m_iLeftCuttingEdge - iImgLeft) / pCurImage->iWidth; 
        right = m_iRightCuttingEdge > iImgRight? 0: (DK_DOUBLE)(iImgRight - m_iRightCuttingEdge) / pCurImage->iWidth; 
    }
}

void UIPdfTrimColumnedDlg::GetColumnedArgs(ReadingMode& m_iReadingMode, DK_UINT& subPageOrder, DK_DOUBLE& width, DK_DOUBLE& height)
{
}

void UIPdfTrimColumnedDlg::MaybeSaveChanges(bool saved)
{
	DebugPrintf(DLC_ZHY,"enter %s::%s,%s(%d)",GetClassName(), __FUNCTION__, __FILE__, __LINE__);
	EndDialog(saved ? 1 : -1);
	SetVisible(false);
	m_btnConfirmed.SetVisible(false);
	m_btnCancel.SetVisible(false);
	m_bIsActionFinished = false;
	m_iCurMovingCorner = MovingCornerInvalid;
	DebugPrintf(DLC_ZHY,"leave %s::%s,%s(%d)",GetClassName(), __FUNCTION__, __FILE__, __LINE__);
}

void UIPdfTrimColumnedDlg::SetAction(PdfTrimColumnedAction action)
{
	m_iCurAction = action;
}

UIPdfTrimColumnedDlg::MovingCorner UIPdfTrimColumnedDlg::HitTest(int x, int y)
{
    DebugPrintf(DLC_GESTURE, "left top(%d, %d), right bottom(%d, %d), x y (%d %d)",
            m_iLeftCuttingEdge, m_iTopCuttingEdge, m_iRightCuttingEdge, m_iBottomCuttingEdge, x, y);
    const int RECOG_SIZE = 100;
    if ((x >= m_iLeftCuttingEdge - RECOG_SIZE) 
            && (x <= m_iLeftCuttingEdge + RECOG_SIZE)
            && (y >= m_iTopCuttingEdge - RECOG_SIZE)
            && (y <= m_iTopCuttingEdge + RECOG_SIZE))
    {
        return MovingTopLeft;
    }
    if ((x >= m_iRightCuttingEdge - RECOG_SIZE) 
            && (x <= m_iRightCuttingEdge + RECOG_SIZE)
            && (y >= m_iBottomCuttingEdge - RECOG_SIZE)
            && (y <= m_iBottomCuttingEdge + RECOG_SIZE))
    {
        return MovingBottomRight;
    }

    return MovingCornerInvalid;
}

void UIPdfTrimColumnedDlg::UpdateMovingCorners()
{
    int updateSize = 30;
    //UpdateWindow(m_iLeftCuttingEdge - updateSize, m_iTopCuttingEdge - updateSize, updateSize<<1, updateSize<<1);
    //UpdateWindow(m_iRightCuttingEdge - updateSize, m_iBottomCuttingEdge - updateSize, updateSize<<1, updateSize<<1);
    UpdateWindow();
}

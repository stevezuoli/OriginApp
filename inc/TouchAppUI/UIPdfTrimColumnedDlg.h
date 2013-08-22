#ifndef UIPDFTRIMCOLUMNED_H
#define UIPDFTRIMCOLUMNED_H

#include "GUI/UIDialog.h"
#include "GUI/UITouchComplexButton.h"
#include "BookReader/IBookReader.h"



#define	MovingDirInvalid	(0x00)
#define	MovingDirHorizontal	(0x01)
#define	MovingDirVertical	(0x10)

class UIPdfTrimColumnedDlg : public UIDialog {

public:
	virtual LPCSTR GetClassName() const
	{
		return "UIPdfTrimColumnedDlg";
	}
	
	enum PdfTrimColumnedAction {
		PdfTrimAction,
		PdfColumnedComicOrderLR,
		PdfColumnedComicOrderRL,
		PdfColumnedOrderAuto,
		PdfColumnedOrder1,
		PdfColumnedOrder2,
		PdfColumnedOrder3,
		PdfColumnedOrder4
	};

	UIPdfTrimColumnedDlg(UIContainer* pParent = NULL, IBookReader* pBookReader = NULL, PdfTrimColumnedAction iPdfTrimColumnedAction = PdfTrimAction);
	virtual ~UIPdfTrimColumnedDlg();

	virtual HRESULT Draw(DK_IMAGE drawingImg);
	virtual void OnCommand(DWORD dwCmdId, UIWindow * pSender, DWORD dwParam);
	virtual bool OnTouchEvent(MoveEvent* moveEvent);

public:
	void GetTrimColumnedChangedArgs(PdfModeController& pdfModeController);
	void GetCuttingEdgeArgs(DK_DOUBLE& top, DK_DOUBLE& left, DK_DOUBLE& bottom, DK_DOUBLE& right);
	void GetColumnedArgs(ReadingMode& m_iReadingMode, DK_UINT& subPageOrder, DK_DOUBLE& width, DK_DOUBLE& height);
	void SetAction(PdfTrimColumnedAction action);

protected:
	virtual HRESULT DrawBackGround(DK_IMAGE drawingImg);

private:
	void MaybeSaveChanges(bool r);

private:
	IBookReader* m_pBookReader;
	PdfTrimColumnedAction m_iCurAction;

	//record the position of the cutting edge guide-line.
	enum MovingCorner {
		MovingTopLeft,
		MovingBottomRight,
		MovingCornerInvalid
	};

	MovingCorner m_iCurMovingCorner;
	int m_iTopCuttingEdge;
	int m_iLeftCuttingEdge;
	int m_iRightCuttingEdge;
	int m_iBottomCuttingEdge;

	//record the position of the columned guide-line.
	//if split-2-pages, only m_iColumnedX is valid.
	int m_iCurMovingDir;
	int m_iSplitPagesCounter;
	int m_iColumnedX;
	int m_iColumnedY;

	bool m_bIsActionFinished;

	UITouchComplexButton m_btnCancel;
	UITouchComplexButton m_btnConfirmed;
	UITouchComplexButton m_btnTips;
    MovingCorner HitTest(int x, int y);
    void UpdateMovingCorners();

};//UIPdfTrimColumnedDlg

#endif//UIPDFTRIMCOLUMNED_H

#ifndef LAYOUTREADR_H
#define LAYOUTREADR_H

#include "TouchAppUI/UIReaderSettingDlg.h"
///////////////////  layout reader  /////////////////////////
class TextBookReader;
class UIReaderSettingDlg;
class LayoutReader {

public:
	LayoutReader(UIReaderSettingDlg* pReaderSettingDlg);
	~LayoutReader();
	DK_IMAGE* GetDemoImage();
	static const float* GetEmboldenTable() { return s_emboldenTable; }
	static const float s_emboldenTable[];
private:
	void InitReader();
	void ClearReader();
	bool IsInstantTurnPageModel();

private:
	TextBookReader* m_pDemoReader;//reader used to open demo*
	UIReaderSettingDlg* m_pReaderSettingdlg;
};//LayoutReader
///////////////////  layout reader  /////////////////////////


#endif//LAYOUTREADR_H

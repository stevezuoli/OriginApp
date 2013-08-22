///////////////////////////////////////////////////////////////////////
// UIDictionarySetDialog.cpp
// Contact: wang mingyin
// Copyright (c) Duokan Corporation. All rights reserved.
//
////////////////////////////////////////////////////////////////////////

#include "TouchAppUI/UIDictionaryDialog.h"
#include "TouchAppUI/UIDictionarySetDialog.h"
#include "I18n/StringManager.h"
#include "GUI/CTpGraphics.h"
#include "Utility/Dictionary.h"
#include "Framework/CDisplay.h"


#include <iostream>
using namespace std;
#include<fstream>

#define MAX_DICT_COUNT        15

#define DICTIOANARY_NAME_ID  0x100
#define  DICTDEFAULTPATH     "/mnt/us/DK_System/xKindle/res/dict"


UIDictionarySetDialog::UIDictionarySetDialog(UIContainer* pParent,const char* pWord)
	: UIDialog(pParent)
	,m_pClearMsgBox(NULL)
	, m_iIndex(-1)
	, m_iDictCount(0)
{
	memset(m_szWord, 0, MAX_WORD_SIZE);
	if(pWord!=NULL)
	{
		strncpy(m_szWord,pWord,MAX_WORD_SIZE-1);
	}
	else
	{
		strcpy(m_szWord,"");
	}
	
	Init();
}
UIDictionarySetDialog::~UIDictionarySetDialog()
{
	if(NULL!=m_pClearMsgBox)
	{
		delete m_pClearMsgBox;
		m_pClearMsgBox=NULL;
	}
	WriteToFile();
	ClearVectorItem();
}
void UIDictionarySetDialog::OnCommand(DWORD dwCmdId, UIWindow * pSender, DWORD dwParam)
{   
	switch(dwCmdId)
	{
		case ID_BTN_BACK_TO_DICT:
			{
				this->EndDialog(1);
			}
			break;
		case ID_BTN_MOVE_UP:
			{ 
				if(m_iIndex>0 && m_iIndex<(int)vec_uibtn.size())
				{
					ChangeBtnName(m_iIndex-1,m_iIndex);
					vec_uibtn[m_iIndex-1]->SetFocus(true);
				}
			}
			break;
		case ID_BTN_MOVE_DOWN:
			{
				if(m_iIndex>=0 && m_iIndex<(int)vec_uibtn.size()-1)
				{
					ChangeBtnName(m_iIndex,m_iIndex+1);
					vec_uibtn[m_iIndex+1]->SetFocus(true);
				}
			}
			break;
		case ID_BTN_DELETE:
			{
		  int btnSize = (int)vec_uibtn.size();
				if(m_iIndex>-1 && !vec_uibtn.empty() && m_iIndex < btnSize)
				{
					if(NULL!=m_pClearMsgBox)
					{
						delete m_pClearMsgBox;
						m_pClearMsgBox=NULL;
					}
					m_pClearMsgBox=new UIMessageBox(this, StringManager::GetStringById(SURE_DELETE), MB_OK | MB_CANCEL);
					if(NULL!=m_pClearMsgBox)
                    {
                        char szDicName[128]={0};
                        snprintf(szDicName,sizeof(szDicName),"%s  %s",StringManager::GetPCSTRById(SURE_DELETE),vec_uibtn[m_iIndex]->GetText());
                        m_pClearMsgBox->SetText(szDicName);          
                        if(MB_OK==m_pClearMsgBox->DoModal())
                        {
                            DeleteCorrespondingIndex(m_iIndex);
                            //先将后面的移动到前面，然后移除最后面一个
                            for(int i=m_iIndex; i<btnSize-1; i++)
                            {
                                ChangeBtnName(i,i+1);
                            }

                            RemoveChild(vec_uibtn[btnSize-1], true);
                            vec_uibtn[btnSize - 1] = NULL;
                            vec_uibtn.pop_back();
                        }
                    }
				}
			}
			break;
		default:
			break;
	}

	Repaint();
	
}
BOOL UIDictionarySetDialog::OnKeyPress(INT32 iKeyCode)
{       
	BOOL ret=UIDialog::OnKeyPress(iKeyCode);
	if(!ret)
	{
		HandleKeyPress();
		Repaint();
	}
	return ret;
}

char* UIDictionarySetDialog::GetWords()
{
	return m_szWord;
}

void UIDictionarySetDialog::Init()
{
	m_btnBack.Initialize(ID_BTN_BACK_TO_DICT, StringManager::GetStringById(BACK_TO_DICT), 18, ImageManager::GetImage(IMAGE_BUTTON_HEAD_24));
	m_btnBack.MoveWindow(0,700,200,30);
	AppendChild(&m_btnBack);
	
	m_btnUp.Initialize(ID_BTN_MOVE_UP, StringManager::GetStringById(DICT_NAME_UP), 18, ImageManager::GetImage(IMAGE_BUTTON_HEAD_24));
	m_btnUp.MoveWindow(220,700,100,30);
	AppendChild(&m_btnUp);
	
	m_btnDown.Initialize(ID_BTN_MOVE_DOWN, StringManager::GetStringById(DICT_NAME_DOWN), 18, ImageManager::GetImage(IMAGE_BUTTON_HEAD_24));
	m_btnDown.MoveWindow(340,700,100,30);
	AppendChild(&m_btnDown);

	m_btnDelete.Initialize(ID_BTN_DELETE, StringManager::GetStringById(BOOK_DELETE), 18, ImageManager::GetImage(IMAGE_BUTTON_HEAD_24));
	m_btnDelete.MoveWindow(460,700,100,30);
	AppendChild(&m_btnDelete);

	
	m_vecDictInfo=Dictionary::GetDictInfo();
	m_iDictCount=Dictionary::GetDictCount();
	AddVectorItem(0,m_iDictCount);
	
	HandleKeyPress();          //初始化状态
}

HRESULT UIDictionarySetDialog::DrawBackGround(DK_IMAGE drawingImg)
{
	
	HRESULT hr(S_OK);
	DK_IMAGE imgSelf;
	DK_RECT rcSelf={0, 0, m_iWidth, m_iHeight};

	RTN_HR_IF_FAILED(CropImage(
		drawingImg,
		rcSelf,
		&imgSelf
		));
	CTpGraphics grp(imgSelf);

	SPtr<ITpImage> spImg = ImageManager::GetImage (IMAGE_MENU_BACK); 
	if (spImg)
	{
		RTN_HR_IF_FAILED(grp.DrawImageStretchBlend (spImg.Get (), 0, 0, m_iWidth, m_iHeight));
		RTN_HR_IF_FAILED(grp.DrawLine(0,685,580,3, SOLID_STROKE));
	}

	return S_OK;
}

void UIDictionarySetDialog::HandleKeyPress()
{
	GetFocusBn(m_iIndex);
	int iSize=vec_uibtn.size();
	if(iSize==1)
	{
		m_btnDown.SetEnable(false);
		m_btnDown.SetFontColor(ColorManager::GetColor(COLOR_MENUITEM_INACTIVE));
		m_btnUp.SetEnable(false);
		m_btnUp.SetFontColor(ColorManager::GetColor(COLOR_MENUITEM_INACTIVE));
	}
	else if(iSize>1)
	{
		if(m_iIndex==0)
		{
			m_btnUp.SetEnable(false);
			m_btnUp.SetFontColor(ColorManager::GetColor(COLOR_MENUITEM_INACTIVE));
			m_btnDown.SetEnable(true);
			m_btnDown.SetFontColor(ColorManager::GetColor(ColorManager::knBlack));
		}
		else if(m_iIndex==(int)(vec_uibtn.size()-1))
		{
			m_btnDown.SetEnable(false);
			m_btnDown.SetFontColor(ColorManager::GetColor(COLOR_MENUITEM_INACTIVE));
			m_btnUp.SetEnable(true);
			m_btnUp.SetFontColor(ColorManager::GetColor(ColorManager::knBlack));
		}
		else
		{
			m_btnDown.SetEnable(true);
			m_btnDown.SetFontColor(ColorManager::GetColor(ColorManager::knBlack));
			m_btnUp.SetEnable(true);
			m_btnUp.SetFontColor(ColorManager::GetColor(ColorManager::knBlack));
		}
	}
	else          //iSize=0
	{
		m_btnDown.SetEnable(false);
		m_btnDown.SetFontColor(ColorManager::GetColor(COLOR_MENUITEM_INACTIVE));
		m_btnUp.SetEnable(false);
		m_btnUp.SetFontColor(ColorManager::GetColor(COLOR_MENUITEM_INACTIVE));
		m_btnDelete.SetEnable(false);
		m_btnDelete.SetFontColor(ColorManager::GetColor(COLOR_MENUITEM_INACTIVE));
	}
	
}

UIComplexButton* UIDictionarySetDialog::GetFocusBn(int& index)
{
	UIComplexButton* pBn = NULL;
	int iSize = vec_uibtn.size();
	index=-1;
	for(int i = 0; i< iSize; i++)
	{
		pBn = vec_uibtn[i];
		index++;
		if(pBn && pBn->IsFocus())
		{
			break;
		}
	}
	return pBn;
}



void UIDictionarySetDialog::ChangeBtnName(int first,int second)
{
	if(0<=first && first<(int)vec_uibtn.size() && 0<=second && second<(int)vec_uibtn.size())
	{
		char ori[128]={0};
		int   lineOri=vec_uibtn[first]->GetUnderLineWidth();
		strncpy(ori,vec_uibtn[first]->GetText(),sizeof(ori)-1);
		char after[128]={0}; 
		int   lineAfter=vec_uibtn[second]->GetUnderLineWidth();
		strncpy(after,vec_uibtn[second]->GetText(),sizeof(after)-1);
		vec_uibtn[first]->SetText(after);
		vec_uibtn[first]->SetUnderLineWidth(lineAfter);
		vec_uibtn[second]->SetText(ori);     
		vec_uibtn[second]->SetUnderLineWidth(lineOri);
	}
	   
}

void UIDictionarySetDialog::DeleteCorrespondingIndex(int index)
{

	 if(0<=index && index<(int)vec_uibtn.size() )
	{
		char szDictName[128]={0};
		char szBtnName[128]={0};
		strncpy(szBtnName,vec_uibtn[index]->GetText(),sizeof(szBtnName)-1);
		string strDelName=AddOrDeleteTitle(false,szBtnName);
		strncpy(szDictName,strDelName.c_str(),sizeof(szDictName)-1);
		for(int i=0; i<(int)vec_uibtn.size(); i++)
		{
			if(szDictName==m_vecDictInfo[i].dictname)
			{
				Dictionary::DeleteDict(i);
				break;
			}
		}
	}
}

void UIDictionarySetDialog::ClearVectorItem()
{
	for(int i=0; i<(int)vec_uibtn.size(); i++)
	{      
		if(vec_uibtn[i]!=NULL)
		{      
			RemoveChild(vec_uibtn[i], true);
			vec_uibtn[i]=NULL;
		}
	}
	vec_uibtn.clear();
}
void UIDictionarySetDialog::AddVectorItem(int iStartNo,int iEndNo)
{
	for(int i=iStartNo; i<iEndNo; i++)
	{
		if(i>MAX_DICT_COUNT)
			break;
		UIComplexButton* pUIComplexBtn=new UIComplexButton();
		if(pUIComplexBtn)
		{
			char szDictName[128]={0};       //要保存空间
			string strDictName=AddOrDeleteTitle(true,m_vecDictInfo[i].dictname.c_str());
			strncpy(szDictName,strDictName.c_str(),sizeof(szDictName));
			pUIComplexBtn->Initialize(DICTIOANARY_NAME_ID+i, szDictName, 30, ImageManager::GetImage(IMAGE_BUTTON_HEAD_24));
			pUIComplexBtn->ShowIcon(false);
			pUIComplexBtn->MoveWindow(20, 20+40*i, 500, 30);
			pUIComplexBtn->SetAlign(ALIGN_LEFT);
			AppendChild(pUIComplexBtn);
			vec_uibtn.push_back(pUIComplexBtn);
			
		}
	}
	if(vec_uibtn.size()>0)
	{
		vec_uibtn[0]->SetFocus(TRUE);
	}
	
}

void UIDictionarySetDialog::OnDialogUnLoad(void *pSender)
{
	if(NULL!=m_pClearMsgBox && !m_pClearMsgBox->IsEndDialog())
	{
		m_pClearMsgBox->EndDialog(0);
	}
}
void UIDictionarySetDialog::OnDialogLoad(void *pSender)
{
}

void UIDictionarySetDialog::OnLoad()
{
	ClearVectorItem();
	
	m_vecDictInfo=Dictionary::GetDictInfo();
	m_iDictCount=Dictionary::GetDictCount();
	AddVectorItem(0,m_iDictCount);
	Repaint();
}

INT32 UIDictionarySetDialog::DoModal()
{
	return UIDialog::DoModal();
}
void UIDictionarySetDialog::EndDialog(INT32 iEndCode,BOOL fRepaintIt)
{
	return UIDialog::EndDialog(iEndCode,fRepaintIt);
}

void UIDictionarySetDialog::WriteToFile()
{
 //在退出时将顺序写入文件，用来保存
	char szFilename[128]={0};
	strcpy(szFilename,DICTDEFAULTPATH);
	strcat(szFilename, "/dict.dat");
	ofstream fout(szFilename);
	if(!fout)
	{
		cerr << "open file error" << endl;
		return ;
	}
	for(int i=0; i<(int)vec_uibtn.size(); i++)
	{
		char szDictName[128]={0};
		char szBtnName[128]={0};
		strncpy(szBtnName,vec_uibtn[i]->GetText(),127);
		string strDictName=AddOrDeleteTitle(false,szBtnName);
		strncpy(szDictName,strDictName.c_str(),sizeof(szDictName)-1);
		 for(int j=0; j<m_iDictCount; j++)
		{
			if(szDictName==m_vecDictInfo[j].dictname)
			{
				fout << m_vecDictInfo[j].filename << endl;
				break;
			}
		}
	}
	fout.close();
}

//为真添加书名号，否则去掉书名号
string UIDictionarySetDialog::AddOrDeleteTitle(bool isAdd,const char* pOriName)
{
	if(pOriName==NULL)
	{
		return "";
	}
	int length=strlen(pOriName);
	string nowName=pOriName;
	if(length>0)
	{
		if(isAdd)
		{
			nowName=StringManager::GetPCSTRById(LEFT_TITLE_NUMBER)+nowName+StringManager::GetPCSTRById(RIGHT_TITLE_NUMBER);
		}
		else
		{
			nowName=nowName.substr(3,length-6);
		}
	}
	return nowName;
}


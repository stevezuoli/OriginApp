#ifndef __TOUCHAPPUI_SELECTEDCONTROLER_H__
#define __TOUCHAPPUI_SELECTEDCONTROLER_H__

#include "BookReader/IDKPageAdapter.h"
#include "dkBaseType.h"
#include "CommonUI/BookTextController.h"
#include <vector>

class SelectedControler : public BookTextController
{
public:
    SelectedControler();
    virtual ~SelectedControler();
    virtual LPCSTR GetClassName() const 
    {
        return "SelectedControler";
    }

    virtual bool InitMember();

    // 设置开始选中坐标点
    virtual bool SetStartPos(int _x, int _y,int _iDiffDistance, bool _isVerticalForms);

    // 移动到页尾
    virtual bool MoveToEndOfPage();

    // 移动到页首
    virtual bool MoveToBeginOfPage();

    // 选中过程中移动
    virtual bool Move(int _x, int _y, bool rightHandler = true); 
private:
    DK_POS           m_pointPageStart;  //每页的起始点，翻页前对应的是m_pointStart，翻页后对应的是m_pointUpLeft或m_pointDownRight
};
#endif //__TOUCHAPPUI_SELECTEDCONTROLER_H__

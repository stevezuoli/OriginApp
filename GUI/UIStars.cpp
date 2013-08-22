#include "GUI/UIStars.h"
#include "GUI/CTpGraphics.h"
#include "Utility/ImageManager.h"
#include "Utility/ColorManager.h"

const char* UIStars::StarEditChange = "UIStarsEditChangeEvent";

UIStars::UIStars() : UIWindow(NULL, ID_BOOKSTORE_ADDSCORE)
	, mTotalStars(5)
	, mBlackStars(5)
	, mHalfStarFollow(false)
	, mStarMargin(1)
    , mAlign(ALIGN_LEFT)
    , mVAlign(VALIGN_CENTER)
{
	Init();
}

UIStars::UIStars(UIContainer* pParent) : UIWindow(pParent, ID_BOOKSTORE_ADDSCORE)
	, mTotalStars(5)
	, mBlackStars(5)
	, mHalfStarFollow(false)
    , mStarMargin(1)
    , mAlign(ALIGN_LEFT)
    , mVAlign(VALIGN_CENTER)
{
    if (NULL != pParent)
    {
        pParent->AppendChild(this);
    }
	Init();
}

UIStars::UIStars(UIContainer* pParent, double score, double totalStars, bool readOnly) : UIWindow(pParent, ID_BOOKSTORE_ADDSCORE)
	, mTotalStars(totalStars)
	, mBlackStars(5)
	, mHalfStarFollow(false)
    , mStarMargin(1)
    , mAlign(ALIGN_LEFT)
    , mVAlign(VALIGN_CENTER)
{
    if (NULL != pParent)
    {
        pParent->AppendChild(this);
    }
    SetScore(score);
	Init();
}

UIStars::~UIStars()
{
}

void UIStars::Init()
{
    SetClickable(true);
	mImageBlackStar = ImageManager::GetImage(IMAGE_BOOKSTORE_STAR_SOLID);
	mImageHollowStar = ImageManager::GetImage(IMAGE_BOOKSTORE_STAR_HOLLOW);
#ifdef KINDLE_FOR_TOUCH
#else
	mImageRightArrow = ImageManager::GetImage(IMAGE_RIGHT_ARROW);
	mImageLeftArrow = ImageManager::GetImage(IMAGE_LEFT_ARROW);
	m_iLeftMargin = 0;
	m_iTopMargin = 0;
	m_iArrowMargin = 5;
    mStarsModifing = false;
#endif
}

void UIStars::SetTotalStars(int totalStars)
{
	mTotalStars = totalStars;
}

bool UIStars::SetScore(double score)
{
    if (score > 10 || score < 2)
    {
        //传进错误数值时，按5☆处理
        mBlackStars = 0;
        mHalfStarFollow = false;
        return false;
    }

    if (score>=9.5 && score<=10)
    {
        mBlackStars = 5;
        mHalfStarFollow = false;
    }
    else if (score>=8.5 && score<9.5)
    {
        mBlackStars = 4;
        mHalfStarFollow = true;
    }
    else if (score>=7.5 && score<8.5)
    {
        mBlackStars = 4;
        mHalfStarFollow = false;
    }
    else if (score>=6.5 && score<7.5)
    {
        mBlackStars = 3;
        mHalfStarFollow = true;
    }
    else if (score>=5.5 && score<6.5)
    {
        mBlackStars = 3;
        mHalfStarFollow = false;
    }
    else if (score>=4.5 && score<5.5)
    {
        mBlackStars = 2;
        mHalfStarFollow = true;
    }
    else if (score>=3.5 && score<4.5)
    {
        mBlackStars = 2;
        mHalfStarFollow = false;
    }
    else if (score>=2.5 && score<3.5)
    {
        mBlackStars = 1;
        mHalfStarFollow = true;
    }
    else if (score>=2 && score<2.5)
    {
        mBlackStars = 1;
        mHalfStarFollow = false;
    }

    return true;
}

void UIStars::SetBlackStars(double blackStars, bool ignoreHalfStar)
{
	if (blackStars<0 || blackStars>mTotalStars)
		return;

	mBlackStars = (int)(blackStars);
	double temp = blackStars - mBlackStars;
	if (temp == 0)
	{
		mHalfStarFollow = false;
	}
	else if (temp<=0.5 && !ignoreHalfStar)
	{
		mHalfStarFollow = true;
	}
	else
	{
		mBlackStars++;
		mHalfStarFollow = false;
	}
}

double UIStars::GetBlackStars() const
{
	return mBlackStars;
}

HRESULT UIStars::DrawBackGround(DK_IMAGE drawingImg)
{
	CTpGraphics grf(drawingImg);
	return grf.FillRect(0, 0, m_iWidth, m_iHeight, ColorManager::knWhite);
}

void UIStars::SetStarsIcons(SPtr<ITpImage> solidIcon, SPtr<ITpImage> hollowIcon)
{
    mImageBlackStar = solidIcon;
    mImageHollowStar = hollowIcon;
}

void UIStars::SetStarsMargin(int margin)
{
    mStarMargin = margin;
}

int UIStars::GetDrawStartLeft() const
{
	int left = GetLeftMargin();
    int minWidth = GetMinWidth();
    switch (mAlign)
    {
    case ALIGN_LEFT:
        break;
    case ALIGN_CENTER:
        if (m_iWidth > minWidth)
        {
            left = ((m_iWidth - minWidth) >> 1);
        }
        break;
    case ALIGN_RIGHT:
        if (m_iWidth > minWidth)
        {
            left += (m_iWidth - minWidth);
        }
        break;
    default:
        break;
    }

    return left;
}

int UIStars::GetDrawStartTop() const
{
    int minHeight = GetMinHeight();
    int top = GetTopMargin();
    switch (mVAlign)
    {
    case VALIGN_TOP:
        break;
    case VALIGN_CENTER:
        if (m_iHeight > minHeight)
        {
            top += ((m_iHeight - minHeight) >> 1);
        }
        break;
    case VALIGN_BOTTOM:
        if (m_iHeight > minHeight)
        {
            top += (m_iHeight - minHeight);
        }
        break;
    }

    return top;
}

HRESULT UIStars::Draw(DK_IMAGE drawingImg)
{
	if (!m_bIsVisible)
	{
		return S_OK;
	}

	if (drawingImg.pbData == NULL)
	{
		return E_FAIL;
	}

	HRESULT hr(S_OK);
	DK_IMAGE imgSelf;
	DK_RECT rcSelf={m_iLeft, m_iTop, m_iLeft + m_iWidth, m_iTop + m_iHeight};
	RTN_HR_IF_FAILED(CropImage(drawingImg, rcSelf, &imgSelf));
    RTN_HR_IF_FAILED(DrawBackGround(imgSelf));
    
	int imageWidth = mImageBlackStar.Get()->GetWidth();
	int imageHeight = mImageBlackStar.Get()->GetHeight();
    int left = GetDrawStartLeft();
    int top = GetDrawStartTop();

	CTpGraphics grf(imgSelf);
	int i = 0;

#ifndef KINDLE_FOR_TOUCH
	int imageArrowWidth = mImageLeftArrow.Get()->GetWidth();
	int imageArrowHeight = mImageLeftArrow.Get()->GetHeight();
	int iArrowTop = m_iTopMargin + ((imageHeight - imageArrowHeight)>>1);
	if (mStarsModifing)
    {
        RTN_HR_IF_FAILED(grf.DrawImage(mImageLeftArrow.Get(), left, iArrowTop));
        left += ( imageArrowWidth + m_iArrowMargin);
    }
#endif

	while(i < mBlackStars)
	{
		RTN_HR_IF_FAILED(grf.DrawImage(mImageBlackStar.Get(), left, top));
		++i;
		left += (mStarMargin + imageWidth);
	}

	if (mHalfStarFollow)
	{
		RTN_HR_IF_FAILED(grf.DrawImage(mImageBlackStar.Get(), left, top, 0, 0, imageWidth>>1, imageHeight));
		RTN_HR_IF_FAILED(grf.DrawImage(mImageHollowStar.Get(), left+(imageWidth>>1), top, imageWidth>>1, 0, imageWidth>>1, imageHeight));
		++i;
		left += (mStarMargin + imageWidth);
	}

	while (i < mTotalStars)
	{
		RTN_HR_IF_FAILED(grf.DrawImage(mImageHollowStar.Get(), left, top));
		i++;
		left += (mStarMargin + imageWidth);
    }

#ifndef KINDLE_FOR_TOUCH
    const int lineWidth = left - storeLeft - mStarMargin;
	if(m_bIsFocus)
	{
		RTN_HR_IF_FAILED(grf.DrawLine(storeLeft, m_iTopMargin + imageHeight + 2, lineWidth, 3, SOLID_STROKE));
	}
	else if (IsEnable())
	{
		RTN_HR_IF_FAILED(grf.DrawLine(storeLeft, m_iTopMargin + imageHeight + 2, lineWidth, 1, DOTTED_STROKE));
	}

	if (mStarsModifing)
	{
		left += (m_iArrowMargin - mStarMargin);
		RTN_HR_IF_FAILED(grf.DrawImage(mImageRightArrow.Get(), left, iArrowTop));
	}
#endif
    return hr;

}

int UIStars::GetMinWidth() const
{
#ifdef KINDLE_FOR_TOUCH
	return (mImageBlackStar.Get()->GetWidth() + mStarMargin)*mTotalStars - mStarMargin + (m_iLeftMargin<<1);
#else
    if (IsEnable())
    {
        return (mImageBlackStar.Get()->GetWidth() + mStarMargin)*mTotalStars - mStarMargin + (m_iLeftMargin<<1)
            + ((mImageRightArrow.Get()->GetWidth() + m_iArrowMargin)<<1);
    }
    else
    {
        return (mImageBlackStar.Get()->GetWidth() + mStarMargin)*mTotalStars - mStarMargin + (m_iLeftMargin<<1);
    }
#endif
}

int UIStars::GetMinHeight() const
{
#ifdef KINDLE_FOR_TOUCH
    return mImageBlackStar.Get()->GetHeight() + (m_iTopMargin<<1);
#else
    if (IsEnable())
    {
        return mImageBlackStar.Get()->GetHeight() + 5 + (m_iTopMargin<<1);//5 for height and space of the focus-line
    }
    else
    {
        return mImageBlackStar.Get()->GetHeight() + (m_iTopMargin<<1);//5 for height and space of the focus-line
    }
#endif
}

void UIStars::MoveWindow(INT32 iLeft, INT32 iTop, INT32 iWidth, INT32 iHeight)
{
	UIWindow::MoveWindow(iLeft, iTop, iWidth, iHeight);
	//int minWidth = GetMinWidth();
	//if (iWidth > minWidth)
	//{
	//	m_iLeftMargin = (iWidth - minWidth)>>1; 
	//}
	//int minHeight = GetMinHeight();
	//if (iHeight > minHeight)
	//{
	//	m_iTopMargin = (iHeight - minHeight)>>1;
	//}
}

#ifdef KINDLE_FOR_TOUCH

void UIStars::PerformClick(MoveEvent* moveEvent)
{
	if (!IsEnable())
	{
		return;
	}
	int relativeX = moveEvent->GetX();
	int imageWidth = mImageBlackStar.Get()->GetWidth();

    relativeX -= GetDrawStartLeft();
	double rate = (double)(relativeX)/(imageWidth+mStarMargin);
	SetBlackStars(rate, true);
    UpdateWindow();
    if (m_pParent)
    {
        m_pParent->OnEvent(UIEvent(COMMAND_EVENT, this, GetId()));
        StarEditChangeEventArgs args;
        FireEvent(StarEditChange, args);
    }

	return;
}
#else

void UIStars::ChangeModifyStatus()
{
	mStarsModifing = !mStarsModifing;
	UpdateWindow();
}

BOOL UIStars::OnKeyPress(int iKeyCode)
{
	DebugPrintf(DLC_ZHY, "enter UIStars::OnKeyPress");

	if (mStarsModifing)
	{
		switch (iKeyCode)
		{
		case KEY_OKAY:
            {
                ChangeModifyStatus();
                if (m_pParent)
                {
                    m_pParent->OnEvent(UIEvent(COMMAND_EVENT, this, GetId()));
                }
                return false;
            }
        case KEY_BACK:
            {
                mBlackStars = 0;
                ChangeModifyStatus();
                return false;
            }
		case KEY_LEFT:
		case KEY_UP:
			ModifyStarsCountByStep(false);
			return false;
		case KEY_RIGHT:
		case KEY_DOWN:
			ModifyStarsCountByStep(true);
			return false;
		default:
			break;
		}
	}
	else
	{
		switch (iKeyCode)
		{
		case KEY_OKAY:
			ChangeModifyStatus();
			return false;
		default:
			break;
		}
	}
	DebugPrintf(DLC_ZHY, "leave UIStars::OnKeyPress");

	return true;
}

void UIStars::ModifyStarsCountByStep(bool increase)
{
	DebugPrintf(DLC_ZHY, "enter UIStars::ModifyStarsCountByStep %d",increase);
	if (!mStarsModifing)
		return;

	if (increase)
	{
		if (mBlackStars == mTotalStars)
			return;

		mBlackStars++;
	}
	else
	{
		if ((mBlackStars == 1) && !mHalfStarFollow)
			return;

		mBlackStars--;
    }
    UpdateWindow();
    StarEditChangeEventArgs args;
    FireEvent(StarEditChange, args);

	DebugPrintf(DLC_ZHY, "leave UIStars::ModifyStarsCountByStep %d",increase);
}
#endif

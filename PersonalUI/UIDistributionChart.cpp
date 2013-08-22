#include "PersonalUI/UIDistributionChart.h"
#include "GUI/CTpGraphics.h"
#include "Common/WindowsMetrics.h"
#include "GUI/UIImage.h"
#include "Utility/ColorManager.h"
#include "I18n/StringManager.h"
#include "GUI/UIImageTransparent.h"

using namespace WindowsMetrics;

UIDistributionChart::UIDistributionChart(UIContainer* pParent)
	: UIContainer(pParent, IDSTATIC)
	, m_descriptionPosition(0)
{
	m_description.SetFontSize(GetWindowFontSize(UIDistributionCharDescriptionIndex));
	m_description.SetEraseBackground(false);
}

UIDistributionChart::~UIDistributionChart()
{
}

void UIDistributionChart::SetDistribution(const std::vector<int>& distribution)
{
	m_ditstribution = distribution;
}

void UIDistributionChart::SetMaxHeight(int maxHeight)
{
	m_maxHeight = maxHeight;
}

void UIDistributionChart::SetDescription(const char* description, int position)
{
	m_description.SetText(description);
	m_descriptionPosition = position;
}

vector<int> UIDistributionChart::ConvertDescriptionToHeight()
{
	vector<int> height;
	if(m_maxHeight > 0)
	{
		int distributionHeight = m_iHeight - 2;
		for(int i = 0; i < (int)m_ditstribution.size(); i++)
		{
			height.push_back((m_ditstribution[i] * distributionHeight) / m_maxHeight);
		}
	}
	return height;
}

vector<PointBezier> UIDistributionChart::GetBezierPoint(const vector<int>& distributionHeight)
{
	int dataNum = distributionHeight.size();
	vector<PointBezier> bezierPoint;
	if(dataNum > 0)
	{
		//所画的最小单位的长度(一共24小时，所以此值应为 m_iWidth/48)
		double unitLength = m_iWidth*1.0 / (dataNum*2);
		int before = distributionHeight.back();

		//24小时是循环的，所以此处取0点和23点的平均值设置首位点
		int startPosition = (before + distributionHeight[0])/2;
		double pointX = 0;
		for(int i = 0; i < (int)distributionHeight.size(); i++)
		{
			bezierPoint.push_back(PointBezier(pointX, (before + distributionHeight[i])/2));
			pointX += unitLength;
			bezierPoint.push_back(PointBezier(pointX, distributionHeight[i]));
			pointX += unitLength;
			before = distributionHeight[i];
		}
		bezierPoint.push_back(PointBezier(pointX, startPosition));
	}
	return bezierPoint;
}

void UIDistributionChart::DrawDescription(DK_IMAGE drawingImg)
{
	if(m_maxHeight > 0 && m_descriptionPosition > 0)
	{
		CTpGraphics grf(drawingImg);
		int everageLineHeight = (m_iHeight - 2) / (m_maxHeight*1.0 / m_descriptionPosition);
		UIImageTransparent imageBlock(this);
		imageBlock.SetImageFile(ImageManager::GetImagePath(IMAGE_ICON_BLOCK));
		int imageHeight = imageBlock.GetImageHeight();
		int imageWidth = imageBlock.GetImageWidth();
		if(everageLineHeight < imageHeight/2)
		{
			everageLineHeight = imageHeight/2;
		}
		int linePositon = m_iHeight - everageLineHeight;
		grf.FillRect(0, linePositon, m_iWidth, linePositon + 2, ColorManager::knBlack);

		imageBlock.MoveWindow(10, linePositon - imageHeight/2, imageWidth, imageHeight);
		imageBlock.Draw(drawingImg);
		int strWidth = m_description.GetTextWidth();
		int strHeight = m_description.GetTextHeight();
		m_description.MoveWindow(10 + (imageWidth - strWidth)/2, linePositon - strHeight/2, strWidth, strHeight);
		m_description.Draw(drawingImg);
	}
}

HRESULT UIDistributionChart::DrawBackGround(DK_IMAGE drawingImg)
{
    HRESULT hr(S_OK);
    CTpGraphics grf(drawingImg);
	RTN_HR_IF_FAILED(grf.EraserBackGround());

	vector<int> distributionHeight = ConvertDescriptionToHeight();
	int dataNum = (int)distributionHeight.size();
    if (dataNum > 0 && m_maxHeight > 0)
    {
		vector<PointBezier> lattice = GetBezierPoint(distributionHeight);
		for(int j = 2; j < (int)lattice.size(); j+=2)
		{
			//绘制bezier曲线
			DarwBezier(lattice[j - 2], lattice[j - 1], lattice[j], grf);
		}
		if(m_descriptionPosition > 0)
		{
			//绘制平均值
			DrawDescription(drawingImg);
		}
    }
	//绘制最下边的线
	grf.FillRect(0, m_iHeight - 2, m_iWidth, m_iHeight, ColorManager::GetColor(COLOR_DISABLECOLOR));
	return hr;
}

void UIDistributionChart::DarwBezier(const PointBezier& point1, const PointBezier& point2, const PointBezier& point3, CTpGraphics& grf)
{
	for(float i = 0; i <= 1; i+=0.01)
	{
		PointBezier point = bezierPercentage(point1, point2, point3, i);
		int pointX = (int)point.x;
		int pointY = (int)point.y;
		grf.FillRect(pointX, m_iHeight - 2 - pointY, pointX + 1, m_iHeight - 2, ColorManager::GetColor(COLOR_DISABLECOLOR));
	}
}

PointBezier UIDistributionChart::bezierPercentage(const PointBezier& point1, const PointBezier& point2, const PointBezier& point3, double percentage)
{
    PointBezier left, right, point;
    left.x = getPercentage(point1.x, point2.x, percentage);
    left.y = getPercentage(point1.y, point2.y, percentage);
    right.x = getPercentage(point2.x, point3.x, percentage);
    right.y = getPercentage(point2.y, point3.y, percentage);
    point.x = getPercentage(left.x, right.x, percentage);
    point.y = getPercentage(left.y, right.y, percentage);
	return point;
}

double UIDistributionChart::getPercentage(double position1, double position2, double percentage)
{
    double difference = position2 - position1;
    return position1 + difference*percentage;
}


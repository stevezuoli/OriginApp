#ifndef _PERSONALUI_UIDISTRIBUTIONCHART_
#define _PERSONALUI_UIDISTRIBUTIONCHART_
#include "GUI/CTpGraphics.h"
#include "GUI/UITextSingleLine.h"

struct PointBezier
{
	PointBezier()
	{
		x = 0;
		y = 0;
	}
	PointBezier(double pointX, double pointY)
	{
		x = pointX;
		y = pointY;
	}
	double x;
	double y;
};

class UIDistributionChart : public UIContainer
{
public:
	UIDistributionChart(UIContainer* pParent);
	~UIDistributionChart();
	virtual LPCSTR GetClassName() const
	{
		return "UIDistributionChart";
	}

	void SetDistribution(const std::vector<int>& distribution);
	void SetMaxHeight(int maxHeight);
	void SetDescription(const char* description, int position);
	
private:
	void DrawDescription(DK_IMAGE drawingImg);
    virtual HRESULT DrawBackGround(DK_IMAGE drawingImg);
	PointBezier bezierPercentage(const PointBezier& point1, const PointBezier& point2, const PointBezier& point3, double percentage);
	void DarwBezier(const PointBezier& point1, const PointBezier& point2, const PointBezier& point3, CTpGraphics& grf);
	double getPercentage(double position1, double position2, double percentage);
	std::vector<PointBezier> GetBezierPoint(const std::vector<int>& distributionHeight);
	std::vector<int> ConvertDescriptionToHeight();

private:
	int m_maxHeight;
	std::vector<int> m_ditstribution;
	int m_descriptionPosition;
	UITextSingleLine m_description;
};
#endif


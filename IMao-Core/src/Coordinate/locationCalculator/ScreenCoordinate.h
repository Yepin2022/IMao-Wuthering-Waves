#pragma once
#include "RelativeCoordinates.h"
#include "Windows.h"
#include<vector>
#include "opencv2/opencv.hpp"
struct RectangularAreaScreenLocation {
	//center
	Coordinate leftPoint;
	Coordinate rightPoint;
	Coordinate topPoint;
	Coordinate bottomPoint;

	RectangularAreaScreenLocation(Coordinate leftPoint, Coordinate rightPoint, Coordinate topPoint, Coordinate bottomPoint)
		:leftPoint(leftPoint),rightPoint(rightPoint),topPoint(topPoint),bottomPoint(bottomPoint){ }
};

class ScreenCoordinate
{
public:
	static Coordinate MinMapCircleCenterScreenCoordinate(const RECT &w_Rect);
	static RectangularAreaScreenLocation MinMapScreenCoordinate(HWND& w_hwnd);
	static RectangularAreaScreenLocation SpecifyScreenCoordinate(const RECT& w_Rect, std::vector<Coordinate> specifyAreaScreenData);
	static RectangularAreaScreenLocation SpecifyScreenCoordinate(const HWND& hwnd, std::vector<Coordinate> specifyAreaScreenData);
	static Coordinate ItemScreenCoordinateOnMinMap(const HWND& hwnd, const Coordinate& itemRC, const Coordinate& playerRC);
	static Coordinate ItemScreenCoordinateOnMinMap(const RECT& rect, const Coordinate& itemRC, const Coordinate& playerRC);
	static Coordinate ItemScreenCoordinateOnMap(const Coordinate& gameMapCenterPointImgMapRC, const Coordinate& itemRC, const std::vector<cv::Point2f>& captureCorners, const RECT& w_rect);
};



#pragma once
#include "vector"
#include "../ImGuiOverWindows.h"
#include "../../Coordinate/CoordinateStruct.h"
#include "../../util.h"
#include "LoadEditRouteData.h"

class DrawRouteOnMap{
public:

	static void GetRoutePointsScreen(const Coordinate& validGameMapcenterPointROC, const std::vector<cv::Point2f>& captureCorners, const RECT& rect, int senceId);
	static void DrawRoute();
	static void ClearRountsData() {
		std::lock_guard<std::mutex> lock(routeMutex);
		routesDatas.clear();
	}
private:
	static std::vector<RouteDatas> routesDatas;
	static std::mutex routeMutex;
};
#pragma once
#include "vector"
#include "../ImGuiOverWindows.h"
#include "../../Coordinate/CoordinateStruct.h"
#include "../../util.h"
#include "LoadEditRouteData.h"


class DrawRouteOnMinMap{
public:
	static void GetRoutePointsScreen(const RECT& rect,const Coordinate& playerROC, float minMapRadius, int senceId);
	static void ClearRountsData() {
		std::lock_guard<std::mutex> lock(routeMutex);
		routesDatas.clear();
	}

	static void DrawRoute(App& app);
private:
	static std::vector<RouteDatas> routesDatas;
	static std::mutex routeMutex;
};
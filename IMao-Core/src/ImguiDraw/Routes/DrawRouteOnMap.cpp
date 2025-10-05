#include "DrawRouteOnMap.h"
#include "../../Coordinate/locationCalculator/ScreenCoordinate.h"

using namespace std;
using namespace cv;
vector<RouteDatas> DrawRouteOnMap::routesDatas;
mutex DrawRouteOnMap::routeMutex;

void DrawRouteOnMap::GetRoutePointsScreen(const Coordinate& validGameMapcenterPointROC, const vector<Point2f>& captureCorners, const RECT& rect, int senceId) {
	DrawRouteOnMap::ClearRountsData();
	lock_guard<mutex> lock(routeMutex);

	for (const auto& routeDatas : LoadEditRouteData::routesDatas) {
		vector<Coordinate> routePointsScreen;
		for (const auto routePointROC : routeDatas.routePointsROC) {
			Coordinate routePointScreen = ScreenCoordinate::ItemScreenCoordinateOnMap(validGameMapcenterPointROC, routePointROC, captureCorners, rect);
			if (routePointScreen.x < rect.right + 50 and routePointScreen.y > -50 and routePointScreen.y < rect.bottom + 50 and routePointScreen.y > -50) {
				routePointsScreen.push_back(routePointScreen);
			}
		}

		if (routePointsScreen.size() >= 2) {
			DrawRouteOnMap::routesDatas.push_back(RouteDatas("name", routeDatas.senceId, vector<Coordinate>(), routePointsScreen));
		}
	}
}

void DrawRouteOnMap::DrawRoute(App& app) {
	lock_guard<mutex> lock(routeMutex);

	if (DrawRouteOnMap::routesDatas.empty())
		return;

	for (const auto& routeDatas : routesDatas) {
		const auto screenPoints = routeDatas.routePointsScreenCoord;

		if (routeDatas.senceId != app.GetPlayerCurrentSceneId()) {
			continue;
		}
			
		auto draw = ImGui::GetBackgroundDrawList();
		for (int i = 0; i < screenPoints.size() - 1; i++) {
			ImVec2 p1 = ImVec2(screenPoints[i].x, screenPoints[i].y);
			ImVec2 p2 = ImVec2(screenPoints[i + 1].x, screenPoints[i + 1].y);
			ImU32 color = IM_COL32(255, 0, 0, 255);

			draw->AddLine(p1, p2, color, 1.5);
		}
	}
}
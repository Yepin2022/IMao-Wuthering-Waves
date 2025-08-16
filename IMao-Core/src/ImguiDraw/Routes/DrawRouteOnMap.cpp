#include "DrawRouteOnMap.h"
#include "../../Coordinate/locationCalculator/ScreenCoordinate.h"
#include "LoadEditRouteData.h"
using namespace std;
using namespace cv;
vector<Coordinate> DrawRouteOnMap::routePointsScreenCoord;
mutex DrawRouteOnMap::routeMutex;

void DrawRouteOnMap::GetRoutePointsScreen(const Coordinate& validGameMapcenterPointROC, const vector<Point2f>& captureCorners, const RECT& rect, int senceId) {
	DrawRouteOnMap::ClearRountsData();

	lock_guard<mutex> lock(routeMutex);
	for (const auto& routeDatas : LoadEditRouteData::routesDatas) {
		for (const auto routePointROC : routeDatas.routePointsROC) {
			Coordinate routePointsScreen = ScreenCoordinate::ItemScreenCoordinateOnMap(validGameMapcenterPointROC, routePointROC, captureCorners, rect);
			if (routePointsScreen.x < rect.right + 50 and routePointsScreen.y > -50 and routePointsScreen.y < rect.bottom + 50 and routePointsScreen.y > -50) {
				DrawRouteOnMap::routePointsScreenCoord.push_back(routePointsScreen);
			}
		}
	}
}

void DrawRouteOnMap::DrawRoute() {
	if (DrawRouteOnMap::routePointsScreenCoord.empty())
		return;

	lock_guard<mutex> lock(routeMutex);

	const auto& screenPoints = routePointsScreenCoord;

	auto draw = ImGui::GetBackgroundDrawList();
	for (int i = 0; i < screenPoints.size()-1; i++) {
		ImVec2 p1 = ImVec2(screenPoints[i].x, screenPoints[i].y);
		ImVec2 p2 = ImVec2(screenPoints[i + 1].x, screenPoints[i + 1].y);
		ImU32 color = IM_COL32(255, 0, 0, 255);

		draw->AddLine(p1, p2, color,1.5);
	}
}
#include "LoadEditRouteData.h"
#include "DrawRouteOnMap.h"
#include "../Items/DrawItemOnGameMap.h"
using namespace std;
using namespace cv;

std::vector<RouteDatas> LoadEditRouteData::routesDatas;
App* LoadEditRouteData::app;
std::thread LoadEditRouteData::Thread;
bool LoadEditRouteData::threadStopFlag;

void LoadEditRouteData::Initi(App* app) {
	if (app != nullptr) {
		LoadEditRouteData::app = app;
		StartThread();
	}
}

void LoadEditRouteData::AddRouteDatas(string name, const Coordinate& ROC_a, const Coordinate& ROC_b) {
	vector<Coordinate> routePointsROC = GenerateEquidistantPoints(ROC_a, ROC_b, 5);
	LoadEditRouteData::routesDatas.push_back(RouteDatas(name, routePointsROC));
	DrawRouteOnMap::ClearRountsData();
}

void LoadEditRouteData::Thread_KeyMonitoring_AddRouteDatas_ByMousePos() {
	const int monitoredKey = 0x51; //Q
	bool keyWasPressed = false;

	int state = 0;//0 准备 1 完成 
	Coordinate a_ROC; Coordinate b_ROC;
	while (!threadStopFlag) {
		if (app == nullptr) return;

		//为空,意味着地图未打开
		if (DrawItemOnGameMap::centerPointNearItemsData.empty()) {
			Sleep(60);
			continue;
		}

		bool keyIsPressed = isKeyPressed(monitoredKey);
		if (keyIsPressed && !keyWasPressed) {
			keyWasPressed = true;
			Coordinate mapCoordinatesOfMousePos = app->GetMapCoordinatesOfMousePos();
			Coordinate ROC = RelativeCoordinates::ImgMapCoordToROC(mapCoordinatesOfMousePos, app->GetPlayerCurrentSceneId());

			if (state == 0) {
				a_ROC = ROC;
				state = 1;
			}
			else {
				b_ROC = ROC;
				AddRouteDatas("name", a_ROC, b_ROC);
				state = 0;
			}
		}
		else if (!keyIsPressed && keyWasPressed) {
			keyWasPressed = false;
		}
		Sleep(60);
	}
}
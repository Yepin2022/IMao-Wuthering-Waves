#include "LoadEditRouteData.h"
#include "DrawRouteOnMap.h"
#include "../Items/DrawItemOnGameMap.h"
#include <filesystem>
#include <shared_mutex>
#include <fstream>
#include "../InteractiveInterface/Notification.h"
using namespace std;
using namespace cv;

std::vector<RouteDatas> LoadEditRouteData::routesDatas;
App* LoadEditRouteData::app;
std::thread LoadEditRouteData::Thread;
bool LoadEditRouteData::threadStopFlag;
string LoadEditRouteData::routeJsonName = "Routes";
string RouteFolderPath;

void LoadEditRouteData::Initi(App* app) {
	if (app != nullptr) {
		LoadEditRouteData::app = app;
		RouteFolderPath = GetCurrentPath() + "\\SavedRoutes";
		try {
			fs::path folderPath(RouteFolderPath);

			if (!fs::exists(folderPath)) {
				fs::create_directories(folderPath);
			}
		}
		catch (const exception& e) {
			cerr << " LoadEditRouteData::Initi:" << e.what() << endl;
		}

		StartThread();
	}
}

void LoadEditRouteData::AddRouteDatas(string name ,int senceId, const Coordinate& ROC_a, const Coordinate& ROC_b) {
	vector<Coordinate> routePointsROC = GenerateEquidistantPoints(ROC_a, ROC_b, 5);
	LoadEditRouteData::routesDatas.push_back(RouteDatas(name, senceId, routePointsROC));
	DrawRouteOnMap::ClearRountsData();
}

void LoadEditRouteData::SetRouteJsonName(const string& setName) {
	routeJsonName = setName;
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
			int senceId = app->GetPlayerCurrentSceneId();
			Coordinate ROC = RelativeCoordinates::ImgMapCoordToROC(mapCoordinatesOfMousePos, senceId);

			if (state == 0) {
				a_ROC = ROC;
				state = 1;
			}
			else {
				b_ROC = ROC;
				AddRouteDatas("name", senceId, a_ROC, b_ROC);
				WriteRoutesDatas(routeJsonName, Scene::SceneIdToName(senceId), a_ROC, b_ROC);
				state = 0;
			}
		}
		else if (!keyIsPressed && keyWasPressed) {
			keyWasPressed = false;
		}
		Sleep(60);
	}
}

vector<json> LoadEditRouteData::ReadRoutesJson() {
	json j;
	vector<json> routesJson;
	try {
		std::unordered_set<std::string> targetExts = { ".json" };
		vector<fs::path> jsonRoutesPath = findFilesByExtensions(RouteFolderPath, targetExts);

		if (jsonRoutesPath.empty()) {
			return routesJson;
		}

		for (const auto& jsonRoutePath : jsonRoutesPath) {
			ifstream jsonRouteFile(jsonRoutePath);
			jsonRouteFile >> j;
			routesJson.push_back(j);
			jsonRouteFile.close();
		}

		return routesJson;
	}
	catch (const exception& ex) {
		cerr << "ReadRoutesJson:" << ex.what() << endl;
	}
}

void LoadEditRouteData::LoadRoutesDatasFromLocal() {
	vector<json> RoutesJson = ReadRoutesJson();
	for (const auto& RouteJson : RoutesJson) {

		for (const auto& [scene, routesData] : RouteJson.items()) {
			for (size_t i = 0; i < routesData.size(); ++i) {

				Coordinate temp_routeFirstPoint = { 0,0 };
				for (const auto& routePoints : routesData[i]) {
					double x = routePoints[0];
					double y = routePoints[1];

					if (temp_routeFirstPoint.IsValid()) {
						AddRouteDatas("name", Scene::SceneNameToId(scene.c_str()), temp_routeFirstPoint, Coordinate(x, y));
						break;
					}

					temp_routeFirstPoint = { x,y };
				}
			}
		}
	}
}

void LoadEditRouteData::WriteRoutesDatas(const string& routeFileName,const string& senceName, const Coordinate& ROC_a, const Coordinate& ROC_b) {
	const string jsonRoutePath = RouteFolderPath + "\\" + routeFileName + ".json";

	try {
		ifstream routeJson(jsonRoutePath);
		json j;

		if (routeJson.is_open()) {
			routeJson >> j;
		}

		j[senceName].push_back({ {ROC_a.x,ROC_a.y},{ROC_b.x,ROC_b.y} });

		{
			ofstream out_file(jsonRoutePath);
			out_file << j.dump(4);
		}
	}
	catch (const exception& ex) {
		cerr << "WriteRount:" << ex.what() << endl;
		Notification::AddInfo(NotificationDatas(" LoadEditRouteData::WriteRoutesDatas: " + string(ex.what()), 5));
	}
}
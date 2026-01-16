#pragma once
#include "vector"
#include "../../Coordinate/CoordinateStruct.h"
#include "../../util.h"
#include "../../App/App.h"
#include <string>
#include <thread>
#include <nlohmann/json.hpp>

using json = nlohmann::json;

struct RouteDatas
{
	std::string name;
	int senceId;
	std::vector<Coordinate> routePointsROC;
	std::vector<Coordinate> routePointsScreenCoord;
	RouteDatas(std::string name,int senceId, std::vector<Coordinate> routePointsROC = std::vector<Coordinate>(), std::vector<Coordinate> routePointsScreenCoord = std::vector<Coordinate>()) : name(name), senceId(senceId), routePointsROC(routePointsROC), routePointsScreenCoord(routePointsScreenCoord) {};
};



class LoadEditRouteData {
public:
	static std::vector<RouteDatas> routesDatas;
	static void Initi(App* app);
	static void AddRouteDatas(std::string name, int senceId, const Coordinate& ROC_a, const Coordinate& ROC_b);
	static void SetRouteJsonName(const std::string& setName);
	static void Thread_KeyMonitoring_AddRouteDatas_ByMousePos();
	static std::vector<json> ReadRoutesJson();
	static std::vector<json> ReadRoutesJson(std::string routeName);
	static void LoadRoutesDatasFromLocal(bool isLoadAll, std::string routeName);
	static void WriteRoutesDatas(const std::string& routeFileName,const std::string& senceName, const Coordinate& ROC_a, const Coordinate& ROC_b);

	static void StopThread() {
		threadStopFlag = true;
		Thread.join();
	}

	static void ClearRoutesDatas() {
		routesDatas.clear();
	}
private:
	static void StartThread() {
		threadStopFlag = false;
		Thread = std::thread(&LoadEditRouteData::Thread_KeyMonitoring_AddRouteDatas_ByMousePos);
	}

	static App* app;
	static std::thread Thread;
	static bool threadStopFlag;
	static std::string routeJsonName;
};
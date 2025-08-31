#pragma once
#include "vector"
#include "../../Coordinate/CoordinateStruct.h"
#include "../../util.h"
#include "../../App/App.h"
#include <string>
#include <thread>

struct RouteDatas
{
	std::string name;
	std::vector<Coordinate> routePointsROC;
	std::vector<Coordinate> routePointsScreenCoord;
	RouteDatas(std::string name, std::vector<Coordinate> routePointsROC = std::vector<Coordinate>(), std::vector<Coordinate> routePointsScreenCoord = std::vector<Coordinate>()) : name(name), routePointsROC(routePointsROC), routePointsScreenCoord(routePointsScreenCoord){};
};



class LoadEditRouteData {
public:
	static std::vector<RouteDatas> routesDatas;
	static void Initi(App* app);
	static void AddRouteDatas(std::string name, const Coordinate& ROC_a, const Coordinate& ROC_b);
	static void Thread_KeyMonitoring_AddRouteDatas_ByMousePos();

	static void StopThread() {
		threadStopFlag = true;
		Thread.join();
	}
private:
	static void StartThread() {
		threadStopFlag = false;
		Thread = std::thread(&LoadEditRouteData::Thread_KeyMonitoring_AddRouteDatas_ByMousePos);
	}

	static App* app;
	static std::thread Thread;
	static bool threadStopFlag;
};
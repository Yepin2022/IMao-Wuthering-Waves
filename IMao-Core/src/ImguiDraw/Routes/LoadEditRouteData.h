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
	RouteDatas(std::string name, std::vector<Coordinate> routePointsROC) :name(name), routePointsROC(routePointsROC) {};
};

class LoadEditRouteData {
public:
	static std::vector<RouteDatas> routesDatas;
	static void Initi(App* app);
	static void AddRouteDatas(std::string name, const Coordinate& ROC_a, const Coordinate& ROC_b);
	static void Thread_KeyMonitoring_AddRouteDatas_ByMousePos();
private:
	static void StartThread() {
		threadStopFlag = false;
		Thread = std::thread(&LoadEditRouteData::Thread_KeyMonitoring_AddRouteDatas_ByMousePos);
	}

	static void StopThread() {
		threadStopFlag = true;
		Thread.join();
	}

	static App* app;
	static std::thread Thread;
	static bool threadStopFlag;
};
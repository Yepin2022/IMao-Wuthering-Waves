#pragma once
#include "..\ImageProcessing\ImageProcessing.h"
#include "..\Feature\Match\FeatureMatch.h"
#include "..\WindowsCapture\WindowsGraphicsCapture\CaptureSnapshot.h"
#include "..\util.h"
#include "..\Coordinate\CoordinateStruct.h"
#include "..\Coordinate\locationCalculator\MapCoordinate.h"
#include "..\WindowsCapture\BitBltCapture\BitBltCapture.h"
#include "..\Coordinate\IdentifyWorldCoordinates\IdentifyWorldCoordinates.h"


class App
{
public:

	App(std::optional<CaptureSnapshot> graphicsCapture, std::optional<BitBltCapture> bitBltCapture,HWND& hwnd) : graphicsCapture(graphicsCapture), bitBltCapture(bitBltCapture),hwnd(hwnd){}

	void StartTasks() {
		allThreadStopFlag = false;
		Init();
		mouseMonitoringThread = std::thread(&App::Thread_GetItemMapScreenCoordinateByMouseMonitoring, this);
		detectGameStateThread = std::thread(&App::Thread_DetectGameState, this);
		keyMonitoringThread = std::thread(&App::Thread_KeyMonitoring_SavePlayerNearItemPoint, this);

		mainThread = std::thread([this]() {
			winrt::init_apartment();
			this->Start().get();
		});
	}

	void StopTasks() {
		allThreadStopFlag = true;

		mainThread.join();
		mouseMonitoringThread.join();
		detectGameStateThread.join();

		FeatureData_map.Release();
		FeatureData_IconTask.Release();
		FeatureData_wavePlateCrystal.Release();
		std::vector<cv::KeyPoint>().swap(nearPlayerMapKeypoints);
		nearPlayerMapDescriptors.release();
	}

	Coordinate GetlastPlayerCoordinate() {
		return lastPlayerImgMapCoordinate;
	}

	Coordinate GetCenterPointCoordinate() {
		return gameMapCenterPointImgMapCoord;
	}

	int GetGoodMatchSize_IconTask() {
		return GoodMatchSize_IconTask;
	}

	int GetGoodMatchSize_IconWavePlateCrystal() {
		return GoodMatchSize_IconWavePlateCrystal;
	}

	Coordinate GetMapCenterCoordinateByMouseMonitoring() {
		return gameMapCenterCoordinateByMouseMonitoring;
	}

	Coordinate GetMapCoordinatesOfMousePos() {
		return gameMapCoordinatesOfMousePos;
	}

	std::vector<cv::Point2f> GetCaptrueCorners() {
		return captrueCorners;
	}

	void SetMapCenterCoordinateByMouseMonitoring(Coordinate coordinate) {
		gameMapCenterCoordinateByMouseMonitoring = coordinate;
	}

	void SetScaleFactor(float setValue) {
		scaleFactor = setValue;
	}

	void SetInertiaStep(float setValue) {
		inertiaStep = setValue;
	}

	static void SetUpdateMapDataCycleTime(int setValue) {
		updateMapDataCycleTime = setValue;
	}

	static void SetUpdateMinMapDataCycleTime(int setValue) {
		updateMinMapDataCycleTime = setValue;
	}

	static void SetEnabledMapShowItem(bool setValue) {
		enabledMapShowItem = setValue;
	}

	static void SetEnabledMinMapShowItem(bool setValue) {
		enabledMinMapShowItem = setValue;
	}

private:
	bool allThreadStopFlag;
	std::thread mainThread;

	static int updateMapDataCycleTime;
	static int updateMinMapDataCycleTime;
	static bool enabledMapShowItem;
	static bool enabledMinMapShowItem;

	Coordinate identifyCoordinate = { 0,0 };
	int PlayerCurrentSceneId = 0;

	Mat gameSnapshot;

	ImageFeatureData FeatureData_map;
	ImageFeatureData FeatureData_IconTask;
	int GoodMatchSize_IconTask = 0;
	ImageFeatureData FeatureData_wavePlateCrystal;
	int GoodMatchSize_IconWavePlateCrystal = 0;
	std::thread detectGameStateThread;

	std::vector<cv::KeyPoint> nearPlayerMapKeypoints;
	cv::Mat nearPlayerMapDescriptors;
	Coordinate lastPlayerImgMapCoordinate;
	Coordinate gameMapCenterPointImgMapCoord;
	bool existMapCenterPointCoordinate;
	int map_ConsecutiveFailuresCount = 0;

	std::optional<CaptureSnapshot> graphicsCapture;
	std::optional<BitBltCapture> bitBltCapture;

	HWND hwnd;
	RECT rect = {0,0,0,0};

	bool isWindowFocused = false;
	bool isOpenMap = false;
	bool isExistMinMap = false;

	std::thread mouseMonitoringThread;
	Coordinate gameMapCenterCoordinateByMouseMonitoring;
	Coordinate gameMapCoordinatesOfMousePos;
	float inertiaStep = 1;
	float scaleFactor = 1;
	std::vector<cv::Point2f> captrueCorners{ cv::Point2f(0,0),cv::Point2f(0,0),cv::Point2f(0,0) ,cv::Point2f(0,0) };
	bool mapNotMoving;

	std::thread keyMonitoringThread;

	winrt::IAsyncAction GetMatSnapshot(bool isTaketAsync, cv::Mat& result);
	winrt::IAsyncAction Start();
	void Thread_DetectGameState();
	bool Init();
	winrt::IAsyncAction UpdateItemMinMapScreenCoordinate(const cv::Mat& snapshot);
	bool IsOpenMap(const cv::Mat& snapshot, int* goodMatchSize);
	int  GetCurrentSceneId(const Coordinate& identifyCoordinate, const Mat& minMapImg);
	bool IsExistMinMap(cv::Mat& snapshot, int* goodMatchSize);
	void UpdateItemMapScreenCoordinateByMatch(const cv::Mat& snapshot);
	void Thread_GetItemMapScreenCoordinateByMouseMonitoring();
	void Thread_KeyMonitoring_SavePlayerNearItemPoint();
};


#include "App.h"
#include "..\Coordinate\locationCalculator\RelativeCoordinates.h"
#include "..\Coordinate\locationCalculator\ScreenCoordinate.h"
#include "..\ImguiDraw\Items\DrawItemOnMinMap.h"
#include "..\ImguiDraw\Items\DrawItemOnGameMap.h"
#include "..\Feature\Processing\FeatureProcessing.h"
#include "..\WindowsCapture\WindowsGraphicsCapture\SimpleCapture.h"
#include "..\ImguiDraw\InteractiveInterface\Notification.h"
#include "../ImguiDraw/Routes/DrawRouteOnMap.h"
#include "../ImguiDraw/Routes/DrawRouteOnMinMap.h"

using namespace std;
using namespace cv;

int	App::updateMapDataCycleTime = 80;
int App::updateMinMapDataCycleTime = 80;
bool App::enabledMapShowItem;
bool App::enabledMinMapShowItem;
Coordinate validGameMapcenterPointROC;

bool App::Init() {
	GetClientRect(hwnd, &rect);
	imguiWindowsHeight = rect.bottom * 0.15;
	imguiWindowsWidth = rect.right * 0.3;

	Notification::AddInfo(NotificationDatas("这是一款免费使用的软件，如果你是付钱买来的，你已经被骗了。", 20));
	Notification::AddInfo(NotificationDatas("The resource is loading, please be patient.", 15));

	const string path = GetCurrentPath() + "\\Assets\\FeaturesDatas";
	const string mapFeatureFilePath = path+"\\Map_features.yml";
	const string IconTask_FeatureFilePath = path + "\\IconTask_Features.yml";
	const string IconWavePlateCrystal_FeatureFilePath = path +"\\IconWavePlateCrystal_Features.yml";

	if (!FeatureLoader::loadFeaturesFromXML(mapFeatureFilePath, FeatureData_map) or
		!FeatureLoader::loadFeatures(IconTask_FeatureFilePath, FeatureData_IconTask) or
		!FeatureLoader::loadFeatures(IconWavePlateCrystal_FeatureFilePath, FeatureData_wavePlateCrystal)) {
		Notification::AddInfo(NotificationDatas("Resource loading failed.", 30));
		return false;
	}

	if(!IdentifyWorldCoordinates::isLoaded)
		IdentifyWorldCoordinates::Init(GetCurrentPath() + "\\Assets\\models\\PP-OCRv5_mobile_det_infer", GetCurrentPath() + "\\Assets\\models\\PP-OCRv5_mobile_rec_infer",string(), GetCurrentPath() + "\\Assets\\models\\ch_ppocr_mobile_v2.0_cls_infer");

	Mat snapshot;
	GetMatSnapshot(true, snapshot);
	std::this_thread::sleep_for(std::chrono::seconds(2));

	return !snapshot.empty();
}

winrt::IAsyncAction App::Start() {
	co_await winrt::resume_background();
	auto startTime = std::chrono::high_resolution_clock::now();
	int cycleTime = 100;
	while (!allThreadStopFlag) {
		if (!GetClientRect(hwnd, &rect)) {
			cout << "Rect error";
			rect.right = 0;
			co_return;
		}

		co_await GetMatSnapshot(false, gameSnapshot);

		//GameMap
		if (isOpenMap and enabledMapShowItem and !gameSnapshot.empty() and isWindowFocused) {
			Coordinate gameMapCenterPointROC; Coordinate lastGameMapCenterPointROC;

			//在绘制大地图区域 imgui透明窗口大小保持游戏窗口一样
			imguiWindowsHeight = rect.bottom;
			imguiWindowsWidth = rect.right;

			if (GetGameMapCenterPointROC(gameSnapshot, gameMapCenterPointROC, lastGameMapCenterPointROC) and mapNotMoving) {
				if (!IsMapMoving(gameMapCenterPointROC, lastGameMapCenterPointROC)) {
					DrawItemOnGameMap::UpdateCenterPointNearItemsData(validGameMapcenterPointROC,captrueCorners,rect,playerCurrentSceneId);
					DrawRouteOnMap::GetRoutePointsScreen(validGameMapcenterPointROC, captrueCorners, rect, playerCurrentSceneId);
				}
			}

			cycleTime = App::updateMapDataCycleTime;
		}else {
			DrawItemOnGameMap::ClearNearItemsData();
			DrawRouteOnMap::ClearRountsData();
		}


		//MinMap
		if (isExistMinMap and !gameSnapshot.empty()) {
			Coordinate playerROC;
			float minMapRadius;

			imguiWindowsHeight = minMapBottomPoint.y + 10;//在绘制小地图区域 缩写imgui透明窗口范围
			imguiWindowsWidth = rect.right * 0.3;

			if (co_await GetMinMapPlayerROC(gameSnapshot, playerROC, minMapRadius)) {
				if (enabledMinMapShowItem) {
					DrawItemOnMinMap::UpdatePlayerNearItemsData(rect, playerROC, minMapRadius, playerCurrentSceneId);
					DrawRouteOnMinMap::GetRoutePointsScreen(rect, playerROC, minMapRadius, playerCurrentSceneId);
				}
				else {
					DrawItemOnMinMap::ClearNearItemsData();
					DrawRouteOnMinMap::ClearRountsData();
				}
			}

			cycleTime = App::updateMinMapDataCycleTime;
		}else{
			DrawItemOnMinMap::ClearNearItemsData();
			DrawRouteOnMinMap::ClearRountsData();
		}

		auto endTime = std::chrono::high_resolution_clock::now();
		auto elapsedTime = std::chrono::duration_cast<std::chrono::milliseconds>(endTime - startTime).count();
		if (elapsedTime < cycleTime) {
			std::this_thread::sleep_for(std::chrono::milliseconds(cycleTime - elapsedTime));
		}
		startTime = std::chrono::high_resolution_clock::now();
	}
}

winrt::IAsyncAction App::GetMatSnapshot(bool isTaketAsync, Mat& result) {
	Mat temp;
	if (bitBltCapture.has_value()) {
		bitBltCapture->GetSnapshot_PrintWindow(temp);
		temp.copyTo(result);
		co_return;
	}
	else if (graphicsCapture.has_value()) {
		if (isTaketAsync) {
			co_await graphicsCapture->TakeSnapshotAsync();
			temp = graphicsCapture->getTakeSnapshotAsyncResult();
		}
		else {
			graphicsCapture->Getcapture()->GetLatestFrame_Mat(temp);
		}
		NonClientRegion nonClientRegion;
		CalculateNonClientAreaSize(hwnd, nonClientRegion);
		temp = ImageProcessing::CropToWindowsClientArea(temp, nonClientRegion, rect);
		temp.copyTo(result);
	}
}


void App::Thread_DetectGameState() {
	const int cycleTime = 100;
	while (!allThreadStopFlag) {
		auto start = std::chrono::high_resolution_clock::now();
		if (!gameSnapshot.empty()) {;
			bool temp_isExistMinMap = IsExistMinMap(gameSnapshot, &GoodMatchSize_IconTask);
			bool temp_isOpenMap = IsOpenMap(gameSnapshot, &GoodMatchSize_IconWavePlateCrystal);
			if (temp_isExistMinMap && temp_isOpenMap) {
				bool b = GoodMatchSize_IconTask > (GoodMatchSize_IconWavePlateCrystal * 0.375);//3 8
				isExistMinMap = b;
				isOpenMap = !b;
			}
			else {
				isOpenMap = temp_isOpenMap;
				isExistMinMap = temp_isExistMinMap;
			}
		}
		else {
			isOpenMap = isExistMinMap = false;
		}

	    isWindowFocused = IsWindowFocused(hwnd) || IsWindowFocused(ImGuiOverWindows::overWindowsHwnd);
		auto end = std::chrono::high_resolution_clock::now();
		auto elapsed = std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count();
		if (elapsed < cycleTime) {
			std::this_thread::sleep_for(std::chrono::milliseconds(cycleTime - elapsed));
		}
	}
}

bool App::IsExistMinMap(Mat& snapshot,int* goodMatchSize) {
	Mat IconTask = ImageProcessing::CropToRegion_IconTask(snapshot, rect);
	IconTask = ImageProcessing::increaseImageResolution(IconTask, 2.5);//2.5
	Ptr<cv::xfeatures2d::SURF> surt = cv::xfeatures2d::SURF::create(80,6,4,true,true);
	ImageFeatureData test_FeatureData_IconTask = FeatureMatch::ExtractSurfFeatures(surt, IconTask);

	if (!test_FeatureData_IconTask.imgDescriptors.empty()) {
		auto goodMatchs = FeatureMatch::FindGoodMatches(FeatureData_IconTask, test_FeatureData_IconTask, 0.7f, 0.6f, DescriptorMatcher::BRUTEFORCE_SL2);
		*goodMatchSize = goodMatchs.size();
		if (*goodMatchSize >= 4) {
			return true;
		}
	}
	else {
		*goodMatchSize = 0;
	}
	
	return false;
}

bool App::IsOpenMap(const Mat& snapshot, int* goodMatchSize) {
	Mat IconWavePlateCrystal = ImageProcessing::CropToRegion_IconWavePlateCrystal(snapshot, rect);
	IconWavePlateCrystal = ImageProcessing::increaseImageResolution(IconWavePlateCrystal, 2.5f);//2.5
	Ptr<cv::xfeatures2d::SURF> surt = cv::xfeatures2d::SURF::create(80, 6, 4, true, true);
	ImageFeatureData IconWavePlateCrystalFeatureData = FeatureMatch::ExtractSurfFeatures(surt, IconWavePlateCrystal);

	if (!IconWavePlateCrystalFeatureData.imgDescriptors.empty()) {
		auto goodMatchs = FeatureMatch::FindGoodMatches(FeatureData_wavePlateCrystal, IconWavePlateCrystalFeatureData, 0.5f, 0.5f, DescriptorMatcher::BRUTEFORCE_SL2);
		*goodMatchSize = goodMatchs.size();
		if (*goodMatchSize >= 12) {
			return true;
		}
	}
	else {
		*goodMatchSize = 0;
	}
	
	return false;
}

bool App::IsMapMoving(const Coordinate& gameMapcenterPointROC, const Coordinate& lastGameMapCenterPointROC) {
	if (abs(validGameMapcenterPointROC.x - gameMapcenterPointROC.x) > 3 or abs(validGameMapcenterPointROC.y - gameMapcenterPointROC.y) > 3) {
		validGameMapcenterPointROC.x = gameMapcenterPointROC.x; validGameMapcenterPointROC.y = gameMapcenterPointROC.y;

		if (abs(gameMapcenterPointROC.x - lastGameMapCenterPointROC.x) > 15 or abs(gameMapcenterPointROC.y - lastGameMapCenterPointROC.y) > 15) {
			DrawItemOnGameMap::ClearNearItemsData();
			DrawRouteOnMap::ClearRountsData();
		}
		return true;
	}
	return false;
}

int App::GetCurrentSceneId(const Coordinate& identifyCoordinate, const Mat& minMapImg) {
	Ptr<xfeatures2d::SURF> surf = xfeatures2d::SURF::create(10, 8, 4, true, true);
	vector<KeyPoint> playerMapKeypoints;
	Mat playerMapDescriptors;

	for (const auto sceneId : Scene::sceneIds) {
		Coordinate mapCoord = MapCoordinate::IdentifyCoorToImgMapCoord(identifyCoordinate, sceneId);

		FeatureFilter::FilterNearKeypoints(App::FeatureData_map.imgKeypoints, App::FeatureData_map.imgDescriptors, Point2f(mapCoord.x, mapCoord.y), 100, playerMapKeypoints, playerMapDescriptors);

		ImageFeatureData MapFeatureData(playerMapKeypoints, playerMapDescriptors);
		ImageFeatureData minMapFeatureData = FeatureMatch::ExtractSurfFeatures(surf, minMapImg);
		if (minMapFeatureData.imgDescriptors.empty())
			return 0;
		vector<DMatch> goodMatch = FeatureMatch::FindGoodMatchesBetweenMapAndMinMap(minMapFeatureData, MapFeatureData);

		Coordinate PlayerImgMapCoordinate;
		bool isExistGoodCoordinate = MapCoordinate::GetGoodPlayerImgMapCoordinateFromMatches(
			minMapImg,
			goodMatch,
			playerMapKeypoints,
			minMapFeatureData.imgKeypoints,
			8.0f,
			mapCoord,
			PlayerImgMapCoordinate
		);


		if (isExistGoodCoordinate) {
			App::gameMapCenterPointImgMapCoord = lastPlayerImgMapCoordinate = PlayerImgMapCoordinate;
			return sceneId;
		}
	}
	return 0;
}

winrt::IAsyncOperation<bool> App::GetMinMapPlayerROC(const Mat& snapshot,Coordinate& outPlayerROC, float& outMinMapRadius) {
	Ptr<xfeatures2d::SURF> surf = xfeatures2d::SURF::create(60, 8, 4, true, true);
	Mat minMapImg = ImageProcessing::CropToMinMapAreaImg(snapshot, rect, minMapBottomPoint);

	if ((identifyCoordinate.x == 0 && identifyCoordinate.y == 0) || playerCurrentSceneId == 0) {

		Notification::AddInfo(NotificationDatas("Continuity match failed, trying to identify coordinates.", 3));

		Mat snapshot_ocr;
		if (graphicsCapture.has_value()) {
			this_thread::sleep_for(std::chrono::milliseconds(50));
		    co_await GetMatSnapshot(true, snapshot_ocr);
		}
		else {
			snapshot_ocr = snapshot;
		}

		if (!IdentifyWorldCoordinates::IdentifyCoordinateFromSnapshot(snapshot_ocr, identifyCoordinate, rect)) {
			identifyCoordinate = { 0,0 };
			co_return false;
		}

		playerCurrentSceneId = GetCurrentSceneId(identifyCoordinate, minMapImg);

		if (playerCurrentSceneId == 0) {
			co_return false;
		}
	}

	const Point2f playerImgMapPoint(lastPlayerImgMapCoordinate.x, lastPlayerImgMapCoordinate.y);
	FeatureFilter::FilterNearKeypoints(App::FeatureData_map.imgKeypoints, App::FeatureData_map.imgDescriptors, playerImgMapPoint, 120, nearPlayerMapKeypoints, nearPlayerMapDescriptors);

	ImageFeatureData MapFeatureData(nearPlayerMapKeypoints, nearPlayerMapDescriptors);
	ImageFeatureData minMapFeatureData = FeatureMatch::ExtractSurfFeatures(surf, minMapImg);

	if (minMapFeatureData.imgDescriptors.empty())
		co_return false;

	vector<DMatch> goodMatch = FeatureMatch::FindGoodMatchesBetweenMapAndMinMap(minMapFeatureData, MapFeatureData);

	Coordinate PlayerImageMapCoordinate;
	bool isExistGoodCoordinate = MapCoordinate::GetGoodPlayerImgMapCoordinateFromMatches(
		minMapImg,
		goodMatch,
		nearPlayerMapKeypoints,
		minMapFeatureData.imgKeypoints,
		15.0f,
		lastPlayerImgMapCoordinate,
		PlayerImageMapCoordinate
	);

	if (isExistGoodCoordinate) {
		App::gameMapCenterPointImgMapCoord = lastPlayerImgMapCoordinate = PlayerImageMapCoordinate;
	}
	else {
		this_thread::sleep_for(std::chrono::milliseconds(120));
		if (isExistMinMap) {
			identifyCoordinate = { 0,0 };//下一个循环进行ocr
		}
		co_return false;
	}

	outPlayerROC = RelativeCoordinates::ImgMapCoordToROC(lastPlayerImgMapCoordinate, playerCurrentSceneId);
	outMinMapRadius = minMapImg.rows / 2;
	existMapCenterPointCoordinate = true;
	co_return true;
}

bool App::GetGameMapCenterPointROC(const Mat& snapshot, Coordinate& outGameMapCenterPointROC, Coordinate& outLastGameMapCenterPointROC) {
	Mat mapCenterAreaImgae = ImageProcessing::CropToMapCenterArea(snapshot, rect);

	Ptr<xfeatures2d::SURF> surt = xfeatures2d::SURF::create(100, 4, 3, true, true);
	ImageFeatureData mapCenterAreaFeatureData = FeatureMatch::ExtractSurfFeatures(surt, mapCenterAreaImgae);

	if (!mapCenterAreaFeatureData.imgDescriptors.empty()) {
		vector<vector<DMatch>> knnMatches;
		vector<KeyPoint> mapCenterPointNearKeypoints;
		Mat mapCenterPointNearDescriptors;

		if (existMapCenterPointCoordinate) {
			//true 小范围搜索featurePoint并给通过鼠标监视算的中心坐标赋正确的值（最后） 
			const Point2f ImgMapCoord_gameMapCenterPoint(App::gameMapCenterPointImgMapCoord.x, App::gameMapCenterPointImgMapCoord.y);
			FeatureFilter::FilterNearGoodKeypoints(App::FeatureData_map.imgKeypoints, App::FeatureData_map.imgDescriptors, ImgMapCoord_gameMapCenterPoint, 400, 16, mapCenterPointNearKeypoints, mapCenterPointNearDescriptors);
		}
		else {
			//通过鼠标监视算出的中心坐标有一定偏差
			//false 获取通过鼠标监视算的中心坐标 大范围搜索featurePoint
			const Point2f ImgMapCoord_gameMapCenterPoint(App::gameMapCenterCoordinateByMouseMonitoring.x, App::gameMapCenterCoordinateByMouseMonitoring.y);
			FeatureFilter::FilterNearGoodKeypoints(App::FeatureData_map.imgKeypoints, App::FeatureData_map.imgDescriptors, ImgMapCoord_gameMapCenterPoint, 2200, 22, mapCenterPointNearKeypoints, mapCenterPointNearDescriptors);
		}

		if (mapCenterPointNearDescriptors.empty()) {
			existMapCenterPointCoordinate = false;
			return false;
		}

		ImageFeatureData centerMapNearFeatureData(mapCenterPointNearKeypoints, mapCenterPointNearDescriptors);
		auto goodMatchs = FeatureMatch::FindGoodMatchesFLANN(mapCenterAreaFeatureData, centerMapNearFeatureData, 0.62f, 0.50f);

		Coordinate centerMapCoordinate; vector<Point2f> captrueCorners_temp;
		existMapCenterPointCoordinate = MapCoordinate::GetMapCoordinateOfCenterGameMapPos(centerMapNearFeatureData, mapCenterAreaFeatureData, goodMatchs, mapCenterAreaImgae, centerMapCoordinate, captrueCorners_temp);

		if (existMapCenterPointCoordinate) {
			 outGameMapCenterPointROC = RelativeCoordinates::ImgMapCoordToROC(centerMapCoordinate, playerCurrentSceneId);
			 outLastGameMapCenterPointROC = RelativeCoordinates::ImgMapCoordToROC(App::gameMapCenterPointImgMapCoord, playerCurrentSceneId);

			if (abs((captrueCorners[2].x - captrueCorners[0].x) - (captrueCorners_temp[2].x - captrueCorners_temp[0].x)) > 10)
				captrueCorners = captrueCorners_temp;

			App::gameMapCenterCoordinateByMouseMonitoring = App::gameMapCenterPointImgMapCoord = centerMapCoordinate;

			map_ConsecutiveFailuresCount = 0;
			return true;
		}
	}

	map_ConsecutiveFailuresCount++;
	if (map_ConsecutiveFailuresCount > 10)
		Notification::AddInfo(NotificationDatas("Please move the map to an area with obvious features or reopen the map.", 3));

	return false;
}

void App::Thread_GetItemMapScreenCoordinateByMouseMonitoring() {
	POINT screenPos = { 0,0 };
	POINT lastDragPos = {0,0};
	bool isDragging = false;
	POINT velocity = { 0,0 };
	float friction = 0.95;
	Coordinate mapCoordinatesOfMouseClickPos;

	while (!allThreadStopFlag) {
		if (IsWindowFocused(hwnd) && GetCursorPos(&screenPos)) {
			bool leftButtonDown = (GetAsyncKeyState(VK_LBUTTON) & 0x8000) != 0;
			gameMapCoordinatesOfMousePos = MapCoordinate::CalculateMouseClickPositionMapCoordinate(hwnd, screenPos, App::gameMapCenterPointImgMapCoord, captrueCorners);

			if (leftButtonDown) {
				if (!isDragging) {
					// 左键按下事件
					isDragging = true;
					lastDragPos = screenPos;
					mapCoordinatesOfMouseClickPos = MapCoordinate::CalculateMouseClickPositionMapCoordinate(hwnd, screenPos, gameMapCenterCoordinateByMouseMonitoring, captrueCorners);
				}
				else {
					// 拖拽过程
					gameMapCenterCoordinateByMouseMonitoring = MapCoordinate::CalculateGameMapCenterCoordinateByMouseLocation(hwnd, screenPos, mapCoordinatesOfMouseClickPos, captrueCorners);
					velocity = { lastDragPos.x - screenPos.x,lastDragPos.y - screenPos.y };
					lastDragPos = screenPos;

					mapNotMoving = false;
					DrawItemOnGameMap::ClearNearItemsData();
					DrawRouteOnMap::ClearRountsData();
				}
			}
			else if (isDragging) {
				// 左键松开事件
				isDragging = false;
			}

			if (!isDragging) {
				MapCoordinate::CalculateInertialslidingCoordinate(hwnd, velocity, captrueCorners, gameMapCenterCoordinateByMouseMonitoring);
				velocity.x *= friction;
				velocity.y *= friction;
				if (velocity.x < 0.001 && velocity.y < 0.001) {
					mapNotMoving = true;
				}
				else {
					mapNotMoving = false;
					DrawItemOnGameMap::ClearNearItemsData();
					DrawRouteOnMap::ClearRountsData();
				}
			}
		}
		std::this_thread::sleep_for(std::chrono::milliseconds(10));
	}
}

//TODO:需用户可自定义快捷键
void App::Thread_KeyMonitoring_SavePlayerNearItemPoint() {
	const int monitoredKey = 0x5A; //Z
	bool keyWasPressed = false; 
	while (true) {
		bool keyIsPressed = isKeyPressed(monitoredKey);
		if (keyIsPressed && !keyWasPressed) {
			keyWasPressed = true;
			DrawItemOnMinMap::SavePlayerNearItemPoint();
		}
		else if (!keyIsPressed && keyWasPressed) {
			keyWasPressed = false;
		}
		Sleep(50);
	}
}



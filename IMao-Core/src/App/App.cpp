#include "App.h"
#include "..\Coordinate\locationCalculator\RelativeCoordinates.h"
#include "..\Coordinate\locationCalculator\ScreenCoordinate.h"
#include "..\ImguiDraw\Items\DrawItemOnMinMap.h"
#include "..\ImguiDraw\Items\DrawItemOnGameMap.h"
#include "..\Feature\Processing\FeatureProcessing.h"
#include "..\WindowsCapture\WindowsGraphicsCapture\SimpleCapture.h"
#include "..\ImguiDraw\InteractiveInterface\Notification.h"

using namespace std;
using namespace cv;

int	App::updateMapDataCycleTime;
int App::updateMinMapDataCycleTime;
bool App::enabledMapShowItem;
bool App::enabledMinMapShowItem;

bool App::Init() {
	Notification::AddInfo(NotificationDatas("这是一款免费使用的软件，如果你是付钱买来的，你已经被骗了。", 20));
	Notification::AddInfo(NotificationDatas("The resource is loading, please be patient.", 10));

	const string path = GetCurrentPath() + "\\Assets\\FeaturesDatas";
	const string mapFeatureFilePath = path+"\\Map_features.yml";
	const string IconTask_FeatureFilePath = path + "\\IconTask_Features.yml";
	const string IconWavePlateCrystal_FeatureFilePath = path +"\\IconWavePlateCrystal_Features.yml";

	if (!FeatureLoader::loadFeatures(mapFeatureFilePath, FeatureData_map) or
		!FeatureLoader::loadFeatures(IconTask_FeatureFilePath, FeatureData_IconTask) or
		!FeatureLoader::loadFeatures(IconWavePlateCrystal_FeatureFilePath, FeatureData_wavePlateCrystal)) {
		Notification::AddInfo(NotificationDatas("Resource loading failed.", 30));
		return false;
	}

	if(!IdentifyWorldCoordinates::isLoaded)
		IdentifyWorldCoordinates::Init(GetCurrentPath() + "\\Assets\\models\\PP-OCRv5_mobile_det_infer", GetCurrentPath() + "\\Assets\\models\\PP-OCRv5_mobile_rec_infer",string(), GetCurrentPath() + "\\Assets\\models\\ch_ppocr_mobile_v2.0_cls_infer");

	Mat snapshot;
	GetClientRect(hwnd, &rect);
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

		if (isOpenMap and enabledMapShowItem and !gameSnapshot.empty() and isWindowFocused) {
			UpdateItemMapScreenCoordinateByMatch(gameSnapshot);
			cycleTime = App::updateMapDataCycleTime;
		}else {
			DrawItemOnGameMap::ClearNearItemsData();
		}

		if (isExistMinMap and !gameSnapshot.empty()) {
			co_await UpdateItemMinMapScreenCoordinate(gameSnapshot);
			cycleTime = App::updateMinMapDataCycleTime;
		}else{
			DrawItemOnMinMap::ClearNearItemsData();
		}

		auto endTime = std::chrono::high_resolution_clock::now();
		auto elapsedTime = std::chrono::duration_cast<std::chrono::milliseconds>(endTime - startTime).count();
		//cout << "elapsedTime:" << elapsedTime << endl;
		if (elapsedTime < cycleTime) {
			std::this_thread::sleep_for(std::chrono::milliseconds(cycleTime - elapsedTime));
		}
		startTime = std::chrono::high_resolution_clock::now();
	}
}

winrt::IAsyncAction App::GetMatSnapshot(bool isTaketAsync, Mat& result) {
	Mat temp;
	if (bitBltCapture.has_value()) {
		bitBltCapture->GetSnapshot(temp);
		temp.copyTo(result);
		co_return;
	}
	else if (graphicsCapture.has_value()) {
		if (isTaketAsync) {
			co_await graphicsCapture->TakeSnapshotAsync();
			temp = graphicsCapture->getCaptureResult();
		}
		else {
			graphicsCapture->Getcapture()->GetLatestFrame(temp);
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
		auto goodMatchs = FeatureMatch::FindGoodMatches(FeatureData_IconTask, test_FeatureData_IconTask, 0.5f, 0.5f, DescriptorMatcher::BRUTEFORCE_SL2);
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
		if (*goodMatchSize >= 16) {
			return true;
		}
	}
	else {
		*goodMatchSize = 0;
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

winrt::IAsyncAction App::UpdateItemMinMapScreenCoordinate(const Mat& snapshot) {
	Ptr<xfeatures2d::SURF> surf = xfeatures2d::SURF::create(8, 8, 4, true, true);
	Mat minMapImg = ImageProcessing::CropToMinMapAreaImg(snapshot, rect);

	if ((identifyCoordinate.x == 0 && identifyCoordinate.y == 0) || PlayerCurrentSceneId==0) {
		
		Notification::AddInfo(NotificationDatas("Continuity match failed, trying to identify coordinates.", 3));

		this_thread::sleep_for(std::chrono::milliseconds(120));
		Mat snapshot_ocr;
		co_await GetMatSnapshot(true, snapshot_ocr);
		if (!IdentifyWorldCoordinates::IdentifyCoordinateFromSnapshot(snapshot_ocr, identifyCoordinate, rect)) {
			identifyCoordinate = { 0,0 };
			co_return;
		}
		PlayerCurrentSceneId = GetCurrentSceneId(identifyCoordinate, minMapImg);

		if (PlayerCurrentSceneId == 0) {
			co_return;
		}
	}

	const Point2f playerImgMapPoint(lastPlayerImgMapCoordinate.x, lastPlayerImgMapCoordinate.y);
	FeatureFilter::FilterNearKeypoints(App::FeatureData_map.imgKeypoints, App::FeatureData_map.imgDescriptors, playerImgMapPoint, 130, nearPlayerMapKeypoints, nearPlayerMapDescriptors);

	ImageFeatureData MapFeatureData(nearPlayerMapKeypoints, nearPlayerMapDescriptors);
	ImageFeatureData minMapFeatureData = FeatureMatch::ExtractSurfFeatures(surf, minMapImg);
	if (minMapFeatureData.imgDescriptors.empty())
		co_return;
	vector<DMatch> goodMatch = FeatureMatch::FindGoodMatchesBetweenMapAndMinMap(minMapFeatureData, MapFeatureData);

	Coordinate PlayerImageMapCoordinate;
	bool isExistGoodCoordinate = MapCoordinate::GetGoodPlayerImgMapCoordinateFromMatches(
		minMapImg,
		goodMatch,
		nearPlayerMapKeypoints,
		minMapFeatureData.imgKeypoints,
		8.0f,
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
		co_return;
	}

	Coordinate playerRC = RelativeCoordinates::ImgMapCoordToRC(lastPlayerImgMapCoordinate, PlayerCurrentSceneId);
	if (enabledMinMapShowItem) {
		DrawItemOnMinMap::UpdatePlayerNearItemsData(rect, playerRC, minMapImg.rows / 2, PlayerCurrentSceneId);
	}
	else {
		DrawItemOnMinMap::ClearNearItemsData();
	}
	existMapCenterPointCoordinate = true;
}

void App::UpdateItemMapScreenCoordinateByMatch(const Mat& snapshot) {
	try {
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
				return;
			}

			ImageFeatureData centerMapNearFeatureData(mapCenterPointNearKeypoints, mapCenterPointNearDescriptors);
			auto goodMatchs = FeatureMatch::FindGoodMatchesFLANN(mapCenterAreaFeatureData, centerMapNearFeatureData, 0.62f, 0.50f);
			Coordinate centerMapCoordinate;
			vector<Point2f> captrueCorners_temp;
			existMapCenterPointCoordinate = MapCoordinate::GetMapCoordinateOfCenterGameMapPos(centerMapNearFeatureData, mapCenterAreaFeatureData, goodMatchs, mapCenterAreaImgae, centerMapCoordinate, captrueCorners_temp);

			if (existMapCenterPointCoordinate) {
				Coordinate gameMapCenterPointRwoc = RelativeCoordinates::ImgMapCoordToRC(centerMapCoordinate, PlayerCurrentSceneId);
				Coordinate lastGameMapCenterPointRwoc = RelativeCoordinates::ImgMapCoordToRC(App::gameMapCenterPointImgMapCoord, PlayerCurrentSceneId);
				if (abs((captrueCorners[2].x - captrueCorners[0].x) - (captrueCorners_temp[2].x - captrueCorners_temp[0].x)) > 10)
					captrueCorners = captrueCorners_temp;

				if (mapNotMoving)//mapNotMoved通过鼠标监视判断是不准确的 需要在UpdateCenterPointNearItemsData里进一步处理
					DrawItemOnGameMap::UpdateCenterPointNearItemsData(gameMapCenterPointRwoc, lastGameMapCenterPointRwoc, captrueCorners, rect ,PlayerCurrentSceneId);

				App::gameMapCenterCoordinateByMouseMonitoring = App::gameMapCenterPointImgMapCoord = centerMapCoordinate;

				map_ConsecutiveFailuresCount = 0;
				return;
			}
		}
	}
	catch (const exception& e) {
		cerr << "报错 " << e.what() << endl;
		return;
	}

	map_ConsecutiveFailuresCount++;
	if (map_ConsecutiveFailuresCount > 10)
		Notification::AddInfo(NotificationDatas("Please move the map to an area with obvious features or reopen the map.", 3));

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
				}
			}
		}
		std::this_thread::sleep_for(std::chrono::milliseconds(10));
	}
}

void App::Thread_KeyMonitoring_SavePlayerNearItemPoint() {
	const int monitoredKey = 0x58;
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
#pragma once
#include<vector>
#include "../../Coordinate/locationCalculator/ScreenCoordinate.h"
#include "DrawItemBase.h"

class DrawItemOnGameMap
{
public:
	static void UpdateCenterPointNearItemsData(const Coordinate& validGameMapcenterPointROC,const std::vector<cv::Point2f>& captureCorners, const RECT& rect, int senceId);
	static void DrawItemsOnGameMap(const RECT& rect, const HWND& hwnd);
	static void ClearNearItemsData() {
		std::lock_guard<std::mutex> lock(PointNearItemsDataMutex);
		centerPointNearItemsData.clear();
	}
	static void SetVisibleSavedPoints(bool value) {
		visibleSavedPoints = value;
	}
private:
	static std::vector<ItemDatas> centerPointNearItemsData;
	static std::string senceName;
	static std::vector<ItemsDatas>* itemsDatas_StoragePtr;

	static std::vector<ItemDatas> GetAndFilterItemsData(const Coordinate& gameMapcenterPointRC, const std::vector<cv::Point2f>& captureCorners, const RECT& rect);
	static bool IsMapMoving(const Coordinate& gameMapcenterPointROC);
	static bool GetBasicDataBySenceId(int senceId);
	static bool visibleSavedPoints;
	static std::mutex PointNearItemsDataMutex;
};


#pragma once
#include "vector"
#include "../../Coordinate/locationCalculator/ScreenCoordinate.h"
#include "DrawItemBase.h"


class DrawItemOnMinMap
{
public:
	static void DrawItemsOnMinMap(const RECT& rect);
	static void UpdatePlayerNearItemsData(HWND& hwnd, Coordinate& playerROC, float minMapRadius, int SceneId);
	static void UpdatePlayerNearItemsData(RECT &w_Rect, Coordinate &playerROC, float minMapRadius, int SceneId);
	static void SavePlayerNearItemPoint();
	static void ClearNearItemsData() {
		nearItemsDatas.clear();
	}

private:
	static std::vector<ItemDatas> nearItemsDatas;
	static std::string senceName;
	static std::vector<ItemsDatas>* itemsDatas_StoragePtr;

	static std::vector<ItemDatas> GetAndFilterItemsData(const RECT& rect, const Coordinate& playerROC, float minMapRadius);
	static bool GetBasicDataBySenceId(int senceId);
};

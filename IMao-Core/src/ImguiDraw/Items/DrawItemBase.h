#pragma once
#include <string>
#include <nlohmann/json.hpp>
#include "Windows.h"
#include "../ImGuiOverWindows.h"
#include "../../Coordinate/CoordinateStruct.h"
#include <thread>
#include <wrl/client.h>
using json = nlohmann::json;

struct ItemDatas {
	std::string itemId; 
	std::string nameId;
	Coordinate screenCoordiante;
	Coordinate itemMapRC;
	bool isSaved = false;
};

struct ItemsDatas {
	std::string nameId;
	std::vector<ItemDatas> itemsDatas;
};

struct ItemTextureData {
	std::string nameId;
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> texture;

	ItemTextureData(std::string nameId = "", const Microsoft::WRL::ComPtr<ID3D11ShaderResourceView>& texture = nullptr)
		: nameId(nameId), texture(texture) {
	}
};


class DrawItemBase {
public:
	static void Init() {
		DrawItemBase::LoadItemsjson();
		savedJosnPath = GetCurrentPath() + "\\SavedPoints\\account_1.json";
		thread_ReadSavedPointsJson = std::thread(&DrawItemBase::Thread_ReadSavedPointsJson);
	}
	static void RenderPointCircle(ImTextureID texture, ImVec2 position, float radius, float transparency, ImColor circleColor);
	static void AddItemDataFromJson(std::string itemId);
	static void ClearItemData(std::string itemId);
	static void SaveItemPoint(std::string scene, ItemDatas itemDatas);
	static void RemoveSavedItemPoint(std::string scene, ItemDatas itemDatas);
	static bool IsValidItemNameId(std::string itemNameId);
	static std::vector<std::string> GetFilteredPoints(std::string scene, std::string nameId);

	static std::vector<ItemTextureData> itemsTextureData;
	static std::vector<ItemsDatas> itemsDatas_World_Storage;
	static std::vector<ItemsDatas> itemsDatas_Tethys_Storage;
	static std::vector<ItemsDatas> itemsDatas_Fabricatorium_Storage;
	static json itemsJsonData_World;
	static json itemsJsonData_Tethys;
	static json itemsJsonData_Fabricatorium;
private:
	static json& SavedItemPoints();
	static void LoadItemsjson();
	static void Thread_ReadSavedPointsJson();
	static bool FindItemJsonData(int sceneId, json*& itemJsonData, std::vector<ItemsDatas>*& itemsDatas_Storage);
	//static json savedItemPoints;
	static std::thread thread_ReadSavedPointsJson;
    static std::string savedJosnPath;
};


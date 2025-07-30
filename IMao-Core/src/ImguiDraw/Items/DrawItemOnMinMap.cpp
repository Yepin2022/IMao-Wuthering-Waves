#include "DrawItemOnMinMap.h"

using namespace std;
vector<ItemDatas> DrawItemOnMinMap::nearItemsDatas;
Coordinate minMapCenterPoint;
string DrawItemOnMinMap::senceName = "World";
vector<ItemsDatas>* DrawItemOnMinMap::itemsDatas_StoragePtr = nullptr;

void DrawItemOnMinMap::UpdatePlayerNearItemsData(HWND &hwnd,Coordinate & playerRC, float minMapRadius,int SceneId) {
	RECT w_Rect;
	GetClientRect(hwnd, &w_Rect);
    if(GetBasicDataBySenceId(SceneId))
        nearItemsDatas = GetAndFilterItemsData(w_Rect, playerRC,minMapRadius);
}

void DrawItemOnMinMap::UpdatePlayerNearItemsData(RECT &w_Rect, Coordinate & playerRC,float minMapRadius, int SceneId) {
    if(GetBasicDataBySenceId(SceneId))
        nearItemsDatas = GetAndFilterItemsData(w_Rect, playerRC, minMapRadius);
}

bool DrawItemOnMinMap::GetBasicDataBySenceId(int senceId) {
    if (senceId == 1) {
        senceName = "World";
        itemsDatas_StoragePtr = &DrawItemBase::itemsDatas_World_Storage;
        return true;
    }

    if (senceId == 2) {
        senceName = "Tethys";
        itemsDatas_StoragePtr = &DrawItemBase::itemsDatas_Tethys_Storage;
        return true;
    }

    if (senceId == 3) {
        senceName = "Fabricatorium";
        itemsDatas_StoragePtr = &DrawItemBase::itemsDatas_Fabricatorium_Storage;
        return true;
    }

    return false;
}

vector<ItemDatas> DrawItemOnMinMap::GetAndFilterItemsData(const RECT& rect, const Coordinate& playerRC, float minMapRadius) {
    vector<ItemDatas> nearFilterItemsData;
    for (const auto& itemsData : *itemsDatas_StoragePtr) {
        vector<string> filteredPoints = DrawItemBase::GetFilteredPoints(senceName, itemsData.nameId);
        for (const auto& itemDatas : itemsData.itemsDatas) {
            if (abs(playerRC.x - itemDatas.itemMapRC.x) < 100 && abs(playerRC.y - itemDatas.itemMapRC.y) < 100) {
                Coordinate itemScreen = ScreenCoordinate::ItemScreenCoordinateOnMinMap(rect, itemDatas.itemMapRC, playerRC);
                minMapCenterPoint = ScreenCoordinate::MinMapCircleCenterScreenCoordinate(rect);
                float twoPointDistance = CalculatePointDistance(itemScreen, minMapCenterPoint);
                if (twoPointDistance <= minMapRadius) {
                    bool isSaved = false;
                    for (const auto& filteredPoint : filteredPoints) {
                        if (filteredPoint == itemDatas.itemId) {
                            isSaved = true;
                            break;
                        }
                    }
                    ItemDatas tempItemData = { itemDatas.itemId ,itemDatas.nameId ,itemScreen,itemDatas.itemMapRC,isSaved };
                    nearFilterItemsData.push_back(tempItemData);
                }
            }
        }
    }
    return nearFilterItemsData;
}

void DrawItemOnMinMap::SavePlayerNearItemPoint() {
    if (nearItemsDatas.empty()) {
        return;
    }
    vector<ItemDatas> itemsDatas = nearItemsDatas;
    for (const auto& itemDatas : itemsDatas) {
        if (!itemDatas.isSaved and abs(minMapCenterPoint.x - itemDatas.screenCoordiante.x) < 10 and abs(minMapCenterPoint.y - itemDatas.screenCoordiante.y)< 10){
            DrawItemBase::SaveItemPoint(senceName, itemDatas);
        }
    }
}

void DrawItemOnMinMap::DrawItemsOnMinMap(const RECT& rect) {
    if (nearItemsDatas.empty()) {
        return;
    }
    vector<ItemDatas> itemsData = nearItemsDatas;
    for (const auto& itemData : itemsData) {
        int image_width1;
        int image_height1;
        bool ret = false;
        Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> texture = nullptr;

        if (itemData.isSaved)
            continue;

        for (const auto& itemTextureData : DrawItemBase::itemsTextureData) {
            if (itemData.nameId == itemTextureData.nameId) {
                texture = itemTextureData.texture;
                ret = true;
                break;
            }
        }

        if (texture == nullptr && !itemData.nameId.empty()) {
            std::wstring temp = L"IDB_PNG_" + std::wstring(itemData.nameId.begin(), itemData.nameId.end());
            ret = ImGuiOverWindows::LoadTextureFromResource(temp.c_str(), &texture, &image_width1, &image_height1);
            //ret = ImGuiOverWindows::LoadTextureFromPath(itemData.iconPath.c_str(), &texture, &image_width1, &image_height1);
           // DrawItemBase::itemsTextureData.push_back(ItemTextureData(itemData.nameId, texture.Get()));
            DrawItemBase::itemsTextureData.push_back(ItemTextureData(itemData.nameId, texture));
        }
       
        if (ret) {
            float radius = (rect.right * 0.012f) / 2;
            ImVec2 screenPosition(itemData.screenCoordiante.x , itemData.screenCoordiante.y );
            DrawItemBase::RenderPointCircle(reinterpret_cast<ImTextureID>(texture.Get()), screenPosition, radius, 0.9f, ImColor(0.91f, 0.68f, 0.36f, 0.8f));
        }
    }
}
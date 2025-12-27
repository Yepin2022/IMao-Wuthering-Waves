#include "DrawItemBase.h"
#include <fstream>
#include "../../DLL_API.h"
#include "../../util.h"
#include "../../Coordinate/locationCalculator/RelativeCoordinates.h"
#include "../InteractiveInterface/Notification.h"
#include <filesystem>
#include <shared_mutex>

using namespace std;
namespace fs = filesystem;

json DrawItemBase::itemsJsonData_World;
json DrawItemBase::itemsJsonData_Tethys;
json DrawItemBase::itemsJsonData_Fabricatorium;
json DrawItemBase::itemsJsonData_Avinoleum;
json DrawItemBase::itemsJsonData_Lahai;

vector<ItemTextureData> DrawItemBase::itemsTextureData;
vector<ItemsDatas> DrawItemBase::itemsDatas_World_Storage;
vector<ItemsDatas> DrawItemBase::itemsDatas_Tethys_Storage;
vector<ItemsDatas> DrawItemBase::itemsDatas_Fabricatorium_Storage;
vector<ItemsDatas> DrawItemBase::itemsDatas_Avinoleum_Storage;
vector<ItemsDatas> DrawItemBase::itemsDatas_Lahai_Storage;
thread DrawItemBase::thread_ReadSavedPointsJson;
string DrawItemBase::savedJsonPath;

static std::shared_mutex g_jsonMutex;          // 读写锁 
static std::filesystem::file_time_type g_lastTime; // 上一次修改时间 

json& DrawItemBase::GetSavedItemPoints() {
    static json j;
    return j;
}

void DrawItemBase::Initi() {
    DrawItemBase::LoadItemsjson();
    savedJsonPath = GetCurrentPath() + "\\SavedPoints\\account_1.json";

    try {
        fs::path folderPath(GetCurrentPath() + "\\SavedPoints");

        if (!fs::exists(folderPath)) {
            fs::create_directories(folderPath);
        }
    }
    catch (const exception& e) {
        cerr << "  DrawItemBase::Initi:" << e.what() << endl;
    }

    thread_ReadSavedPointsJson = std::thread(&DrawItemBase::Thread_ReadSavedPointsJson);
}

bool LoadJson(json& JsonData, const wchar_t* resourceName) {
    HRSRC hResInfo = FindResource(g_hDllInstance, resourceName, L"JSON");
    if (!hResInfo) {
        std::cout << "无法找到资源！" << std::endl;
        return false;
    }

    HGLOBAL hResData = LoadResource(g_hDllInstance, hResInfo);
    if (!hResData) {
        std::cout << "无法加载资源！" << std::endl;
        return false;
    }

    LPVOID pResData = LockResource(hResData);
    if (!pResData) {
        std::cout << "无法锁定资源！" << std::endl;
        return false;
    }

    DWORD resSize = SizeofResource(g_hDllInstance, hResInfo);

    try {
        std::string jsonStr(static_cast<const char*>(pResData), resSize);
        JsonData = json::parse(jsonStr);
    }
    catch (const json::parse_error& e) {
        std::cout << "JSON parsing error:" << e.what() << std::endl;
        return false;
    }
}

void DrawItemBase::LoadItemsjson() {
    LoadJson(itemsJsonData_Tethys, L"ITEMSJSON_Tethys");
    LoadJson(itemsJsonData_World, L"ITEMSJSON_World");
    LoadJson(itemsJsonData_Fabricatorium, L"ITEMSJSON_Fabricatorium");
    LoadJson(itemsJsonData_Avinoleum, L"ITEMSJSON_Avinoleum");
    LoadJson(itemsJsonData_Lahai, L"ITEMSJSON_Lahai");
}

bool DrawItemBase::IsValidItemNameId(string itemNameId) {
    for (const auto& itemsDatas : itemsDatas_World_Storage) {
        if (itemsDatas.nameId == itemNameId) {
            return true;
        }
    }

    for (const auto& itemsDatas : itemsDatas_Tethys_Storage) {
        if (itemsDatas.nameId == itemNameId) {
            return true;
        }
    }

    for (const auto& itemsDatas : itemsDatas_Fabricatorium_Storage) {
        if (itemsDatas.nameId == itemNameId) {
            return true;
        }
    }

    for (const auto& itemsDatas : itemsDatas_Avinoleum_Storage) {
        if (itemsDatas.nameId == itemNameId) {
            return true;
        }
    }

    for (const auto& itemsDatas : itemsDatas_Lahai_Storage) {
        if (itemsDatas.nameId == itemNameId) {
            return true;
        }
    }

    return false;
}

bool DrawItemBase::FindItemJsonData(int sceneId, json*& itemJsonData, vector<ItemsDatas>*& itemsDatas_Storage) {
    if (sceneId == 1) {
        itemJsonData = &itemsJsonData_World;
        itemsDatas_Storage = &itemsDatas_World_Storage;
        return true;
    }

    if (sceneId == 2) {
        itemJsonData = &itemsJsonData_Tethys;
        itemsDatas_Storage = &itemsDatas_Tethys_Storage;
        return true;
    }

    if (sceneId == 3) {
        itemJsonData = &itemsJsonData_Fabricatorium;
        itemsDatas_Storage = &itemsDatas_Fabricatorium_Storage;
        return true;
    }

    if (sceneId == 4) {
        itemJsonData = &itemsJsonData_Avinoleum;
        itemsDatas_Storage = &itemsDatas_Avinoleum_Storage;
        return true;
    }

    if (sceneId == 5) {
        itemJsonData = &itemsJsonData_Lahai;
        itemsDatas_Storage = &itemsDatas_Lahai_Storage;
        return true;
    }

    return false;
}

void DrawItemBase::AddItemDataFromJson(string itemId) {
    try {
        json* itemsJsonDataPtr = nullptr;
        vector<ItemsDatas>* itemsDatas_StoragePtr = nullptr;

        for (const auto& sceneId : Scene::sceneIds) {
            vector<ItemDatas> itemsDatas;
           
            if (!FindItemJsonData(sceneId, itemsJsonDataPtr, itemsDatas_StoragePtr))
                return;

            for (const auto& [item_id, item_info] : (*itemsJsonDataPtr).items()) {
                string nameId = item_info["id"].get<string>();
                if (nameId == itemId) {
                    for (const auto& location : item_info["location"]) {
                        double IdentifyCoord_x = location["x"] / 100;
                        double IdentifyCoord_y = location["y"] / 100;
                        Coordinate itemMapROC = RelativeCoordinates::IdentifyCoordToROC(Coordinate(IdentifyCoord_x, IdentifyCoord_y),sceneId);
   
                        string s = location["id"].get<string>();
                        ItemDatas tempItemDatas = { s ,nameId,Coordinate(0,0),itemMapROC ,false };
                        itemsDatas.push_back(tempItemDatas);
                    }
                   (*itemsDatas_StoragePtr).push_back(ItemsDatas(nameId, itemsDatas));
                }
            }

        }
    }
    catch (const exception& e) {
        cerr << "Exception in AddItemDataFromJson: " << e.what() << endl;
        return;
    }
}

void DrawItemBase::ClearItemData(string itemId) {
    for (int i = 0; i < itemsDatas_World_Storage.size();i++) {
        if (itemsDatas_World_Storage[i].nameId == itemId) {
            itemsDatas_World_Storage.erase(itemsDatas_World_Storage.begin() + i);
            break;
        }
    }

    for (int i = 0; i < itemsDatas_Tethys_Storage.size(); i++) {
        if (itemsDatas_Tethys_Storage[i].nameId == itemId) {
            itemsDatas_Tethys_Storage.erase(itemsDatas_Tethys_Storage.begin() + i);
            break;
        }
    }

    for (int i = 0; i < itemsDatas_Avinoleum_Storage.size(); i++) {
        if (itemsDatas_Avinoleum_Storage[i].nameId == itemId) {
            itemsDatas_Avinoleum_Storage.erase(itemsDatas_Avinoleum_Storage.begin() + i);
            break;
        }
    }

    for (int i = 0; i < itemsDatas_Lahai_Storage.size(); i++) {
        if (itemsDatas_Lahai_Storage[i].nameId == itemId) {
            itemsDatas_Lahai_Storage.erase(itemsDatas_Lahai_Storage.begin() + i);
            break;
        }
    }
}


void DrawItemBase::RenderPointCircle(ImTextureID texture, ImVec2 position,float radius,float transparency, ImColor circleColor) {
    auto draw = ImGui::GetBackgroundDrawList();
    draw->AddCircleFilled(position, radius, ImColor(0.23f, 0.26f, 0.32f, transparency));

    draw->AddImageRounded(texture, ImVec2(position.x - radius, position.y - radius), ImVec2(position.x + radius, position.y + radius), ImVec2(0, 0), ImVec2(1, 1), ImColor(1.0f, 1.0f, 1.0f, transparency), radius);

    draw->AddCircle(position, radius, circleColor);
}

void DrawItemBase::SaveItemPoint(string scene, ItemDatas itemDatas) {
    try {
        unique_lock lock(g_jsonMutex);
        auto& j = GetSavedItemPoints();

        j[scene][itemDatas.nameId].push_back({ {"id", itemDatas.itemId} });

        {   
            ofstream out_file(savedJsonPath);
            out_file << j.dump(4);
        }

        g_lastTime = filesystem::last_write_time(savedJsonPath);
    }
    catch (const exception& e) {
        Notification::AddInfo(NotificationDatas("DrawItemBase::SaveItemPoint: " + string(e.what()), 5));
    }
}

void DrawItemBase::RemoveSavedItemPoint(string scene, ItemDatas itemDatas) {
    try {
        unique_lock lock(g_jsonMutex);// 独占写 
        auto& j = GetSavedItemPoints();

        auto& item_array = j[scene][itemDatas.nameId];
        for (auto it = item_array.begin(); it != item_array.end(); ++it) {
            if (it->at("id") == itemDatas.itemId) {
                item_array.erase(it);
                break;
            }
        }
        ofstream out_file(savedJsonPath);

        out_file << j.dump(4);
        g_lastTime = filesystem::last_write_time(savedJsonPath);
    }catch (const exception& e) {
        Notification::AddInfo(NotificationDatas("DrawItemBase::RemoveSavedItemPoint: " + string(e.what()), 5));
    }
}

vector<string> DrawItemBase::GetFilteredPoints(string scene,string nameId) {
    vector<string> out;
    try {
        shared_lock lock(g_jsonMutex);
        const auto& j = GetSavedItemPoints();

        if (!j.contains(scene) ||
            !j[scene].is_object() ||
            !j[scene].contains(nameId) ||
            !j[scene][nameId].is_array()) {
            return out;
        }

        for (const auto& item : j[scene][nameId]) {
            if (item.contains("id") && item["id"].is_string()) {
                out.emplace_back(item["id"].get<string>());
            }
        }
    }
    catch (const exception& e) {
        cerr << "GetFilteredPoints exception: " << e.what() << '\n';
    }
    return out;
}

void DrawItemBase::Thread_ReadSavedPointsJson() {
    while (true) {
        try {
            if (fs::exists(savedJsonPath)) {
                auto t = fs::last_write_time(savedJsonPath);
                if (t != g_lastTime) {  // 只有变化才读 
                    unique_lock lock(g_jsonMutex);
                    ifstream file(savedJsonPath);
                    file >> GetSavedItemPoints();
                    g_lastTime = t;
                }
            }
        }
        catch (const exception& e) {
            Notification::AddInfo(NotificationDatas("JSON reload failed.", 3));
        }
        this_thread::sleep_for(chrono::milliseconds(250));
    }
}

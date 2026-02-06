#pragma once
#include <iostream>
#include<vector>
#include <string>
struct Coordinate {
    double x;
    double y;
    Coordinate(double x = 0, double y = 0) : x(x), y(y) {}

    bool IsValid() {
        if (x == 0 and y == 0) {
            return false;
        }
        return true;
    }
};

//大世界坐标(0,0)在图片地图上对应的坐标
struct WorldOriginCoordinates {
    inline static double x = 2474;
    inline static double y = 1957;
};

//泰缇斯之底坐标(0，0)在图片地图上对应的坐标
//TODO:游戏内大小地图有偏差bug，需在原来的基础上，x-10，y+20
struct TethysOriginCoordinates {
    inline static double x = 8593;
    inline static double y = 1382;
};

//隐海试验厂 3
//TODO:游戏内大小地图有偏差bug，需在原来的基础上，x+6
struct FabricatoriumOriginCoordinates {
    inline static double x = 7437;
    inline static double y = 13783;
};

//阿维纽林 4
struct AvinoleumOriginCoordinates {
    inline static double x = 2433;
    inline static double y = 9030;
};

//罗伊冰原 5
struct LahaiOriginCoordinates {
    inline static double x = 21662;
    inline static double y = 13138;
};

//TODO:需要适配更多地图
struct Scene {
    inline static std::vector<int> sceneIds = {1,2,3,4,5};
    inline static std::vector<std::string> sceneNames = { "World","Tethys","Fabricatorium","Avinoleum","Lahai"};
    
    static std::string SceneIdToName(int sceneId) {
        for (size_t i = 0; i < sceneIds.size(); i++) {
            if (sceneIds[i] == sceneId) {
                return sceneNames[i];
            }
        }
        return std::string();
    }

    static int SceneNameToId(std::string sceneName) {
        for (size_t i = 0; i < sceneNames.size(); i++) {
            if (sceneNames[i] == sceneName) {
                return sceneIds[i];
            }
        }
        return -1;
    }
};

struct GameWindowsScreenData {
    //TODO:目前仅支持16:9的分辨率,需要适配更多
    inline static double w_width = 1600;
    inline static double w_height = 900;
    inline static double ScaleFactorFromMinMapToMap = 1.25;
    inline static double minMapOnMap_width = 194;
    inline static double minMapOnMap_height = 194;

    inline static Coordinate MinMapCenter = { 108,100 };
    inline static Coordinate MinMapTop = { 110,23 };
    inline static Coordinate MinMapBottom = { 108,177 };
    inline static Coordinate MinMapLeft = { 30,100 };
    inline static Coordinate MinMapRight = { 184,100 };
    inline static std::vector<Coordinate> MinMapScreenData = { MinMapTop ,MinMapBottom,MinMapLeft ,MinMapRight };

    inline static Coordinate ShowWorldCoordinateAreaTop = {90,865};
    inline static Coordinate ShowWorldCoordinateAreaBottom = { 90,898 };
    inline static Coordinate ShowWorldCoordinateAreaLeft = {27,888};
    inline static Coordinate ShowWorldCoordinateAreaRight = {160,888};
    inline static std::vector<Coordinate> ShowWorldAreaScreenData = { ShowWorldCoordinateAreaTop ,ShowWorldCoordinateAreaBottom,ShowWorldCoordinateAreaLeft ,ShowWorldCoordinateAreaRight };

    inline static Coordinate IconTask_Top = { 25,183 };
    inline static Coordinate IconTask_Bottom = { 25,207 };
    inline static Coordinate IconTask_Left = { 12,195 };
    inline static Coordinate IconTask_Right = { 39,195 };
    inline static std::vector<Coordinate> IconTask_ScreenData = {IconTask_Top ,IconTask_Bottom ,IconTask_Left ,IconTask_Right };

    inline static Coordinate IconWavePlateCrystal_Top = { 978,37};
    inline static Coordinate IconWavePlateCrystal_Bottom = { 989,66 };
    inline static Coordinate IconWavePlateCrystal_Left = { 969,52 };
    inline static Coordinate IconWavePlateCrystal_Right= { 999,52 };
    inline static std::vector<Coordinate> IconWavePlateCrystal_ScreenData = { IconWavePlateCrystal_Top ,IconWavePlateCrystal_Bottom ,IconWavePlateCrystal_Left ,IconWavePlateCrystal_Right };

    //350*350
    inline static Coordinate mapCenterArea_Top = {796,275};
    inline static Coordinate mapCenterArea_Bottom = {796,625};
    inline static Coordinate mapCenterArea_Left = {625,456};
    inline static Coordinate mapCenterArea_Right = {975,456};
    inline static std::vector<Coordinate> mapCenterAreaSrceenData = { mapCenterArea_Top ,mapCenterArea_Bottom ,mapCenterArea_Left ,mapCenterArea_Right };
};
#pragma once
#include <iostream>
#include<vector>
struct Coordinate {
    double x;
    double y;
    Coordinate(double x = 0, double y = 0) : x(x), y(y) {}
};

//大世界坐标(0,0)在图片地图上对应的坐标
struct WorldOriginCoordinates {
    inline static double x = 4175;
    inline static double y = 2212;
};

//泰缇斯之底坐标(0，0)在图片地图上对应的坐标
struct TethysOriginCoordinates {
    inline static double x = 11247;
    inline static double y = 2122;
};

struct FabricatoriumOriginCoordinates {
    inline static double x = 7751;
    inline static double y = 13982;
};

//TODO:需要适配更多地图
struct Scene {
    inline static std::vector<int> sceneIds = { 1,2 ,3};
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

    inline static Coordinate IconWavePlateCrystal_Top = { 1048,38 };
    inline static Coordinate IconWavePlateCrystal_Bottom = { 1047,65 };
    inline static Coordinate IconWavePlateCrystal_Left = { 1035,52 };
    inline static Coordinate IconWavePlateCrystal_Right= { 1064,52 };
    inline static std::vector<Coordinate> IconWavePlateCrystal_ScreenData = { IconWavePlateCrystal_Top ,IconWavePlateCrystal_Bottom ,IconWavePlateCrystal_Left ,IconWavePlateCrystal_Right };

    //350*350
    inline static Coordinate mapCenterArea_Top = {796,275};
    inline static Coordinate mapCenterArea_Bottom = {796,625};
    inline static Coordinate mapCenterArea_Left = {625,456};
    inline static Coordinate mapCenterArea_Right = {975,456};
    inline static std::vector<Coordinate> mapCenterAreaSrceenData = { mapCenterArea_Top ,mapCenterArea_Bottom ,mapCenterArea_Left ,mapCenterArea_Right };
};
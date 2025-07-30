#include "ScreenCoordinate.h"
#include "../../util.h"

using namespace cv;
using namespace std;
Coordinate ScreenCoordinate::MinMapCircleCenterScreenCoordinate(const RECT &w_Rect) {
    double w_width = w_Rect.right;  
    double w_height = w_Rect.bottom;

    Coordinate MinMapCenter;
    MinMapCenter.x = GameWindowsScreenData::MinMapCenter.x * (w_width / GameWindowsScreenData::w_width);
    MinMapCenter.y = GameWindowsScreenData::MinMapCenter.y * (w_height / GameWindowsScreenData::w_height);

    return MinMapCenter;
}

//水平缩放因子*x，垂直缩放因子*y
RectangularAreaScreenLocation ScreenCoordinate::MinMapScreenCoordinate(HWND& w_hwnd) {
    double HorizontalFactor = 0;//水平缩放因子
    double VerticaFactor = 0;//垂直缩放因子

    CalculateWindowScalingFactors(w_hwnd, HorizontalFactor, VerticaFactor);

    Coordinate topPoint(GameWindowsScreenData::MinMapTop.x * HorizontalFactor,
        GameWindowsScreenData::MinMapTop.y * VerticaFactor);

    Coordinate bottomPoint(GameWindowsScreenData::MinMapBottom.x * HorizontalFactor,
        GameWindowsScreenData::MinMapBottom.y * VerticaFactor);

    Coordinate leftPoint(GameWindowsScreenData::MinMapLeft.x * HorizontalFactor,
        GameWindowsScreenData::MinMapLeft.y * VerticaFactor);

    Coordinate rightPoint(GameWindowsScreenData::MinMapRight.x * HorizontalFactor,
        GameWindowsScreenData::MinMapRight.y * VerticaFactor);

    RectangularAreaScreenLocation MPCSC(leftPoint, rightPoint, topPoint, bottomPoint);

    return MPCSC;
}


RectangularAreaScreenLocation ScreenCoordinate::SpecifyScreenCoordinate(const RECT& w_Rect, std::vector<Coordinate> specifyAreaScreenData) {
    double HorizontalFactor = 0;//水平缩放因子
    double VerticaFactor = 0;//垂直缩放因子

    CalculateWindowScalingFactors(w_Rect, HorizontalFactor, VerticaFactor);

    Coordinate topPoint(specifyAreaScreenData[0].x * HorizontalFactor,
        specifyAreaScreenData[0].y * VerticaFactor);

    Coordinate bottomPoint(specifyAreaScreenData[1].x * HorizontalFactor,
        specifyAreaScreenData[1].y * VerticaFactor);

    Coordinate leftPoint(specifyAreaScreenData[2].x * HorizontalFactor,
        specifyAreaScreenData[2].y * VerticaFactor);

    Coordinate rightPoint(specifyAreaScreenData[3].x * HorizontalFactor,
        specifyAreaScreenData[3].y * VerticaFactor);

    RectangularAreaScreenLocation SCSC(leftPoint, rightPoint, topPoint, bottomPoint);

    return SCSC;
}

RectangularAreaScreenLocation ScreenCoordinate::SpecifyScreenCoordinate(const HWND& hwnd, std::vector<Coordinate> specifyAreaScreenData) {
    RECT rect;
    GetClientRect(hwnd, &rect);

    return ScreenCoordinate::SpecifyScreenCoordinate(rect, specifyAreaScreenData);
}

//已知item相对世界原点的坐标  玩家相对世界原点的坐标 小地图中心点在屏幕的坐标，那么item的屏幕坐标为
//item相对玩家的坐标.x * 小地图相对地图的缩放比例 + 小地图中心点在屏幕的坐标.x
//小地图中心点在屏幕的坐标.y - item相对玩家的坐标.y * 小地图相对地图的缩放比例
//小地图相对地图的缩放比例 =  1 - (1.25 - (w / 1600))
Coordinate ScreenCoordinate::ItemScreenCoordinateOnMinMap(const HWND& hwnd,const Coordinate& itemRC,const Coordinate& playerRC) {
    double HorizontalFactor = 0;//水平缩放因子
    double VerticaFactor = 0;//垂直缩放因子
    CalculateWindowScalingFactors(hwnd, HorizontalFactor, VerticaFactor);
    Coordinate MinMapCenterScreen(HorizontalFactor * GameWindowsScreenData::MinMapCenter.x, VerticaFactor * GameWindowsScreenData::MinMapCenter.y);


    RECT rect;
    GetClientRect(hwnd, &rect);
    double scaleFactorFromMapToMinMap = 1 - (GameWindowsScreenData::ScaleFactorFromMinMapToMap - (rect.right / 1600.0f));


    Coordinate itemRelativeCoordinateToPlayerRwoc = RelativeCoordinates::GetRelativeCoordinates(itemRC, playerRC);


    Coordinate itemScreenCoordiante(itemRelativeCoordinateToPlayerRwoc.x * scaleFactorFromMapToMinMap + MinMapCenterScreen.x,MinMapCenterScreen.y - itemRelativeCoordinateToPlayerRwoc.y * scaleFactorFromMapToMinMap);

    return itemScreenCoordiante;
  
}


Coordinate ScreenCoordinate::ItemScreenCoordinateOnMinMap(const RECT& rect, const Coordinate& itemRC, const Coordinate& playerRC) {
    double HorizontalFactor = 0;//水平缩放因子
    double VerticaFactor = 0;//垂直缩放因子
    CalculateWindowScalingFactors(rect, HorizontalFactor, VerticaFactor);
    Coordinate MinMapCenterScreen(HorizontalFactor * GameWindowsScreenData::MinMapCenter.x, VerticaFactor * GameWindowsScreenData::MinMapCenter.y);

    double scaleFactorFromMapToMinMap = 1 - (GameWindowsScreenData::ScaleFactorFromMinMapToMap - (rect.right / 1600.0f));

    Coordinate itemRelativeCoordinateToPlayerRwoc = RelativeCoordinates::GetRelativeCoordinates(itemRC, playerRC);


    Coordinate itemScreenCoordianteOnMinMap(itemRelativeCoordinateToPlayerRwoc.x * scaleFactorFromMapToMinMap + MinMapCenterScreen.x, MinMapCenterScreen.y - itemRelativeCoordinateToPlayerRwoc.y * scaleFactorFromMapToMinMap);

    return itemScreenCoordianteOnMinMap;

}


Coordinate ScreenCoordinate::ItemScreenCoordinateOnMap(const Coordinate &gameMapCenterPointImgMapRC,const Coordinate &itemRC,const vector<Point2f> &captureCorners,const RECT &w_rect) {
    if (captureCorners.empty()) {
        return Coordinate(-1, -1);
    }

    auto MapCenterAreaData = ScreenCoordinate::SpecifyScreenCoordinate(w_rect, GameWindowsScreenData::mapCenterAreaSrceenData);

    float scalingFactor =  (MapCenterAreaData.rightPoint.x - MapCenterAreaData.leftPoint.x)/ (captureCorners[2].x - captureCorners[0].x);

   // Coordinate gameMapCenterPointRWOC = RelativeCoordinates::ImgMapCoordToRWOC(gameMapCenter);
    Coordinate ScreenCenter(w_rect.right / 2, w_rect.bottom / 2);

   float itemScreenCoordinateOnMap_x = ScreenCenter.x + (itemRC.x - gameMapCenterPointImgMapRC.x) * scalingFactor;
   float itemScreenCoordinateOnMap_y = ScreenCenter.y - (itemRC.y - gameMapCenterPointImgMapRC.y) * scalingFactor;

   return Coordinate(itemScreenCoordinateOnMap_x, itemScreenCoordinateOnMap_y);
}
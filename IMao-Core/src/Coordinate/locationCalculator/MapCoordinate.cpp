#include "MapCoordinate.h"
#include "Windows.h"
#include "../locationCalculator/ScreenCoordinate.h"

using namespace cv;
using namespace std;

//通过原图和测试图匹配点计算玩家在地图的坐标---测试图(不同分辨率有不同的缩放)的中心点在原图的坐标
Coordinate CalculatePlayerImgMapCoordinate(Mat minMapImage,const Point2f& mapPoint,const Point2f& minMapPoint) {

    double plyerImgMap_x = mapPoint.x - ( GameWindowsScreenData::minMapOnMap_width / minMapImage.cols) * (minMapPoint.x - minMapImage.cols / 2);
    double plyerImgMap_y = mapPoint.y - (GameWindowsScreenData::minMapOnMap_height / minMapImage.rows) * (minMapPoint.y - minMapImage.rows / 2);

    return Coordinate(plyerImgMap_x, plyerImgMap_y);
}

//筛选出与玩家上一个地图坐标距离在最大阈值内的坐标
vector<Coordinate> FilterPlaerImgMapCoordinates(const vector<Coordinate>& allCoordinates, const Coordinate& lastCoordinate, float maxDistance) {
    vector<Coordinate> filteredCoordinates;
    for (const auto& coordinate : allCoordinates) {
        if (abs(coordinate.x - lastCoordinate.x) <= maxDistance && abs(coordinate.y - lastCoordinate.y) <= maxDistance) {
            filteredCoordinates.push_back(coordinate);
        }
    }
    return filteredCoordinates;
}

// 通过计算一组坐标点在x和y方向上的中位数，算出最优玩家在地图上的坐标
Coordinate CalculateMedianCoordinate(vector<Coordinate>& playerImgCoordinates) {
    Coordinate outresult;
    vector<double> sort_x;
    vector<double> sort_y;

    for (const auto& playerImgCoordinate : playerImgCoordinates) {
        sort_x.push_back(playerImgCoordinate.x);
        sort_y.push_back(playerImgCoordinate.y);
    }

    if (sort_x.size() == 1) {
        outresult.x = sort_x[0];
        outresult.y = sort_y[0];
        return outresult;
    }

    std::sort(sort_x.begin(), sort_x.end());
    std::sort(sort_y.begin(), sort_y.end());

    if (sort_x.size() % 2 == 1) {
        outresult.x = sort_x[sort_x.size() / 2];
        outresult.y = sort_y[sort_y.size() / 2];
    }
    else {
        outresult.x = (sort_x[sort_x.size() / 2 - 1] + sort_x[sort_x.size() / 2]) / 2;
        outresult.y = (sort_y[sort_y.size() / 2 - 1] + sort_y[sort_y.size() / 2]) / 2;
    }
    return outresult;
}

// 根据匹配结果和关键点计算出MapImage的坐标并筛选符合要求的
bool MapCoordinate::GetGoodPlayerImgMapCoordinateFromMatches(const Mat& minMapImage,const vector<DMatch>& matches, const vector<KeyPoint>& mapKeypoints, const vector<KeyPoint>& minMapKeypoints, float maxDistance, const Coordinate& lastCoordinate, Coordinate& outPlayerMapCoordinate) {
    vector<Coordinate> playerMapCoordinates;
    for (const auto& match : matches) {
        Point2f sourcePoint = mapKeypoints[match.queryIdx].pt;
        Point2f destinationPoint = minMapKeypoints[match.trainIdx].pt;
        playerMapCoordinates.push_back(CalculatePlayerImgMapCoordinate(minMapImage,sourcePoint, destinationPoint));
    }
    playerMapCoordinates = FilterPlaerImgMapCoordinates(playerMapCoordinates, lastCoordinate,maxDistance);
    
    if (playerMapCoordinates.size() == 0)
        return false;

    Coordinate playerMapCoordinate = CalculateMedianCoordinate(playerMapCoordinates);
    outPlayerMapCoordinate = playerMapCoordinate;
    return true;
}


Coordinate MapCoordinate::PlayerWorldCoordToImgMapCoord(Coordinate WorldCoordinate) {
    Coordinate PlayerMapCoor(WorldCoordinate.x * 1.205 + WorldOriginCoordinates::x, WorldCoordinate.y * 1.205 + WorldOriginCoordinates::y);
    return PlayerMapCoor;
}

Coordinate MapCoordinate::PlayerTethysCoordToImgMapCoord(Coordinate TethysCoordinate) {
    Coordinate PlayerMapCoor(TethysCoordinate.x * 1.205 + TethysOriginCoordinates::x, TethysCoordinate.y * 1.205 + TethysOriginCoordinates::y);
    return PlayerMapCoor;
}

Coordinate MapCoordinate::PlayerFabricatoriumCoordToImgMapCoord(Coordinate FabricatoriumCoordinate) {
    Coordinate PlayerMapCoor(FabricatoriumCoordinate.x * 1.205 + FabricatoriumOriginCoordinates::x, FabricatoriumCoordinate.y * 1.205 + FabricatoriumOriginCoordinates::y);
    return PlayerMapCoor;
}

Coordinate MapCoordinate::IdentifyCoorToImgMapCoord(Coordinate identifyCoordinate,int sceneId) {
    if (sceneId == 1) {
       return PlayerWorldCoordToImgMapCoord(identifyCoordinate);
    }

    if (sceneId == 2) {
        return PlayerTethysCoordToImgMapCoord(identifyCoordinate);
    }

    if (sceneId == 3) {
        return PlayerFabricatoriumCoordToImgMapCoord(identifyCoordinate);
    }

    return Coordinate(0, 0);
}

//截取游戏中心的地图区域(350*350),在通过特征匹配，取角点，算出截取区域中心点在MapImage的坐标
bool MapCoordinate::GetMapCoordinateOfCenterGameMapPos(const ImageFeatureData& originalFeatureData, const ImageFeatureData& testFeatureData, const vector<DMatch>& goodMatches, Mat& testIame,Coordinate& outResult) {
    if (goodMatches.size()<4) {
        return false;
    }

    vector<Point2f> scene_corners = FeatureMatch::GetTestImageCornersInOriginal(originalFeatureData, testFeatureData, goodMatches, testIame);

    if (scene_corners.empty()) {
        return false;
    }

outResult.x = (scene_corners[0].x + scene_corners[2].x) / 2;
outResult.y = (scene_corners[0].y + scene_corners[2].y) / 2;

return true;
}


bool MapCoordinate::GetMapCoordinateOfCenterGameMapPos(const ImageFeatureData& originalFeatureData, const ImageFeatureData& testFeatureData, const vector<DMatch>& goodMatches, Mat& testIame, Coordinate& outResult, vector<Point2f>& outSceneCorners) {
    if (goodMatches.size() < 4) {
        return false;
    }

    vector<Point2f> scene_corners = FeatureMatch::GetTestImageCornersInOriginal(originalFeatureData, testFeatureData, goodMatches, testIame);

    if (scene_corners.empty()) {
        return false;
    }

    float areaWidth = scene_corners[2].x - scene_corners[0].x;
    float areaHigth = scene_corners[2].y - scene_corners[0].y;

    //判断区域是否为正方形
    if (areaWidth > 0 && areaHigth > 0) {
        if (abs(areaWidth - areaHigth) < 4) {
            outResult.x = (scene_corners[0].x + scene_corners[2].x) / 2;
            outResult.y = (scene_corners[0].y + scene_corners[2].y) / 2;

            outSceneCorners = scene_corners;
            return true;
        }
    }
    return false;
}


Coordinate MapCoordinate::CalculateMouseClickPositionMapCoordinate(const HWND gameHwnd, const POINT clickPosScreenPoint, const Coordinate centerMapScreenCoordinate, vector<Point2f> captureCorners) {
    WindowCorners gameWindowCorners = GetWindowClientCorners(gameHwnd);
    POINT gameWindowCenterPos = { (gameWindowCorners.topLeft.x + gameWindowCorners.topRight.x) / 2,
        (gameWindowCorners.topLeft.y + gameWindowCorners.bottomLeft.y) / 2 };

    if (captureCorners.empty()) {
        return Coordinate(0, 0);
    }

    double HorizontalFactor = 0;//水平缩放因子
    double VerticaFactor = 0;//垂直缩放因子

    CalculateWindowScalingFactors(gameHwnd, HorizontalFactor, VerticaFactor);

    float scalingFactor = (captureCorners[2].x - captureCorners[0].x) / (HorizontalFactor*(GameWindowsScreenData::mapCenterArea_Right.x - GameWindowsScreenData::mapCenterArea_Left.x));

    double clickPosScreenCoor_x = centerMapScreenCoordinate.x + scalingFactor * (clickPosScreenPoint.x - gameWindowCenterPos.x);
    double clickPosScreenCoor_y = centerMapScreenCoordinate.y + scalingFactor * (clickPosScreenPoint.y - gameWindowCenterPos.y);

    return Coordinate(clickPosScreenCoor_x, clickPosScreenCoor_y);
}


Coordinate MapCoordinate::CalculateGameMapCenterCoordinateByMouseLocation(const HWND gameHwnd, const POINT clickPosScreenPoint, const Coordinate clickScreenPointMapCoordinate, vector<Point2f> captureCorners) {
    WindowCorners gameWindowCorners = GetWindowClientCorners(gameHwnd);
    POINT gameWindowCenterPos = { (gameWindowCorners.topLeft.x + gameWindowCorners.topRight.x) / 2,
        (gameWindowCorners.topLeft.y + gameWindowCorners.bottomLeft.y) / 2 };

    if (captureCorners.empty()) {
        return Coordinate(0, 0);
    }

    POINT clickPosScreenPoint_temp = clickPosScreenPoint;

    if (clickPosScreenPoint.x < gameWindowCorners.bottomLeft.x && clickPosScreenPoint.y < gameWindowCorners.bottomLeft.y) {

    }
    if (clickPosScreenPoint.x > gameWindowCorners.bottomRight.x) {
        clickPosScreenPoint_temp = { gameWindowCorners.bottomRight.x,clickPosScreenPoint.y };
    }

    if (clickPosScreenPoint.y > gameWindowCorners.bottomRight.y) {
        clickPosScreenPoint_temp = { clickPosScreenPoint.x,gameWindowCorners.bottomRight.y };
    }

    auto MapCenterAreaData = ScreenCoordinate::SpecifyScreenCoordinate(gameHwnd, GameWindowsScreenData::mapCenterAreaSrceenData);
    float scalingFactor = (captureCorners[2].x - captureCorners[0].x) / (MapCenterAreaData.rightPoint.x - MapCenterAreaData.leftPoint.x);

    Coordinate centerGameMapPosCoordinate(clickScreenPointMapCoordinate.x - scalingFactor * (clickPosScreenPoint_temp.x - gameWindowCenterPos.x), clickScreenPointMapCoordinate.y - scalingFactor * (clickPosScreenPoint_temp.y - gameWindowCenterPos.y));

    return centerGameMapPosCoordinate;
}


float CalculateInertiaStep(const HWND& gameHwnd,const vector<Point2f> captureCorners) {
    double HorizontalFactor = 0;//水平缩放因子
    double VerticaFactor = 0;//垂直缩放因子

    CalculateWindowScalingFactors(gameHwnd, HorizontalFactor, VerticaFactor);
    return (captureCorners[2].x - captureCorners[0].x - ((GameWindowsScreenData::mapCenterArea_Right.x - GameWindowsScreenData::mapCenterArea_Left.x))) / 16.75 + 18.78;
}


void MapCoordinate::CalculateInertialslidingCoordinate(const HWND& gameHwnd, const POINT& velocity, const vector<Point2f> captureCorners, Coordinate& mapCenterCoordinateByMouseMonitoring) {
    float inertiaStep = CalculateInertiaStep(gameHwnd,captureCorners);

    if (abs(velocity.x) > 0.1) {
        mapCenterCoordinateByMouseMonitoring.x += (velocity.x > 0 ? 1 : -1) * inertiaStep;
    }

    if (abs(velocity.y) > 0.1){
        mapCenterCoordinateByMouseMonitoring.y += (velocity.y > 0 ? 1 : -1) * inertiaStep;
    }
}
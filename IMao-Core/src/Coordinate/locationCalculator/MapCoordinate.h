#pragma once
#include <opencv2/opencv.hpp>
#include "Windows.h"
#include "../CoordinateStruct.h"
#include "../../Feature/Match/FeatureMatch.h"

class MapCoordinate
{
    public:
       static bool GetGoodPlayerImgMapCoordinateFromMatches(const cv::Mat& minMapImage,const std::vector<cv::DMatch>& matches, const std::vector<cv::KeyPoint>& mapKeypoints, const std::vector<cv::KeyPoint>& minMapKeypoints, float maxDistance, const Coordinate& lastCoordinate, Coordinate& outPlayerMapCoordinate);
       static Coordinate PlayerWorldCoordToImgMapCoord(Coordinate WorldCoordinate);

       static Coordinate PlayerTethysCoordToImgMapCoord(Coordinate TethysCoordinate);

       static Coordinate PlayerFabricatoriumCoordToImgMapCoord(Coordinate FabricatoriumCoordinate);

       static Coordinate IdentifyCoorToImgMapCoord(Coordinate playerCoordinate, int sceneId);

       static bool GetMapCoordinateOfCenterGameMapPos(const ImageFeatureData& originalFeatureData, const ImageFeatureData& testFeatureData, const std::vector<cv::DMatch>& goodMatches, cv::Mat& testIame, Coordinate& outResult);
       static bool GetMapCoordinateOfCenterGameMapPos(const ImageFeatureData& originalFeatureData, const ImageFeatureData& testFeatureData, const std::vector<cv::DMatch>& goodMatches, cv::Mat& testIame, Coordinate& outResult, std::vector<cv::Point2f>& outSceneCorners);

       static Coordinate CalculateMouseClickPositionMapCoordinate(const HWND gameHwnd, const POINT clickPosScreenPoint, const Coordinate centerMapScreenCoordinate, std::vector<cv::Point2f> captureCorners);
       static Coordinate CalculateGameMapCenterCoordinateByMouseLocation(const HWND gameHwnd, const POINT clickPosScreenPoint, const Coordinate clickScreenPointMapCoordinate, std::vector<cv::Point2f> captureCorners);

       static void CalculateInertialslidingCoordinate(const HWND& gameHwnd, const POINT& velocity, const std::vector<cv::Point2f> captureCorners, Coordinate& mapCenterCoordinateByMouseMonitoring);
};
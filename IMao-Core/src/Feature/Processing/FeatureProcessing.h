#pragma once
#include <string>
#include <vector>
#include <opencv2/opencv.hpp>
#include "../Match/FeatureMatch.h"

class FeatureLoader
{
	public:
		static bool loadFeaturesFromXML(const std::string& filename, ImageFeatureData& imageFeatureData);
		static bool loadFeatures(const std::string& filename, ImageFeatureData& imageFeatureData);
};

class FeatureFilter {
	public:
		static void FilterNearKeypoints(const std::vector<cv::KeyPoint> inputKeypoints, const cv::Mat inputDescriptors,
			const cv::Point2f palyerPoint, int searchRadius,
			std::vector<cv::KeyPoint>& outputKeypoints, cv::Mat& outputDescriptors);

		static void FilterNearGoodKeypoints(const std::vector<cv::KeyPoint>& inputKeypoints, const cv::Mat& inputDescriptors, const cv::Point2f palyerPoint, int searchRadius, float sizeThreshold, std::vector<cv::KeyPoint>& outputKeypoints, cv::Mat& outputDescriptors);

		static std::vector<cv::DMatch> FilterGoodMatchesByRatioAndDistance(std::vector<std::vector<cv::DMatch>> knnMatches, float ratioThresh, float maxDist);
};


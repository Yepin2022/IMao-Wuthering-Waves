#pragma once
#include <opencv2/opencv.hpp>
#include "../../ImageProcessing/ImageProcessing.h"
#include <opencv2/xfeatures2d.hpp> 

struct ImageFeatureData {
    std::vector<cv::KeyPoint> imgKeypoints;
    cv::Mat imgDescriptors;

    // 默认构造函数
    ImageFeatureData() = default;

    // 原有构造函数
    ImageFeatureData(const std::vector<cv::KeyPoint>& keypoints, const cv::Mat& descriptors)
        : imgKeypoints(keypoints), imgDescriptors(descriptors) {
    }

    void Release() {
        std::vector<cv::KeyPoint>().swap(imgKeypoints);
        imgDescriptors.release();
    }
};

class FeatureMatch
{
	public:
		static std::vector<cv::DMatch> FindGoodMatchesBetweenMapAndMinMap(const ImageFeatureData& minMapFeatureData, const ImageFeatureData& mapFeatureData);
		static std::vector<cv::Point2f> GetTestImageCornersInOriginal(const ImageFeatureData& originalFeatureData, const ImageFeatureData& testFeatureData, const std::vector<cv::DMatch>& goodMatches, cv::Mat& testIame);
		static ImageFeatureData ExtractSurfFeatures(const cv::Mat& img);
		static ImageFeatureData ExtractSurfFeatures(cv::Ptr<cv::xfeatures2d::SURF>& surt, const cv::Mat& img);
		static std::vector<cv::DMatch> FindGoodMatches(const ImageFeatureData& originalFeatureData, const ImageFeatureData& testFeatureData, float ratioThresh, float maxDist, const cv::DescriptorMatcher::MatcherType& matcherType);
		static std::vector<cv::DMatch> FindGoodMatchesFLANN(const ImageFeatureData& originalFeatureData, const ImageFeatureData& testFeatureData, float ratioThresh, float maxDist);
};


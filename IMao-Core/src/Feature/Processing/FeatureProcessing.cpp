#include "FeatureProcessing.h"
using namespace cv;
using namespace std;

bool FeatureLoader::loadFeatures(const string& filename, vector<KeyPoint>& keypoints, Mat& descriptors) {
    FileStorage fs(filename, FileStorage::READ);
    if (fs.isOpened()) {
        fs["keypoints"] >> keypoints;
        fs["descriptors"] >> descriptors;
        fs.release();
        return true;
    }
    return false;
}

bool FeatureLoader::loadFeatures(const string& filename, ImageFeatureData& imageFeatureData) {
    FileStorage fs(filename, FileStorage::READ);
    if (fs.isOpened()) {
        fs["keypoints"] >> imageFeatureData.imgKeypoints;
        fs["descriptors"] >> imageFeatureData.imgDescriptors;
        fs.release();
        return true;
    }
    return false;
}


// 筛选当前位置附近的特征点
void FeatureFilter::FilterNearKeypoints(const std::vector<cv::KeyPoint> inputKeypoints, const cv::Mat inputDescriptors,
    const cv::Point2f palyerPoint, int searchRadius,
    std::vector<cv::KeyPoint>& outputKeypoints, cv::Mat& outputDescriptors) {
    
    outputKeypoints.clear();
    outputDescriptors.release();
    for (size_t i = 0; i < inputKeypoints.size(); ++i) {
        cv::Point2f currentPoint = inputKeypoints[i].pt;
        if (cv::norm(currentPoint - palyerPoint) <= searchRadius) {
            outputKeypoints.push_back(inputKeypoints[i]);
            outputDescriptors.push_back(inputDescriptors.row(i));
        }
    }
}

// 比较函数，用于按特征点质量从高到低排序
bool compareKeypoints(const cv::KeyPoint& kp1, const cv::KeyPoint& kp2) {
    return kp1.response > kp2.response;
}

void FeatureFilter::FilterNearGoodKeypoints(const vector<KeyPoint>& inputKeypoints, const Mat& inputDescriptors, const Point2f palyerPoint, int searchRadius,float sizeThreshold, vector<KeyPoint>& outputKeypoints, Mat& outputDescriptors) {
    
    outputKeypoints.clear();
    outputDescriptors.release();
    for (size_t i = 0; i < inputKeypoints.size(); ++i) {
        cv::Point2f currentPoint = inputKeypoints[i].pt;
        if (cv::norm(currentPoint - palyerPoint) <= searchRadius) {
            if (inputKeypoints[i].size > sizeThreshold) {
                outputKeypoints.push_back(inputKeypoints[i]);
                outputDescriptors.push_back(inputDescriptors.row(i));
            }
        }
    }
}

vector<DMatch> FeatureFilter::FilterGoodMatchesByRatioAndDistance(vector<vector<DMatch>> knnMatches, float ratioThresh,float maxDist) {
    vector<DMatch> goodMatches;
    for (size_t i = 0; i < knnMatches.size(); i++) {
        if (knnMatches[i][0].distance < ratioThresh * knnMatches[i][1].distance && knnMatches[i][0].distance < maxDist) {
            goodMatches.push_back(knnMatches[i][0]);
        }
    }
    return goodMatches;
}
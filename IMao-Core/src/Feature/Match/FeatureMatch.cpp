#include "FeatureMatch.h"
#include "../Processing/FeatureProcessing.h"
using namespace std;
using namespace cv;
using namespace cv::xfeatures2d;

ImageFeatureData FeatureMatch::ExtractSurfFeatures(const Mat& img) {
    Ptr<SURF> surt = SURF::create(40, 8, 4, true, true);

    Mat imgGray = ImageProcessing::imgToGray(img);

    vector<KeyPoint> imgKeypoints;
    Mat imgDescriptors;
    surt->detectAndCompute(imgGray, noArray(), imgKeypoints, imgDescriptors);
    ImageFeatureData imageFeatureData = ImageFeatureData(imgKeypoints, imgDescriptors);
    return imageFeatureData;
}

ImageFeatureData FeatureMatch::ExtractSurfFeatures(Ptr<SURF>& surt,const Mat& img) {
	Mat imgGray = ImageProcessing::imgToGray(img);

	vector<KeyPoint> imgKeypoints;
	Mat imgDescriptors;
	surt->detectAndCompute(imgGray, noArray(), imgKeypoints, imgDescriptors);
    if (!imgKeypoints.empty() and !imgDescriptors.empty()) {
        ImageFeatureData imageFeatureData = ImageFeatureData(imgKeypoints, imgDescriptors);
        return imageFeatureData;
    }
    return ImageFeatureData();
}

vector<DMatch> FeatureMatch::FindGoodMatches(const ImageFeatureData& originalFeatureData, const ImageFeatureData& testFeatureData, float ratioThresh, float maxDist, const DescriptorMatcher::MatcherType& matcherType) {

    auto matcher = DescriptorMatcher::create(matcherType);
    vector<vector<DMatch>> knnMatches;
    try {
        matcher->knnMatch(originalFeatureData.imgDescriptors, testFeatureData.imgDescriptors, knnMatches, 2);
    }
    catch (const cv::Exception& e) {
        std::cerr << "knnMatch 函数抛出异常: " << e.what() << std::endl;
        vector<DMatch> err;
        return err;
    }

    vector<DMatch> goodMatches;
    goodMatches = FeatureFilter::FilterGoodMatchesByRatioAndDistance(knnMatches, ratioThresh, maxDist);
    return goodMatches;
}

vector<DMatch> FeatureMatch::FindGoodMatchesFLANN(const ImageFeatureData& originalFeatureData, const ImageFeatureData& testFeatureData, float ratioThresh, float maxDist) {

    FlannBasedMatcher matcher;

    vector<vector<DMatch>> knnMatches;
    try {
        matcher.knnMatch(originalFeatureData.imgDescriptors, testFeatureData.imgDescriptors, knnMatches, 2);
    }
    catch (const cv::Exception& e) {
        std::cerr << "knnMatch 函数抛出异常: " << e.what() << std::endl;
        vector<DMatch> err;
        return err;
    }

    vector<DMatch> goodMatches;
    goodMatches = FeatureFilter::FilterGoodMatchesByRatioAndDistance(knnMatches, ratioThresh, maxDist);
    return goodMatches;
}

vector<DMatch> FeatureMatch::FindGoodMatchesBetweenMapAndMinMap(const ImageFeatureData& minMapFeatureData, const ImageFeatureData& mapFeatureData){

    return FindGoodMatches(mapFeatureData, minMapFeatureData,0.65f,0.5f, DescriptorMatcher::BRUTEFORCE_SL2);

}


//用筛选后的特征，在原图中找到测试图的角点
vector<Point2f> FeatureMatch::GetTestImageCornersInOriginal(const ImageFeatureData& originalFeatureData, const ImageFeatureData& testFeatureData,const vector<DMatch>& goodMatches,Mat& testIame) {
    vector<Point2f> obj;
    vector<Point2f> scene;
    for (size_t i = 0; i < goodMatches.size(); i++) {
        obj.push_back(testFeatureData.imgKeypoints[goodMatches[i].queryIdx].pt);
        scene.push_back(originalFeatureData.imgKeypoints[goodMatches[i].trainIdx].pt);
    }
    Mat H = findHomography(obj, scene, RANSAC);

    if (H.empty()) {
        cout << "警告: 无法计算有效的单应性矩阵!" << endl;
        return vector<Point2f>(); 
    }

    // 得到测试图的角点
    vector<Point2f> obj_corners(4);
    obj_corners[0] = Point2f(0, 0);
    obj_corners[1] = Point2f(testIame.cols, 0);
    obj_corners[2] = Point2f(testIame.cols, testIame.rows);
    obj_corners[3] = Point2f(0, testIame.rows);
    vector<Point2f> scene_corners(4);

    perspectiveTransform(obj_corners, scene_corners, H);

    return scene_corners;
}

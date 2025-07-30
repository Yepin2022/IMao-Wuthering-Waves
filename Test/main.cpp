#include <opencv2/opencv.hpp> 
#include <opencv2/features2d.hpp> 
#include <opencv2/xfeatures2d.hpp> 
#include <vector>
#include <iostream>
#include <fstream>
#include <chrono>
#include <cmath>
#include <algorithm>
#include <Windows.h>
using namespace cv::xfeatures2d;
using namespace cv;
using namespace std;

int main_1() {
    //Image resource download address：https://github.com/Yepin2022/IMao-WW-Resources
    cv::Mat img = cv::imread("C:\\Map.png", cv::IMREAD_GRAYSCALE);
    if (img.empty()) {
        std::cerr << "无法读取图像" << std::endl;
        return -1;
    }

    cv::Ptr<cv::xfeatures2d::SURF> surf = cv::xfeatures2d::SURF::create(60, 8, 4, true, true);
    std::vector<cv::KeyPoint> allKeypoints;
    cv::Mat allDescriptors;

    surf->detectAndCompute(img, cv::noArray(), allKeypoints, allDescriptors);

    // 按特征点质量从高到低排序
    std::sort(allKeypoints.begin(), allKeypoints.end(), compareKeypoints);

    //划分为 100×100 的网格（单元格）
    int rows = 100;
    int cols = 100;
    int height = img.rows;
    int width = img.cols;
    int cellHeight = height / rows;
    int cellWidth = width / cols;

    std::vector<cv::KeyPoint> selectedKeypoints;
    cv::Mat selectedDescriptors;

    for (int i = 0; i < rows; ++i) {
        for (int j = 0; j < cols; ++j) {
            cv::Rect cell(j * cellWidth, i * cellHeight, cellWidth, cellHeight);
            std::vector<cv::KeyPoint> cellKeypoints;
            cv::Mat cellDescriptors;

            for (size_t k = 0; k < allKeypoints.size(); ++k) {
                if (cell.contains(allKeypoints[k].pt)) {
                    cellKeypoints.push_back(allKeypoints[k]);
                    cv::Mat descRow = allDescriptors.row(k);
                    cellDescriptors.push_back(descRow);
                }
            }

            // 每个网格选取至多 160 个特征点
            int numPoints = std::min(160, static_cast<int>(cellKeypoints.size()));
            for (int k = 0; k < numPoints; ++k) {
                selectedKeypoints.push_back(cellKeypoints[k]);
                cv::Mat descRow = cellDescriptors.row(k);
                selectedDescriptors.push_back(descRow);
            }
        }
    }

    std::string featureFilePath = "C:\\Map_features.yml";
    saveFeatures(featureFilePath, selectedKeypoints, selectedDescriptors);

    std::vector<cv::KeyPoint> loadedKeypoints;
    cv::Mat loadedDescriptors;
    if (loadFeatures(featureFilePath, loadedKeypoints, loadedDescriptors)) {
        std::cout << "成功读取特征点和描述符" << std::endl;
    }
    else {
        std::cerr << "读取特征点和描述符失败" << std::endl;
    }

    return 0;
}

//https://web-static.kurobbs.com/mcmap/tiles/C6AC13F249564BC6A3990E8D067A092C/8/8_0_0.png?x-oss-process=image/format,webp/resize,w_1024,h_1024
int main_2() { 
    int min_x = -20;
    int max_x = 20;
    int min_y = -20;
    int max_y = 20;


    int img_width = 1024;
    int img_height = 1024;

    int total_width = (max_x - min_x + 1) * img_width;
    int total_height = (max_y - min_y + 1) * img_height;

    Mat result(total_height, total_width, CV_8UC3, Scalar(0, 0, 0));

    for (int y = max_y; y >= min_y; --y) {
        for (int x = min_x; x <= max_x; ++x) {  
            string filename = "C:/鸣潮地图/8_" + to_string(x) + "_" + to_string(y) + ".png";

            if (fileExists(filename)) {
                Mat img = imread(filename);

                if (!img.empty()) {
                    int x_offset = (x - min_x) * img_width;
                    int y_offset = (max_y - y) * img_height;

                    img.copyTo(result(Rect(x_offset, y_offset, img_width, img_height)));
                }
            }
        }
    }

    string output_path = "C:/Map.png";
    imwrite(output_path, result);

    return 0;
}
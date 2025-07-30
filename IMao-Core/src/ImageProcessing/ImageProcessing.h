#pragma once
#include <opencv2/opencv.hpp>
#include "../pch.h"
#include <vector>
#include <thread>
#include <opencv2/features2d.hpp> 
#include <Windows.h>
#include "../util.h"

class ImageProcessing {
	public:
		static cv::Mat extractCircularRegionFromImage(cv::Mat img, cv::Point center, int radius);
		static cv::Mat imgToGray(cv::Mat img);
		static cv::Mat cropImageWithRect(cv::Mat img, cv::Rect roi);
		static cv::Mat increaseImageResolution(cv::Mat img, float scaleFactor);
		static cv::Mat centerAndScaleImage(cv::Mat img, double scaleFactor);

		static cv::Mat CropToShowWorldCoordinateAreaImg(const cv::Mat& snapshot, const RECT& w_Rect);
		static cv::Mat CropToShowWorldCoordinateAreaImg(const cv::Mat& snapshot, const HWND hwnd);

		static cv::Mat CropToMinMapAreaImg(const cv::Mat& snapshot, const RECT& w_Rect);
		static cv::Mat CropToMinMapAreaImg(const cv::Mat& snapshot, HWND& hwnd);

		static cv::Mat CropToRegion_IconTask(const cv::Mat& snapshot, const RECT& w_Rect);
		static cv::Mat CropToRegion_IconWavePlateCrystal(const cv::Mat& snapshot, const RECT& w_Rect);
		static cv::Mat CropToMapCenterArea(const cv::Mat& snapshot, const RECT& w_Rect);
		static cv::Mat CropToWindowsClientArea(const cv::Mat& snapshot,const NonClientRegion& nonClientRegion,const RECT& rect);
};
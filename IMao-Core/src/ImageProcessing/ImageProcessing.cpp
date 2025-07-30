#include "ImageProcessing.h"
#include "../Coordinate/locationCalculator/ScreenCoordinate.h"
#include "../util.h"
using namespace cv;

Mat ImageProcessing::extractCircularRegionFromImage(Mat img,Point center,int radius) {
	Mat mask = Mat::zeros(img.size(), CV_8UC1);
	circle(mask, center, radius, Scalar(255), FILLED);
	
	Mat circularRegionImg;
	img.copyTo(circularRegionImg, mask);

	return circularRegionImg;
}

Mat ImageProcessing::imgToGray(Mat img) {
	Mat ImgGray;
	cvtColor(img, ImgGray, COLOR_BGR2GRAY);

	return ImgGray;
}

Mat ImageProcessing::cropImageWithRect(Mat img, Rect roi) {
	Mat croppedImage = img(roi);
	return croppedImage;
}


Mat ImageProcessing::increaseImageResolution(Mat image, float scaleFactor) {
	int newWidth = image.cols * scaleFactor;
	int newHeight = image.rows * scaleFactor;

	cv::Mat resizedImage;

	cv::resize(image, resizedImage, cv::Size(newWidth, newHeight), 0, 0, cv::INTER_CUBIC);

	return resizedImage;
}


Mat ImageProcessing::centerAndScaleImage(Mat img, double scaleFactor) {

	// 设定目标图像尺寸（根据需求调整）
	int targetWidth = 1000;  // 目标宽度
	int targetHeight = 500; // 目标高度
	cv::Mat blackBG(targetHeight, targetWidth, CV_8UC3, cv::Scalar(0, 0, 0)); // 黑色背景

	// 计算粘贴位置（示例为居中，可自定义）
	int x = (targetWidth - img.cols) / 2;
	int y = (targetHeight - img.rows) / 2;

	// 确保裁剪区域尺寸不超过目标尺寸
	if (x >= 0 && y >= 0 && x + img.cols <= targetWidth && y + img.rows <= targetHeight) {
		img.copyTo(blackBG(cv::Rect(x, y, img.cols, img.rows)));
	}

	// 计算缩放中心
	cv::Point2f center(blackBG.cols / 2.0, blackBG.rows / 2.0);

	// 获取仿射变换矩阵，围绕中心缩放
	cv::Mat rotationMatrix = cv::getRotationMatrix2D(center, 0, scaleFactor);

	// 创建一个用于存储缩放后图片的矩阵
	cv::Mat scaledImage;

	// 应用仿射变换
	cv::warpAffine(blackBG, scaledImage, rotationMatrix, blackBG.size(), cv::INTER_NEAREST);

	return scaledImage;
}


Mat ImageProcessing::CropToShowWorldCoordinateAreaImg(const Mat& snapshot, const RECT& w_Rect) {
    Mat showWorldCoordinateAreaImg;

	auto showWorldCoordinateAreaLocationData = ScreenCoordinate::SpecifyScreenCoordinate(w_Rect, GameWindowsScreenData::ShowWorldAreaScreenData);

	Rect roi(showWorldCoordinateAreaLocationData.leftPoint.x,
		 showWorldCoordinateAreaLocationData.topPoint.y, showWorldCoordinateAreaLocationData.rightPoint.x - showWorldCoordinateAreaLocationData.leftPoint.x,
		showWorldCoordinateAreaLocationData.bottomPoint.y - showWorldCoordinateAreaLocationData.topPoint.y);

	showWorldCoordinateAreaImg = ImageProcessing::cropImageWithRect(snapshot, roi);

	return showWorldCoordinateAreaImg;
}


Mat ImageProcessing::CropToShowWorldCoordinateAreaImg(const Mat& snapshot, const HWND hwnd) {
	Mat showWorldCoordinateAreaImg;
	RECT w_Rect;
	GetClientRect(hwnd, &w_Rect);

	showWorldCoordinateAreaImg = CropToShowWorldCoordinateAreaImg(snapshot, w_Rect);

	return showWorldCoordinateAreaImg;
}


Mat ImageProcessing::CropToMinMapAreaImg(const Mat& snapshot, const RECT& w_Rect) {
	Mat circularRegionImg;

	auto minMapLocationData = ScreenCoordinate::SpecifyScreenCoordinate(w_Rect, GameWindowsScreenData::MinMapScreenData);

	Rect roi(minMapLocationData.leftPoint.x,
		 minMapLocationData.topPoint.y,
		minMapLocationData.rightPoint.x - minMapLocationData.leftPoint.x,
		minMapLocationData.bottomPoint.y - minMapLocationData.topPoint.y);
	Mat croppedImage = ImageProcessing::cropImageWithRect(snapshot, roi);
	Point center(croppedImage.rows / 2, croppedImage.cols / 2); int radius = croppedImage.cols / 2 - 3;
	circularRegionImg = ImageProcessing::extractCircularRegionFromImage(croppedImage, center, radius);

	return circularRegionImg;
}


Mat ImageProcessing::CropToMinMapAreaImg(const Mat& snapshot, HWND& hwnd) {
	Mat circularRegionImg;
	RECT w_Rect;
	GetClientRect(hwnd, &w_Rect);

	circularRegionImg = CropToMinMapAreaImg(snapshot, w_Rect);
	return circularRegionImg;
}


Mat ImageProcessing::CropToRegion_IconTask(const Mat& snapshot,const RECT& w_Rect) {

	auto IconTask_SpecifyScreenData = ScreenCoordinate::SpecifyScreenCoordinate(w_Rect, GameWindowsScreenData::IconTask_ScreenData);

	Rect roi(IconTask_SpecifyScreenData.leftPoint.x,
		IconTask_SpecifyScreenData.topPoint.y,
		IconTask_SpecifyScreenData.rightPoint.x - IconTask_SpecifyScreenData.leftPoint.x,
		IconTask_SpecifyScreenData.bottomPoint.y - IconTask_SpecifyScreenData.topPoint.y);
	Mat croppedImage = ImageProcessing::cropImageWithRect(snapshot, roi);

	return croppedImage;
}

Mat ImageProcessing::CropToRegion_IconWavePlateCrystal(const Mat& snapshot, const RECT& w_Rect) {

	auto IconWavePlateCrystal_SpecifyScreenData = ScreenCoordinate::SpecifyScreenCoordinate(w_Rect, GameWindowsScreenData::IconWavePlateCrystal_ScreenData);

	Rect roi(IconWavePlateCrystal_SpecifyScreenData.leftPoint.x,
		IconWavePlateCrystal_SpecifyScreenData.topPoint.y,
		IconWavePlateCrystal_SpecifyScreenData.rightPoint.x - IconWavePlateCrystal_SpecifyScreenData.leftPoint.x,
		IconWavePlateCrystal_SpecifyScreenData.bottomPoint.y - IconWavePlateCrystal_SpecifyScreenData.topPoint.y);
	Mat croppedImage = ImageProcessing::cropImageWithRect(snapshot, roi);
	
	return croppedImage;
}

Mat ImageProcessing::CropToMapCenterArea(const Mat& snapshot, const RECT& w_Rect) {
	
	auto MapCenterAreaData = ScreenCoordinate::SpecifyScreenCoordinate(w_Rect, GameWindowsScreenData::mapCenterAreaSrceenData);

	Rect roi(MapCenterAreaData.leftPoint.x,
		MapCenterAreaData.topPoint.y,
		MapCenterAreaData.rightPoint.x - MapCenterAreaData.leftPoint.x,
		MapCenterAreaData.bottomPoint.y - MapCenterAreaData.topPoint.y);
	Mat croppedImage = ImageProcessing::cropImageWithRect(snapshot, roi);

	return croppedImage;
}

Mat ImageProcessing::CropToWindowsClientArea(const Mat& snapshot,const NonClientRegion& nonClientRegion,const RECT& rect) {
	Rect roi(nonClientRegion.non_client_width_total, nonClientRegion.non_client_height_total, rect.right, rect.bottom);
	Mat croppedImage = ImageProcessing::cropImageWithRect(snapshot,roi);
	return croppedImage;
}